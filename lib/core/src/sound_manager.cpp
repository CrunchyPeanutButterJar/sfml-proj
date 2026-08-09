#include <SFML/Audio/Music.hpp>
#include <core/audio/sound_manager.hpp>
#include <core/state/statemanager.hpp>
#include <utils/utilities.hpp>

using namespace core::audio;

SoundManager::SoundManager(AudioManager& l_audioMgr) : m_audioManager(l_audioMgr) {}

SoundManager::~SoundManager()
{
    cleanup();
}

void SoundManager::changeState(state::StateType l_state)
{
    pauseAll(m_currentState);
    unpauseAll(l_state);
    m_currentState = l_state;
}

void SoundManager::removeState(state::StateType l_state)
{
    auto& state_sounds = m_audio.find(l_state)->second;
    for (auto& itr : state_sounds)
    {
        recycleSound(itr.first, std::move(itr.second.second), itr.second.first.m_name);
    }
    m_audio.erase(l_state);
    auto music = m_music.find(l_state);
    if (music == m_music.end())
    {
        return;
    }

    m_music.erase(l_state);
}

void SoundManager::cleanup()
{
    m_audio.clear();
    m_recycled.clear();
    m_music.clear();

    m_properties.clear();
    m_numSounds = 0;
    m_lastID    = 0;
}

void SoundManager::update(float l_dT)
{
    m_elapsed += l_dT;
    if (m_elapsed < 0.33F)
    {
        return;
    }
    // Run once every third of a second.
    m_elapsed       = 0;
    auto& container = m_audio[m_currentState];
    for (auto itr = container.begin(); itr != container.end();)
    {
        if (itr->second.second.first->getStatus() == 0U)
        {
            recycleSound(itr->first, std::move(itr->second.second), itr->second.first.m_name);
            itr = container.erase(itr); // Remove sound.
            continue;
        }
        ++itr;
    }
    auto music = m_music.find(m_currentState);
    if (music == m_music.end())
    {
        return;
    }
    if (music->second.second->getStatus() != 0U)
    {
        return;
    }
    m_music.erase(music);
    --m_numSounds;
}

auto SoundManager::play(const std::string& l_sound, const sf::Vector3f& l_position, bool l_loop,
                        bool l_relative) -> std::optional<SoundID>
{
    SoundProps* props = getSoundProperties(l_sound);
    if (props == nullptr)
    {
        return {};
    } // Failed to load sound properties.
    SoundID id{};
    auto    sound = createSound(id, props->m_audioName);
    if (sound.first == nullptr)
    {
        return {};
    }
    // Sound created successfully.
    setUpSound(sound.first.get(), props, l_loop, l_relative);
    sound.first->setPosition(l_position);
    SoundInfo info(props->m_audioName);
    sound.first->play();
    m_audio[m_currentState].emplace(id, std::make_pair(info, std::move(sound)));
    return id;
}

auto SoundManager::play(const SoundID& l_id) -> bool
{
    auto& container = m_audio[m_currentState];
    auto  sound     = container.find(l_id);
    if (sound == container.end())
    {
        return false;
    }
    sound->second.second.first->play();
    sound->second.first.m_manualPaused = false;
    return true;
}

auto SoundManager::stop(const SoundID& l_id) -> bool
{
    auto& container = m_audio[m_currentState];
    auto  sound     = container.find(l_id);
    if (sound == container.end())
    {
        return false;
    }
    sound->second.second.first->stop();
    sound->second.first.m_manualPaused = true;
    return true;
}

auto SoundManager::pause(const SoundID& l_id) -> bool
{
    auto& container = m_audio[m_currentState];
    auto  sound     = container.find(l_id);
    if (sound == container.end())
    {
        return false;
    }
    sound->second.second.first->pause();
    sound->second.first.m_manualPaused = true;
    return true;
}

auto SoundManager::playMusic(const std::string& l_musicFilePath, float l_volume,
                             bool l_loop) -> bool
{
    auto s =
        m_music.insert_or_assign(m_currentState, std::make_pair(SoundInfo{""}, MusicPtr{})).first;

    if (!s->second.second)
    {
        s->second.second = std::make_unique<sf::Music>();
        ++m_numSounds;
    }

    sf::Music*        music = s->second.second.get();
    const std::string Path  = utils::getResourcesDirectory() + l_musicFilePath;

    if (!music->openFromFile(Path))
    {
        m_music.erase(s);
        --m_numSounds;
        FAILURE_NON_FATAL("Failed to load music from file: {}", Path);
        return false;
    }
    music->setLoop(l_loop);
    music->setVolume(l_volume);
    music->setRelativeToListener(true); // Always relative.
    music->play();
    s->second.first.m_name = l_musicFilePath;
    return true;
}

auto SoundManager::playMusic() -> bool
{
    auto music = m_music.find(m_currentState);
    if (music == m_music.end())
    {
        return false;
    }
    if (!music->second.second)
    {
        return false;
    }
    music->second.second->play();
    music->second.first.m_manualPaused = false;
    return true;
}

auto SoundManager::stopMusic() -> bool
{
    auto music = m_music.find(m_currentState);
    if (music == m_music.end())
    {
        return false;
    }
    music->second.second->stop();
    m_music.erase(music);
    --m_numSounds;
    return true;
}

auto SoundManager::pauseMusic() -> bool
{
    auto music = m_music.find(m_currentState);
    if (music == m_music.end())
    {
        return false;
    }

    music->second.second->pause();
    music->second.first.m_manualPaused = true;
    return true;
}

auto SoundManager::setPosition(const SoundID& l_id, const sf::Vector3f& l_pos) -> bool
{
    auto& container = m_audio[m_currentState];
    auto  sound     = container.find(l_id);
    if (sound == container.end())
    {
        return false;
    }
    sound->second.second.first->setPosition(l_pos);
    return true;
}

auto SoundManager::isPlaying(const SoundID& l_id) -> bool
{
    auto& container = m_audio[m_currentState];
    auto  sound     = container.find(l_id);
    return (sound != container.end() ? sound->second.second.first->getStatus() : 0) != 0;
}

auto SoundManager::getSoundProperties(const std::string& l_soundName) -> SoundProps*
{
    auto properties = m_properties.find(l_soundName);
    if (properties == m_properties.end())
    {
        if (!loadProperties(l_soundName))
        {
            return nullptr;
        }
        properties = m_properties.find(l_soundName);
    }
    return &properties->second;
}

auto SoundManager::loadProperties(const std::string& l_fileName) -> bool
{
    const auto Path = utils::getResourcesDirectory() + "media/sounds/" + l_fileName + ".sound";

    auto file = utils::readFile(Path);
    if (!file.has_value())
    {
        FAILURE_NON_FATAL("Failed to load sound: {}", Path);
        return false;
    }
    SoundProps props("");

    utils::Tokens tokens{std::move(file.value())};
    while (!tokens.empty())
    {
        auto key = *consumeToken<std::string>(tokens);
        if (key == "Audio")
        {
            props.m_audioName = *consumeToken<std::string>(tokens);
        }
        else if (key == "Volume")
        {
            props.m_volume = *consumeToken<float>(tokens);
        }
        else if (key == "Pitch")
        {
            props.m_pitch = *consumeToken<float>(tokens);
        }
        else if (key == "Distance")
        {
            props.m_minDistance = *consumeToken<float>(tokens);
        }
        else if (key == "Attenuation")
        {
            props.m_attenuation = *consumeToken<float>(tokens);
        }
        else
        {
            FAILURE_NON_FATAL("invalid key '{}' in sound file: {}", key, Path);
        }
    }

    if (props.m_audioName.empty())
    {
        return false;
    }
    m_properties.emplace(l_fileName, props);
    return true;
}

void SoundManager::pauseAll(state::StateType l_state)
{
    auto& container = m_audio[l_state];
    for (auto itr = container.begin(); itr != container.end();)
    {
        if (itr->second.second.first->getStatus() == 0U)
        {
            recycleSound(itr->first, std::move(itr->second.second), itr->second.first.m_name);
            itr = container.erase(itr);
            continue;
        }
        itr->second.second.first->pause();
        ++itr;
    }
    auto music = m_music.find(l_state);
    if (music == m_music.end())
    {
        return;
    }

    music->second.second->pause();
}

void SoundManager::unpauseAll(state::StateType l_state)
{
    auto& container = m_audio[l_state];
    for (auto& itr : container)
    {
        if (itr.second.first.m_manualPaused)
        {
            continue;
        }
        itr.second.second.first->play();
    }

    auto music = m_music.find(l_state);
    if (music == m_music.end())
    {
        return;
    }
    if (music->second.first.m_manualPaused)
    {
        return;
    }
    music->second.second->play();
}

auto SoundManager::createSound(SoundID& l_id, const std::string& l_audioName) -> SoundAndSoundBuffer
{
    SoundAndSoundBuffer sound;
    if (!m_recycled.empty() && (m_numSounds >= MAX_SOUNDS || m_recycled.size() >= SOUND_CACHE))
    {
        auto itr = m_recycled.begin();
        while (itr != m_recycled.end())
        {
            if (itr->first.second == l_audioName)
            {
                break;
            }
            ++itr;
        }
        if (itr == m_recycled.end())
        {
            // If a sound with the same name hasn't been found!
            auto element = m_recycled.begin();
            l_id         = element->first.first;
            sound.second = m_audioManager.acquire(l_audioName);
            sound.first  = std::move(element->second.first);
            sound.first->setBuffer(*element->second.second);
            m_recycled.erase(element);
        }
        else
        {
            l_id  = itr->first.first;
            sound = std::move(itr->second);
            m_recycled.erase(itr);
        }
        return sound;
    }
    if (m_numSounds < MAX_SOUNDS)
    {
        if (auto buffer = m_audioManager.acquire(l_audioName))
        {
            sound.first  = std::make_unique<sf::Sound>();
            sound.second = buffer;
            l_id         = m_lastID;
            ++m_lastID;
            ++m_numSounds;
            sound.first->setBuffer(*buffer);
            return sound;
        }
    }
    FAILURE_NON_FATAL("Failed to create sound");
    return {};
}

void SoundManager::setUpSound(sf::Sound* l_snd, const SoundProps* l_props, bool l_loop,
                              bool l_relative)
{
    l_snd->setVolume(l_props->m_volume);
    l_snd->setPitch(l_props->m_pitch);
    l_snd->setMinDistance(l_props->m_minDistance);
    l_snd->setAttenuation(l_props->m_attenuation);
    l_snd->setLoop(l_loop);
    l_snd->setRelativeToListener(l_relative);
}

void SoundManager::recycleSound(SoundID l_id, SoundAndSoundBuffer l_snd, const std::string& l_name)
{
    m_recycled.emplace_back(std::make_pair(l_id, l_name), std::move(l_snd));
}