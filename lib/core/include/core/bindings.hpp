#ifndef CORE_BINDINGS_HPP
#define CORE_BINDINGS_HPP

#include <string>
#include <type_traits>
#include <utils/phantomtype.hpp>
#include <variant>

namespace core
{
using KeyPressedEnumType         = int;
using MouseButtonPressedEnumType = int;

using KeyPressed  = utils::PhantomType<KeyPressedEnumType, struct KeyPressedTag>;
using KeyReleased = utils::PhantomType<KeyPressedEnumType, struct KeyReleasedTag>;

using MouseButtonPressed = utils::PhantomType<KeyPressedEnumType, struct MouseButtonPressedTag>;

using MouseButtonReleased = utils::PhantomType<KeyPressedEnumType, struct MouseButtonReleasedTag>;

struct Closed
{
};
struct MouseMoved
{
};

struct MouseWheelScrolled
{
};

struct TextEntered
{
};

struct WindowResized
{
};

inline auto operator==(Closed /*unused*/, Closed /*unused*/) -> bool
{
    return true;
}
inline auto operator==(MouseMoved /*unused*/, MouseMoved /*unused*/) -> bool
{
    return true;
}

inline auto operator==(MouseWheelScrolled /*unused*/, MouseWheelScrolled /*unused*/) -> bool
{
    return true;
}

inline auto operator==(TextEntered /*unused*/, TextEntered /*unused*/) -> bool
{
    return true;
}

inline auto operator==(WindowResized /*unused*/, WindowResized /*unused*/) -> bool
{
    return true;
}

// GUI events

using GuiEvent = std::pair<std::string, std::string>; // interface - element

using GuiEventClick   = utils::PhantomType<GuiEvent, struct GuiEventClickParam>;
using GuiEventRelease = utils::PhantomType<GuiEvent, struct GuiEventReleaseParam>;
using GuiEventHover   = utils::PhantomType<GuiEvent, struct GuiEventHoverParam>;
using GuiEventLeave   = utils::PhantomType<GuiEvent, struct GuiEventLeaveParam>;

using SimplifiedEvent =
    std::variant<KeyPressed, KeyReleased, TextEntered, MouseButtonPressed, MouseButtonReleased,
                 MouseMoved, MouseWheelScrolled, Closed, WindowResized, GuiEventClick,
                 GuiEventRelease, GuiEventHover, GuiEventLeave>;

using Action           = std::string;
using SimplifiedEvents = std::vector<SimplifiedEvent>;

using BindingsRegisterFn = std::pair<Action, SimplifiedEvents> (*)();

void registerDefaultCustomizableBinding(BindingsRegisterFn l_fn);
void registerNonCustomizableBinding(BindingsRegisterFn l_fn);

struct Customizable;
struct NonCustomizable;

template <BindingsRegisterFn Fn, typename Tag> class RegisterBinding
{
  public:
    RegisterBinding()
    {
        if (s_init)
        {
            return;
        }

        if constexpr (std::is_same_v<Tag, core::Customizable>)
        {
            registerDefaultCustomizableBinding(Fn);
        }
        else if constexpr (std::is_same_v<Tag, core::NonCustomizable>)
        {
            registerNonCustomizableBinding(Fn);
        }
        else
        {
            static_assert(false, "Invalid Tag");
        }

        s_init = true;
    }

  private:
    static bool s_init;
};

template <BindingsRegisterFn Fn, typename Tag> bool RegisterBinding<Fn, Tag>::s_init = false;

#define BINDING(action, ...)                                                                       \
    +[]() -> std::pair<core::Action, core::SimplifiedEvents>                                       \
    { return std::make_pair(core::Action(action), core::SimplifiedEvents{__VA_ARGS__}); }

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define REGISTER_BINDING(Tag, action, ...)                                                         \
    inline const core::RegisterBinding<BINDING(action __VA_OPT__(, ) __VA_ARGS__), Tag> CONCAT(    \
        reg_, __LINE__)                                                                            \
    {                                                                                              \
    }

} // namespace core
#endif