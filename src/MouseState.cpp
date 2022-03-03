#include "MouseState.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>

bool MouseState::getState(const SDL_Event &event){
    // inMotion = false;
    // xrel = 0; yrel = 0;

    if(event.type == SDL_MOUSEMOTION){ // get mouse position and other things
        x = event.motion.x;
        y = event.motion.y;
        xrel = event.motion.xrel;
        yrel = event.motion.yrel;
        inMotion = true;
    } else if(event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP){ // get mouse clicks
        MouseBtnState btnState = event.type == SDL_MOUSEBUTTONDOWN ? MouseBtnState::Pressed : MouseBtnState::Released;

        if(event.button.button == SDL_BUTTON_LEFT) leftBtn = btnState;
        else if(event.button.button == SDL_BUTTON_RIGHT) rightBtn = btnState;
        else if(event.button.button == SDL_BUTTON_MIDDLE) middleBtn = btnState;
    } else if(event.type == SDL_MOUSEWHEEL){ // get mouse wheel state, scroll direction, count etc...
        // horizontal scroll
        if(x > 0){
            scroll.scrollDir = ScrollDirection::Right;
            scroll.horizontalCount = event.wheel.x;
        }else if(x < 0){
            scroll.scrollDir = ScrollDirection::Left;
            scroll.horizontalCount = -event.wheel.x;
        }

        // vertical scroll
        if(y > 0){
            scroll.scrollDir = ScrollDirection::Up;
            scroll.verticalCount = event.wheel.y;
        }else if(y < 0){
            scroll.scrollDir = ScrollDirection::Down;
            scroll.verticalCount = -event.wheel.y;
        }
    }else
        // this wasnt a mouse event
        return false;

    // this was a mouse event
    return true;
}
