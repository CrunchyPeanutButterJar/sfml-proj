#ifndef ECS_ENTITY_C_SOUND_LISTENER_HPP
#define ECS_ENTITY_C_SOUND_LISTENER_HPP

#include <ecs/entity/c_base.hpp>

namespace ecs::entity
{
class CSoundListener : public CBase
{
  public:
    CSoundListener();
    void readInput(utils::Tokens& l_stream) override;
};
} // namespace ecs::entity
#endif
