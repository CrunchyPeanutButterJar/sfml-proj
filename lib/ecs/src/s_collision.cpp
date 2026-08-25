#include "ecs/entity/c_state.fwd.hpp"
#include "ecs/messaging/entity_events.hpp"
#include "ecs/messaging/event_queue.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <ecs/ecs_types.hpp>
#include <ecs/entity/c_collidable.hpp>
#include <ecs/entity/c_movable.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/c_state.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/map.hpp>
#include <ecs/system/s_collision.hpp>
#include <ecs/system/system_manager.hpp>
#include <queue>
#include <utils/assert.hpp>
#include <utils/bitmask.hpp>

namespace ecs::system
{

namespace
{
using CollisionFnId = std::pair<entity::EntityTag, entity::EntityTag>;

std::vector<std::pair<CollisionFnId, CollisionResolutionFun>> s_collision_resolutions;

auto getCollisionFnId(entity::EntityTag l_entity1, entity::EntityTag l_entity2) -> CollisionFnId
{
    auto id = std::make_pair(l_entity1, l_entity2);
    if (id.first > id.second)
    {
        std::swap(id.first, id.second);
    }

    return id;
}

auto seekCollisionFn(entity::EntityTag l_entity1, entity::EntityTag l_entity2)
{
    auto id = getCollisionFnId(l_entity1, l_entity2);
    return std::ranges::find_if(s_collision_resolutions,
                                [id](const auto& el) { return el.first == id; });
}
} // namespace

void registerCollisionResolution(entity::EntityTag l_entity1, entity::EntityTag l_entity2,
                                 CollisionResolutionFun fun)
{
    auto itr = seekCollisionFn(l_entity1, l_entity2);
    if (itr != s_collision_resolutions.end())
    {
        FAILURE_NON_FATAL("Collision resolution is getting overriden");
        s_collision_resolutions.erase(itr);
    }

    s_collision_resolutions.emplace_back(getCollisionFnId(l_entity1, l_entity2), fun);
}

SCollision::SCollision(SystemManager& l_systemManager) : SBase{System::Collision, l_systemManager}
{
    utils::Bitmask req;
    req.turnOnBit((unsigned int)Component::Position);
    req.turnOnBit((unsigned int)Component::Collidable);
    m_requiredComponents.emplace_back(req);
}

void SCollision::setMap(ecs::Map* l_map)
{
    m_map = l_map;
}

static auto isMovingUp(ecs::EntityId l_entity, ecs::system::SystemManager& l_systemManager) -> bool
{
    auto* movable = l_systemManager.getEntityManager().getComponent<ecs::entity::CMovable>(
        l_entity, ecs::Component::Movable);
    return movable != nullptr && movable->getVelocity().y < 0;
}

void SCollision::update(float /*l_dt*/)
{
    if (m_map == nullptr)
    {
        return;
    }

    auto& entities = m_systemManager.getEntityManager();

    for (auto entity : m_entities)
    {
        auto* position = entities.getComponent<entity::CPosition>(entity, Component::Position);
        auto* collidable =
            entities.getComponent<entity::CCollidable>(entity, Component::Collidable);
        collidable->setPosition(position->getPosition());
        collidable->resetCollisionFlags();
    }

    for (size_t i = 0; i < m_entities.size(); i++)
    {
        for (size_t j = i + 1; j < m_entities.size(); j++)
        {
            auto entity1 = m_entities[i];
            auto entity2 = m_entities[j];

            auto* collidable2 =
                entities.getComponent<entity::CCollidable>(m_entities[j], Component::Collidable);
            auto* collidable1 =
                entities.getComponent<entity::CCollidable>(m_entities[i], Component::Collidable);

            auto tag1 = collidable1->getEntityTag();
            auto tag2 = collidable2->getEntityTag();

            auto itr = seekCollisionFn(tag1, tag2);
            if (itr == s_collision_resolutions.end())
            {
                continue;
            }

            const auto& aabb1 = collidable1->getCollidable();
            const auto& aabb2 = collidable2->getCollidable();

            if (aabb1.intersects(aabb2))
            {
                if (tag1 > tag2)
                {
                    std::swap(entity1, entity2);
                    std::swap(tag1, tag2);
                }
                m_entityCollisions.emplace_back(entity1, entity2, tag1, tag2);
            }
        }
    }

    for (auto entity : m_entities)
    {
        auto* position = entities.getComponent<entity::CPosition>(entity, Component::Position);
        auto* collidable =
            entities.getComponent<entity::CCollidable>(entity, Component::Collidable);
        checkOutOfBounds(position, collidable);
        if (auto* state = entities.getComponent<ecs::entity::CState>(entity, ecs::Component::State);
            state != nullptr && state->getState() != entity::EntityState::Flying &&
            !isMovingUp(entity, m_systemManager))
        {
            mapCollisions(entity, position, collidable);
        }

        if (collidable->getGroundTile() == nullptr)
        {
            m_systemManager.addEvent(entity,
                                     (messaging::EventId)messaging::EntityEvent::Not_Grounded);
        }
    }
}

void SCollision::handleEvent(EntityId /*l_entity*/, messaging::EntityEvent /*l_event*/) {}

void SCollision::notify(const messaging::Message& /*l_message*/) {}

void SCollision::lateUpdate()
{
    for (auto [entity1, entity2, tag1, tag2] : m_entityCollisions)
    {
        seekCollisionFn(tag1, tag2)->second(entity1, entity2, m_map->getContext());
    }

    m_entityCollisions.clear();
}

namespace
{
struct CollisionElement
{
    float                       m_width{};
    float                       m_height{};
    const core::graphics::Tile* m_tile{};
    sf::FloatRect               m_tileBounds;

    [[nodiscard]] auto getArea() const -> float { return m_width * m_height; }
};

struct HasSmallerArea
{
    auto operator()(const CollisionElement& lhs, const CollisionElement& rhs) const -> bool
    {
        return lhs.getArea() < rhs.getArea();
    }
};

using Collisions =
    std::priority_queue<CollisionElement, std::vector<CollisionElement>, HasSmallerArea>;
} // namespace

void SCollision::mapCollisions(EntityId l_entity, entity::CPosition* l_position,
                               entity::CCollidable* l_collidable)
{
    const auto& tile_sheet_config = m_map->getTileSheetConfig();
    const int   TileSize          = tile_sheet_config.m_tileSize;

    Collisions collisions;

    const auto& entity_aabb = l_collidable->getCollidable();

    const int FromX = floor(entity_aabb.left / TileSize);
    const int ToX   = floor((entity_aabb.left + entity_aabb.width) / TileSize);
    const int FromY = floor(entity_aabb.top / TileSize);
    const int ToY   = floor((entity_aabb.top + entity_aabb.height) / TileSize);

    for (int i_col = FromX; i_col <= ToX; i_col++)
    {
        for (int i_row = FromY; i_row <= ToY; i_row++)
        {
            if (const auto* tile = m_map->getTile(i_row, i_col))
            {
                const sf::FloatRect TileAABB{
                    static_cast<float>(i_col * TileSize), static_cast<float>(i_row * TileSize),
                    static_cast<float>(TileSize), static_cast<float>(TileSize)};
                sf::FloatRect intersection;
                entity_aabb.intersects(TileAABB, intersection);
                collisions.push(CollisionElement{.m_width      = intersection.width,
                                                 .m_height     = intersection.height,
                                                 .m_tile       = tile,
                                                 .m_tileBounds = TileAABB});
            }
        }
    }

    while (!collisions.empty())
    {
        const auto Collision = collisions.top();
        collisions.pop();
        sf::FloatRect intersection;
        if (!entity_aabb.intersects(Collision.m_tileBounds, intersection))
        {
            continue;
        }

        const auto& tile_bounds = Collision.m_tileBounds;

        if (intersection.width < intersection.height)
        {
            float resolve = 0.;

            if (entity_aabb.left < tile_bounds.left)
            {
                resolve = -intersection.width;
            }
            else if (entity_aabb.left > tile_bounds.left)
            {
                resolve = intersection.width;
            }

            l_position->moveBy({resolve, 0.});
            l_collidable->setPosition(l_position->getPosition());
            m_systemManager.addEvent(l_entity,
                                     (messaging::EventId)messaging::EntityEvent::Colliding_X);
            l_collidable->collideOnX();
        }
        else
        {
            float resolve = 0.;

            if (entity_aabb.top > tile_bounds.top)
            {
                resolve = intersection.height;
            }
            else if (entity_aabb.top < tile_bounds.top)
            {
                resolve = -intersection.height;
            }

            l_position->moveBy({0., resolve});
            l_collidable->setPosition(l_position->getPosition());
            m_systemManager.addEvent(l_entity,
                                     (messaging::EventId)messaging::EntityEvent::Colliding_Y);
            if (Collision.m_tileBounds.top > entity_aabb.top)
            {
                l_collidable->collideOnY(Collision.m_tile);
            }
            else
            {
                l_collidable->collideOnY(nullptr);
            }
        }
    }
}

void SCollision::checkOutOfBounds(entity::CPosition* position, entity::CCollidable* collidable)
{
    ASSERT_DEBUG_BUILD(m_map != nullptr, "");

    const auto TileSheetConfig         = m_map->getTileSheetConfig();
    const auto TileSize                = TileSheetConfig.m_tileSize;
    const auto [NbOfRows, NbOfColumns] = m_map->getMapSize();

    auto [x, y] = position->getPosition();

    if (x < 0.)
    {
        x = 0.;
        position->setPosition({x, y});
        collidable->setPosition(position->getPosition());
    }
    else if (x > NbOfColumns * TileSize)
    {
        x = static_cast<float>(NbOfColumns * TileSize);
        position->setPosition({x, y});
        collidable->setPosition(position->getPosition());
    }
}

} // namespace ecs::system