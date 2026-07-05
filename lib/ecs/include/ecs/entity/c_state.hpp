#ifndef ECS_ENTITY_C_STATE_HPP
#define ECS_ENTITY_C_STATE_HPP

#include <ecs/entity/c_base.hpp>
#include <ecs/entity/c_state.fwd.hpp>

namespace ecs::entity
{

class CState : public CBase
{
  public:
    CState();

    void readInput(utils::Tokens& l_tokens) override;

    [[nodiscard]] auto getState() const -> EntityState;
    void               setState(EntityState l_state);

  private:
    EntityState m_state{EntityState::Idle};
};
} // namespace ecs::entity

#endif