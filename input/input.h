/* Copyright (c) 2017-2019 Dmitry Stepanov a.k.a mr.DIMAS
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

/**
* @brief Keyboard possible keys
*/
enum de_key {
	DE_KEY_UKNOWN = -1,
	DE_KEY_A = 0,
	DE_KEY_B,
	DE_KEY_C,
	DE_KEY_D,
	DE_KEY_E,
	DE_KEY_F,
	DE_KEY_G,
	DE_KEY_H,
	DE_KEY_I,
	DE_KEY_J,
	DE_KEY_K,
	DE_KEY_L,
	DE_KEY_M,
	DE_KEY_N,
	DE_KEY_O,
	DE_KEY_P,
	DE_KEY_Q,
	DE_KEY_R,
	DE_KEY_S,
	DE_KEY_T,
	DE_KEY_U,
	DE_KEY_V,
	DE_KEY_W,
	DE_KEY_X,
	DE_KEY_Y,
	DE_KEY_Z,
	DE_KEY_NUM0,
	DE_KEY_NUM1,
	DE_KEY_NUM2,
	DE_KEY_NUM3,
	DE_KEY_NUM4,
	DE_KEY_NUM5,
	DE_KEY_NUM6,
	DE_KEY_NUM7,
	DE_KEY_NUM8,
	DE_KEY_NUM9,
	DE_KEY_ESC,
	DE_KEY_LCONTROL,
	DE_KEY_LSHIFT,
	DE_KEY_LAlt,
	DE_KEY_LSystem,
	DE_KEY_RControl,
	DE_KEY_RShift,
	DE_KEY_RAlt,
	DE_KEY_RSystem,
	DE_KEY_Menu,
	DE_KEY_LBracket,
	DE_KEY_RBracket,
	DE_KEY_SemiColon,
	DE_KEY_Comma,
	DE_KEY_Period,
	DE_KEY_Quote,
	DE_KEY_Slash,
	DE_KEY_BackSlash,
	DE_KEY_Tilde,
	DE_KEY_Equal,
	DE_KEY_Dash,
	DE_KEY_Space,
	DE_KEY_Return,
	DE_KEY_BACKSPACE,
	DE_KEY_Tab,
	DE_KEY_PageUp,
	DE_KEY_PageDown,
	DE_KEY_End,
	DE_KEY_Home,
	DE_KEY_Insert,
	DE_KEY_DELETE,
	DE_KEY_Add,
	DE_KEY_Subtract,
	DE_KEY_Multiply,
	DE_KEY_Divide,
	DE_KEY_LEFT,
	DE_KEY_RIGHT,
	DE_KEY_UP,
	DE_KEY_DOWN,
	DE_KEY_NUMPAD0, /**< Numpad 0 key */
	DE_KEY_NUMPAD1, /**< Numpad 1 key */
	DE_KEY_NUMPAD2, /**< Numpad 2 key */
	DE_KEY_NUMPAD3, /**< Numpad 3 key */
	DE_KEY_NUMPAD4, /**< Numpad 4 key */
	DE_KEY_NUMPAD5, /**< Numpad 5 key */
	DE_KEY_NUMPAD6, /**< Numpad 6 key */
	DE_KEY_NUMPAD7, /**< Numpad 7 key */
	DE_KEY_NUMPAD8, /**< Numpad 8 key */
	DE_KEY_NUMPAD9, /**< Numpad 9 key */
	DE_KEY_F1,      /**< F1 key */
	DE_KEY_F2,      /**< F2 key */
	DE_KEY_F3,      /**< F3 key */
	DE_KEY_F4,      /**< F4 key */
	DE_KEY_F5,      /**< F5 key */
	DE_KEY_F6,      /**< F6 key */
	DE_KEY_F7,      /**< F7 key */
	DE_KEY_F8,      /**< F8 key */
	DE_KEY_F9,      /**< F9 key */
	DE_KEY_F10,     /**< F10 key */
	DE_KEY_F11,     /**< F11 key */
	DE_KEY_F12,     /**< F12 key */
	DE_KEY_F13,     /**< F13 key */
	DE_KEY_F14,     /**< F14 key */
	DE_KEY_F15,     /**< F15 key */
	DE_KEY_Pause,   /**< Pause\break key */
};

/**
* @brief Possible mouse buttons codes
*/
enum de_mouse_button {
	DE_BUTTON_LEFT,   /**< Left mouse button */
	DE_BUTTON_RIGHT,  /**< Right mouse button */
	DE_BUTTON_MIDDLE, /**< Middle (wheel) mouse button */
};
