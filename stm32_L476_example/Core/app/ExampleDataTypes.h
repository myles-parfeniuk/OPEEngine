#pragma once

// data type to represent events created by a button
enum class ButtonEvent
{
    quick_press,
    long_press,
    held,
    release
};

static constexpr const char *ButtonEvent_to_str(ButtonEvent evt)
{
    switch (evt)
    {
    case ButtonEvent::quick_press:
        return "quick_press";
    case ButtonEvent::long_press:
        return "long_press";
    case ButtonEvent::held:
        return "held";
    case ButtonEvent::release:
        return "release";
    default:
        return "unknown";
    }
}