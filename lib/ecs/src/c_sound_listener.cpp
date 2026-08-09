#include <ecs/entity/c_sound_listener.hpp>

using namespace ecs::entity;

CSoundListener::CSoundListener() : CBase{Component::SoundListener} {};

void CSoundListener::readInput(utils::Tokens& /*unused*/) {}