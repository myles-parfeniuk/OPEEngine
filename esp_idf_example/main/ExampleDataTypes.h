#pragma once

// data type to represent events created by a button
enum class button_event_t
{
    quick_press,
    long_press,
    held,
    release
};