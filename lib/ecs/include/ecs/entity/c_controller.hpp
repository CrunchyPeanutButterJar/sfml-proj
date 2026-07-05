#ifndef ECS_ENTITY_C_CONTROLLER_HPP
#define ECS_ENTITY_C_CONTROLLER_HPP

#include <ecs/entity/c_base.hpp>

namespace ecs::entity
{
class CController : public CBase
{
  public:
    CController();

    void readInput(utils::Tokens& /*unused*/) override;
};
} // namespace ecs::entity

#endif