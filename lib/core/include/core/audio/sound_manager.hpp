#ifndef CORE_AUDIO_SOUND_MANAGER_HPP
#define CORE_AUDIO_SOUND_MANAGER_HPP

#include <SFML/Audio.hpp>
#include <core/audio/audio_manager.hpp>
#include <core/audio/sound_manager.fwd.hpp>
#include <core/audio/sound_props.hpp>
#include <core/state/statemanager.fwd.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace core::audio
{

using SoundID = unsigned int;

struct SoundInfo
{
    SoundInfo(std::string l_name) : m_name(std::move(l_name)) {}
    std::string m_name;
    bool        m_manualPaused{false};
};

using SoundProperties     = std::unordered_map<std::string, SoundProps>;
using SoundPtr            = std::unique_ptr<sf::Sound>;
using SoundAndSoundBuffer = std::pair<SoundPtr, std::shared_ptr<sf::SoundBuffer>>;
using SoundContainer      = std::unordered_map<SoundID, std::pair<SoundInfo, SoundAndSoundBuffer>>;
using Sounds              = std::unordered_map<state::StateType, SoundContainer>;
using RecycledSounds = std::vector<std::pair<std::pair<SoundID, std::string>, SoundAndSoundBuffer>>;
using MusicPtr       = std::unique_ptr<sf::Music>;
using MusicContainer = std::unordered_map<state::StateType, std::pair<SoundInfo, MusicPtr>>;

class SoundManager
{
  public:
    SoundManager(AudioManager& l_audioMgr);
    ~SoundManager();

    void changeState(state::StateType l_state);
    void removeState(state::StateType l_state);

    void update(float l_dT);

    auto play(const std::string& l_sound, const sf::Vector3f& l_position, bool l_loop = false,
              bool l_relative = false) -> std::optional<SoundID>;
    auto play(const SoundID& l_id) -> bool;
    auto stop(const SoundID& l_id) -> bool;
    auto pause(const SoundID& l_id) -> bool;

    auto playMusic(const std::string& l_musicFilePath, float l_volume = 100.F,
                   bool l_loop = false) -> bool;
    auto playMusic() -> bool;
    auto stopMusic() -> bool;
    auto pauseMusic() -> bool;

    auto setPosition(const SoundID& l_id, const sf::Vector3f& l_pos) -> bool;
    auto isPlaying(const SoundID& l_id) -> bool;
    auto getSoundProperties(const std::string& l_soundName) -> SoundProps*;

    static const int MAX_SOUNDS  = 150;
    static const int SOUND_CACHE = 75;

  private:
    auto loadProperties(const std::string& l_file) -> bool;
    void pauseAll(state::StateType l_state);
    void unpauseAll(state::StateType l_state);

    auto        createSound(SoundID& l_id, const std::string& l_audioName) -> SoundAndSoundBuffer;
    static void setUpSound(sf::Sound* l_snd, const SoundProps* l_props, bool l_loop = false,
                           bool l_relative = false);
    void        recycleSound(SoundID l_id, SoundAndSoundBuffer l_snd, const std::string& l_name);

    void cleanup();

    Sounds           m_audio;
    MusicContainer   m_music;
    RecycledSounds   m_recycled;
    SoundProperties  m_properties;
    state::StateType m_currentState{};

    SoundID      m_lastID{0};
    unsigned int m_numSounds{0};
    float        m_elapsed{0.F};

    AudioManager& m_audioManager;
};
} // namespace core::audio

#endif