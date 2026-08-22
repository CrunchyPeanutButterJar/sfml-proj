#include <ecs/entity/c_sound_emitter.hpp>
#include <sstream>
#include <utils/utilities.hpp>

using namespace ecs::entity;

CSoundEmitter::CSoundEmitter() : CBase(Component::SoundEmitter) {};

void CSoundEmitter::readInput(utils::Tokens& l_tokens)
{
    const char MainDelimiter  = ':';
    const char FrameDelimiter = ',';
    auto       scope          = l_tokens.setDelimiterScoped('\n');

    auto          line_str = *consumeToken<std::string>(l_tokens);
    utils::Tokens line{std::istringstream{std::move(line_str)}, MainDelimiter};

    size_t i_sound = 0;

    while (!line.empty())
    {
        if (i_sound >= m_params.max_size())
        {
            FAILURE_NON_FATAL("SoundEmitter has too many entity sounds! Max is {}",
                              MAX_ENTITY_SOUNDS);
            return;
        }

        auto default_delimiter_scope = line.setDelimiterScoped(MainDelimiter);

        std::string sound = *consumeToken<std::string>(line);

        m_params[i_sound].m_sound = sound;

        auto          scope      = line.setDelimiterScoped('"');
        auto          frames_str = *consumeToken<std::string>(line);
        utils::Tokens frames{std::istringstream{std::move(frames_str)}, FrameDelimiter};

        size_t i_frame = 0;
        while (!frames.empty())
        {
            if (i_frame >= m_params[i_sound].m_frames.max_size())
            {
                FAILURE_NON_FATAL("Too many sound frames! max is {}",
                                  SoundParameters::MAX_SOUND_FRAMES);
                break;
            }
            auto frame_id                         = *consumeToken<unsigned int>(frames);
            m_params[i_sound].m_frames[i_frame++] = frame_id;
        }

        i_sound++;
    }
}

auto CSoundEmitter::getSoundId() -> std::optional<SoundID>
{
    return m_soundID;
}
void CSoundEmitter::setSoundId(std::optional<SoundID> l_id)
{
    m_soundID = l_id;
}

auto CSoundEmitter::getSound(EntitySound l_snd) -> std::string
{
    static std::string empty;

    return ((int)l_snd < MAX_ENTITY_SOUNDS ? m_params[(int)l_snd].m_sound : empty);
}

auto CSoundEmitter::isSoundFrame(EntitySound l_snd, int l_frame) -> bool
{
    if ((int)l_snd >= MAX_ENTITY_SOUNDS)
    {
        return false;
    }
    for (int i = 0; i < SoundParameters::MAX_SOUND_FRAMES; ++i)
    {
        if (!m_params[(int)l_snd].m_frames[i].has_value())
        {
            return false;
        }
        if (m_params[(int)l_snd].m_frames[i] == l_frame)
        {
            return true;
        }
    }
    return false;
}