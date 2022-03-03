/**
 * @file      KeyboardState.hpp
 * @brief     Header of
 * @date      Sun Dec  5 13:48:59 2021
 * @author    Siddharth Mishra
 */

#ifndef KEYBOARD_STATE
#define KEYBOARD_STATE

#include <SDL2/SDL_events.h>

enum class KeyState : bool{
    Pressed = true, Released = false
};

struct KeyboardState{
    /**
     * @brief get key state from given event.
     * @param event to get keyboard state from.
     * @return true if this was a keyboard event, false otherwise.
     */
    bool getState(const SDL_Event& event);

    KeyState keyA = KeyState::Released,
        keyB = KeyState::Released,
        keyC = KeyState::Released,
        keyD = KeyState::Released,
        keyE = KeyState::Released,
        keyF = KeyState::Released,
        keyG = KeyState::Released,
        keyH = KeyState::Released,
        keyI = KeyState::Released,
        keyJ = KeyState::Released,
        keyK = KeyState::Released,
        keyL = KeyState::Released,
        keyM = KeyState::Released,
        keyN = KeyState::Released,
        keyO = KeyState::Released,
        keyP = KeyState::Released,
        keyQ = KeyState::Released,
        keyR = KeyState::Released,
        keyS = KeyState::Released,
        keyT = KeyState::Released,
        keyU = KeyState::Released,
        keyV = KeyState::Released,
        keyW = KeyState::Released,
        keyX = KeyState::Released,
        keyY = KeyState::Released,
        keyZ = KeyState::Released;

    KeyState mouseLeft = KeyState::Released,
        mouseRight = KeyState::Released;

    KeyState key1 = KeyState::Released,
        key2 = KeyState::Released,
        key3 = KeyState::Released,
        key4 = KeyState::Released,
        key5 = KeyState::Released,
        key6 = KeyState::Released,
        key7 = KeyState::Released,
        key8 = KeyState::Released,
        key9 = KeyState::Released,
        key0 = KeyState::Released;

    KeyState keyLeft = KeyState::Released,
        keyRight = KeyState::Released,
        keyUp = KeyState::Released,
        keyDown = KeyState::Released;

    KeyState numPad1 = KeyState::Released,
        numPad2 = KeyState::Released,
        numPad3 = KeyState::Released,
        numPad4 = KeyState::Released,
        numPad5 = KeyState::Released,
        numPad6 = KeyState::Released,
        numPad7 = KeyState::Released,
        numPad8 = KeyState::Released,
        numPad9 = KeyState::Released,
        numPad0 = KeyState::Released;

    KeyState keySpace = KeyState::Released,
        keyTab = KeyState::Released,
        keyCapsLock = KeyState::Released,
        keyLeftShift = KeyState::Released,
        keyLeftCtrl = KeyState::Released,
        keyLeftAlt = KeyState::Released,
        keyReturn = KeyState::Released,
        keyRightShift = KeyState::Released,
        keyRightAlt = KeyState::Released,
        keyRightCtrl = KeyState::Released,
        keyEnter = KeyState::Released,
        numPadEnter = KeyState::Released,
        keyBackSpace = KeyState::Released,
        keyEscape = KeyState::Released;
};

#endif//KEYBOARD_STATE
