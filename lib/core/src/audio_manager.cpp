#include <core/audio/audio_manager.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

using namespace core::audio;

AudioManager::AudioManager() : ResourceManager("audio.cfg") {}

auto AudioManager::load(const std::string& l_path) -> std::unique_ptr<sf::SoundBuffer>
{
    auto sound = std::make_unique<sf::SoundBuffer>();
    if (!sound->loadFromFile(utils::getResourcesDirectory() + l_path))
    {
        sound.reset();
        FAILURE_NON_FATAL(" Failed to load sound: {}", l_path);
    }
    return sound;
}
