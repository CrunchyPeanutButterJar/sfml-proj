#ifndef CORE_AUDIO_SOUND_PROPS_HPP
#define CORE_AUDIO_SOUND_PROPS_HPP

#include <string>
#include <utility>

namespace core::audio
{
struct SoundProps
{
    SoundProps(std::string l_name) : m_audioName(std::move(l_name)) {}
    std::string m_audioName;
    float       m_volume{100};
    float       m_pitch{1.F};
    float       m_minDistance{10.F};
    float       m_attenuation{10.F};
};
} // namespace core::audio
#endif
