/**
 * @file      MouseState.hpp
 * @brief     Provides an interface to interact with mouse in a better way.
 * @date      Mon Dec  6 12:46:28 2021
 * @author    Siddharth Mishra
 */

#ifndef MOUSE_STATE_HPP
#define MOUSE_STATE_HPP

#include <SDL2/SDL_events.h>

enum class MouseBtnState : bool { Pressed = true, Released = false };

enum class ScrollDirection : uint8_t { Up, Down, Left, Right };

struct ScrollState{
    uint8_t verticalCount, horizontalCount;
    ScrollDirection scrollDir;
};

class MouseState{
public:
    /**
     * @brief get mouse state from given event.
     * @param event to get keyboard state from.
     * @return true if this was a keyboard event, false otherwise.
     */
    bool getState(const SDL_Event& event);

    float x = 0, y = 0;
    float xrel = 0, yrel = 0;
    ScrollState scroll;

    bool inMotion = false;

    MouseBtnState leftBtn = MouseBtnState::Released;
    MouseBtnState rightBtn = MouseBtnState::Released;
    MouseBtnState middleBtn = MouseBtnState::Released;
};

#endif//MOUSE_STATE_HPP
