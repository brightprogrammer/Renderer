#include "KeyboardState.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

// check if a keyboard key is pressed or released
bool checkKeyEvt(const SDL_KeyboardEvent& evt, const SDL_KeyCode& keycode){
    return evt.keysym.sym == keycode;
}

// get keyboard state from given event
bool KeyboardState::getState(const SDL_Event &event){
    if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP){
        KeyState keyState = event.type == SDL_KEYDOWN ? KeyState::Pressed : KeyState::Released;
        SDL_KeyboardEvent kbdEvt = event.key;

        if(checkKeyEvt(kbdEvt, SDLK_a)) keyA = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_b)) keyB = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_c)) keyC = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_d)) keyD = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_e)) keyE = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_f)) keyF = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_g)) keyG = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_h)) keyH = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_i)) keyI = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_j)) keyJ = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_k)) keyK = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_l)) keyL = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_m)) keyM = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_n)) keyN = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_o)) keyO = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_p)) keyP = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_q)) keyQ = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_r)) keyR = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_s)) keyS = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_t)) keyT = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_u)) keyU = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_v)) keyV = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_w)) keyW = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_x)) keyX = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_y)) keyY = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_z)) keyZ = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_1)) key1 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_2)) key2 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_3)) key3 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_4)) key4 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_5)) key5 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_6)) key6 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_7)) key7 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_8)) key8 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_9)) key9 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_0)) key0 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_1)) numPad1 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_2)) numPad2 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_3)) numPad3 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_4)) numPad4 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_5)) numPad5 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_6)) numPad6 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_7)) numPad7 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_8)) numPad8 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_9)) numPad9 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_0)) numPad0 = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_ESCAPE)) keyEscape = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_TAB)) keyTab = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_LCTRL)) keyLeftCtrl = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_RCTRL)) keyRightCtrl = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_LALT)) keyLeftAlt = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_RALT)) keyRightAlt = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_CAPSLOCK)) keyCapsLock = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_RETURN)) keyEnter = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_KP_ENTER)) numPadEnter = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_RSHIFT)) keyRightShift = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_LSHIFT)) keyLeftShift = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_LEFT)) keyLeft = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_RIGHT)) keyRight = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_UP)) keyUp = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_DOWN)) keyDown = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_BACKSPACE)) keyBackSpace = keyState;
        else if(checkKeyEvt(kbdEvt, SDLK_SPACE)) keySpace = keyState;

        // we processed the event
        return true;
    }else{
        // this wasn't a keyboard event
        return false;
    }
}
