#ifndef CORE_AUDIO_AUDIO_MANAGER_HPP
#define CORE_AUDIO_AUDIO_MANAGER_HPP

#include <SFML/Audio/SoundBuffer.hpp>
#include <core/audio/audio_manager.fwd.hpp>
#include <core/resource_manager.hpp>

namespace core::audio
{
class AudioManager : public core::ResourceManager<AudioManager, sf::SoundBuffer>
{
  public:
    AudioManager();

    static auto load(const std::string& l_path) -> std::unique_ptr<sf::SoundBuffer>;
};
} // namespace core::audio

#endif