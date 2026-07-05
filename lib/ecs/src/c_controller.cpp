#include "utils/bitmask.hpp"
#include "utils/utilities.hpp"
#include <ecs/entity/c_controller.hpp>

namespace ecs::entity
{
CController::CController() : CBase{Component::Controller} {}

void CController::readInput(utils::Tokens& /*unused*/) {}

} // namespace ecs::entity