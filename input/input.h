/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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
enum de_key
{
	DE_KEY_UKNOWN = -1,
	DE_KEY_A = 0,    /**< A key */
	DE_KEY_B,        /**< B key */
	DE_KEY_C,        /**< C key */
	DE_KEY_D,        /**< D key */
	DE_KEY_E,        /**< E key */
	DE_KEY_F,        /**< F key */
	DE_KEY_G,        /**< G key */
	DE_KEY_H,        /**< H key */
	DE_KEY_I,        /**< I key */
	DE_KEY_J,        /**< J key */
	DE_KEY_K,        /**< K key */
	DE_KEY_L,        /**< L key */
	DE_KEY_M,        /**< N key */
	DE_KEY_N,        /**< A key */
	DE_KEY_O,        /**< O key */
	DE_KEY_P,        /**< P key */
	DE_KEY_Q,        /**< Q key */
	DE_KEY_R,        /**< R key */
	DE_KEY_S,        /**< S key */
	DE_KEY_T,        /**< T key */
	DE_KEY_U,        /**< U key */
	DE_KEY_V,        /**< V key */
	DE_KEY_W,        /**< W key */
	DE_KEY_X,        /**< X key */
	DE_KEY_Y,        /**< Y key */
	DE_KEY_Z,        /**< Z key */
	DE_KEY_NUM0,     /**< Numpad 0 */
	DE_KEY_NUM1,     /**< Numpad 1 */
	DE_KEY_NUM2,     /**< Numpad 2 */
	DE_KEY_NUM3,     /**< Numpad 3 */
	DE_KEY_NUM4,     /**< Numpad 4 */
	DE_KEY_NUM5,     /**< Numpad 5 */
	DE_KEY_NUM6,     /**< Numpad 6 */
	DE_KEY_NUM7,     /**< Numpad 7 */
	DE_KEY_NUM8,     /**< Numpad 8 */
	DE_KEY_NUM9,     /**< Numpad 9 */
	DE_KEY_ESC,      /**< Escape */
	DE_KEY_LCONTROL, /**< Left control */
	DE_KEY_LSHIFT,   /**< Left shift */
	DE_KEY_LAlt,     /**< Left alt */
	DE_KEY_LSystem,  /**< Left system key */
	DE_KEY_RControl, /**< Right control */
	DE_KEY_RShift,   /**< Right shift */
	DE_KEY_RAlt,     /**< Right alt */
	DE_KEY_RSystem,  /**< Right system key */
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
	DE_KEY_BackSpace,
	DE_KEY_Tab,
	DE_KEY_PageUp,
	DE_KEY_PageDown,
	DE_KEY_End,
	DE_KEY_Home,
	DE_KEY_Insert,
	DE_KEY_Delete,
	DE_KEY_Add,
	DE_KEY_Subtract,
	DE_KEY_Multiply,
	DE_KEY_Divide,
	DE_KEY_Left,
	DE_KEY_Right,
	DE_KEY_Up,
	DE_KEY_Down,
	DE_KEY_Numpad0, /**< Numpad 0 key */
	DE_KEY_Numpad1, /**< Numpad 1 key */
	DE_KEY_Numpad2, /**< Numpad 2 key */
	DE_KEY_Numpad3, /**< Numpad 3 key */
	DE_KEY_Numpad4, /**< Numpad 4 key */
	DE_KEY_Numpad5, /**< Numpad 5 key */
	DE_KEY_Numpad6, /**< Numpad 6 key */
	DE_KEY_Numpad7, /**< Numpad 7 key */
	DE_KEY_Numpad8, /**< Numpad 8 key */
	DE_KEY_Numpad9, /**< Numpad 9 key */
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
	DE_KEY_Count    /**< Total key count */
};

/**
* @brief Possible mouse buttons codes
*/
enum de_mouse_button
{
	DE_BUTTON_LEFT,   /**< Left mouse button */
	DE_BUTTON_RIGHT,  /**< Right mouse button */
	DE_BUTTON_MIDDLE, /**< Middle (wheel) mouse button */
	DE_BUTTON_COUNT   /**< Total mouse button count */
};

/**
* @brief Returns DE_TRUE if specified key is pressed
*/
int de_is_key_pressed(de_core_t* core, enum de_key key);

/**
* @brief Writes out mouse velocity
* @param vel pointer to vector
*/
void de_get_mouse_velocity(de_core_t* core, de_vec2_t* vel);

/**
* @brief Returns DE_TRUE if specified key is released
*/
int de_is_key_released(de_core_t* core, enum de_key key);

int de_is_mouse_pressed(de_core_t* core, enum de_mouse_button btn);

void de_get_mouse_pos(de_core_t* core, de_vec2_t* pos);