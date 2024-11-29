#pragma once
#include "SDL2/SDL_keycode.h"

namespace FS
{
    class Input
    {
    public:
        /// <summary>
        /// An enum listing all supported keyboard keys.
        /// This uses SDL keycodes directly, allowing seamless integration with SDL input handling.
        /// </summary>
        enum class KeyboardKey
        {
            Space = SDLK_SPACE,
            Apostrophe = SDLK_QUOTE,
            Comma = SDLK_COMMA,
            Minus = SDLK_MINUS,
            Period = SDLK_PERIOD,
            Slash = SDLK_SLASH,
            Digit0 = SDLK_0,
            Digit1 = SDLK_1,
            Digit2 = SDLK_2,
            Digit3 = SDLK_3,
            Digit4 = SDLK_4,
            Digit5 = SDLK_5,
            Digit6 = SDLK_6,
            Digit7 = SDLK_7,
            Digit8 = SDLK_8,
            Digit9 = SDLK_9,
            Semicolon = SDLK_SEMICOLON,
            Equal = SDLK_EQUALS,
            A = SDLK_a,
            B = SDLK_b,
            C = SDLK_c,
            D = SDLK_d,
            E = SDLK_e,
            F = SDLK_f,
            G = SDLK_g,
            H = SDLK_h,
            I = SDLK_i,
            J = SDLK_j,
            K = SDLK_k,
            L = SDLK_l,
            M = SDLK_m,
            N = SDLK_n,
            O = SDLK_o,
            P = SDLK_p,
            Q = SDLK_q,
            R = SDLK_r,
            S = SDLK_s,
            T = SDLK_t,
            U = SDLK_u,
            V = SDLK_v,
            W = SDLK_w,
            X = SDLK_x,
            Y = SDLK_y,
            Z = SDLK_z,
            LeftBracket = SDLK_LEFTBRACKET,
            Backslash = SDLK_BACKSLASH,
            RightBracket = SDLK_RIGHTBRACKET,
            GraveAccent = SDLK_BACKQUOTE,
            Escape = SDLK_ESCAPE,
            Enter = SDLK_RETURN,
            Tab = SDLK_TAB,
            Backspace = SDLK_BACKSPACE,
            Insert = SDLK_INSERT,
            Delete = SDLK_DELETE,
            ArrowRight = SDLK_RIGHT,
            ArrowLeft = SDLK_LEFT,
            ArrowDown = SDLK_DOWN,
            ArrowUp = SDLK_UP,
            PageUp = SDLK_PAGEUP,
            PageDown = SDLK_PAGEDOWN,
            Home = SDLK_HOME,
            End = SDLK_END,
            CapsLock = SDLK_CAPSLOCK,
            ScrollLock = SDLK_SCROLLLOCK,
            NumLock = SDLK_NUMLOCKCLEAR,
            PrintScreen = SDLK_PRINTSCREEN,
            Pause = SDLK_PAUSE,
            F1 = SDLK_F1,
            F2 = SDLK_F2,
            F3 = SDLK_F3,
            F4 = SDLK_F4,
            F5 = SDLK_F5,
            F6 = SDLK_F6,
            F7 = SDLK_F7,
            F8 = SDLK_F8,
            F9 = SDLK_F9,
            F10 = SDLK_F10,
            F11 = SDLK_F11,
            F12 = SDLK_F12,
            F13 = SDLK_F13,
            F14 = SDLK_F14,
            F15 = SDLK_F15,
            F16 = SDLK_F16,
            F17 = SDLK_F17,
            F18 = SDLK_F18,
            F19 = SDLK_F19,
            F20 = SDLK_F20,
            F21 = SDLK_F21,
            F22 = SDLK_F22,
            F23 = SDLK_F23,
            F24 = SDLK_F24,
            Numpad0 = SDLK_KP_0,
            Numpad1 = SDLK_KP_1,
            Numpad2 = SDLK_KP_2,
            Numpad3 = SDLK_KP_3,
            Numpad4 = SDLK_KP_4,
            Numpad5 = SDLK_KP_5,
            Numpad6 = SDLK_KP_6,
            Numpad7 = SDLK_KP_7,
            Numpad8 = SDLK_KP_8,
            Numpad9 = SDLK_KP_9,
            NumpadDecimal = SDLK_KP_PERIOD,
            NumpadDivide = SDLK_KP_DIVIDE,
            NumpadMultiply = SDLK_KP_MULTIPLY,
            NumpadSubtract = SDLK_KP_MINUS,
            NumpadAdd = SDLK_KP_PLUS,
            NumpadEnter = SDLK_KP_ENTER,
            NumpadEqual = SDLK_KP_EQUALS,
            LeftShift = SDLK_LSHIFT,
            LeftControl = SDLK_LCTRL,
            LeftAlt = SDLK_LALT,
            LeftSuper = SDLK_LGUI,
            RightShift = SDLK_RSHIFT,
            RightControl = SDLK_RCTRL,
            RightAlt = SDLK_RALT,
            RightSuper = SDLK_RGUI,
            Menu = SDLK_MENU
        };
        
        /// <summary>
        /// An enum listing all supported mouse buttons.
        /// This uses the same numbering as in GLFW input, so a GLFW-based implementation can use it directly without any
        /// further mapping.
        /// </summary>
        enum class MouseButton
        {
            Left = 0,
            Right = 1,
            Middle = 2
        };

        /// Checks and returns whether a given mouse button is being held down in the current frame.
        bool GetMouseButton(MouseButton button) { return mButtonsDown[button]; }

        /// Gets the screen position of the mouse in pixel coordinates, relative to the top-left corner of the screen.
        glm::vec2 GetMousePosition() const { return mMousePosition; }

        /// Gets the mouse wheel, relative to the initial value when starting the game.
        glm::vec2 GetMouseWheel() const { return mMouseWheel; }; 

        /// Checks and returns whether a given keyboard key is being held down in the current frame.
        bool GetKeyboardKey(const KeyboardKey button) { return mKeysDown[button]; }

    private:
        friend class Engine;
        friend class Context;
        void Update();

        std::unordered_map<MouseButton, bool> mButtonsDown;
        std::unordered_map<KeyboardKey, bool> mKeysDown;
        glm::vec2 mMousePosition;
        glm::vec2 mMouseWheel;
    };

}  // namespace FS