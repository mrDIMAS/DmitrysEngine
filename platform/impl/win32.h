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

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

static int de_win32_remap_key(WPARAM key, LPARAM flags) {
	switch (key) {
		case VK_SHIFT:
		{
			UINT lShift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
			UINT scancode = (UINT)((flags & (0xFF << 16)) >> 16);
			return scancode == lShift ? DE_KEY_LSHIFT : DE_KEY_RShift;
		}
		case VK_MENU: return (HIWORD(flags) & KF_EXTENDED) ? DE_KEY_RAlt : DE_KEY_LAlt;
		case VK_CONTROL: return (HIWORD(flags) & KF_EXTENDED) ? DE_KEY_RControl : DE_KEY_LCONTROL;
		case VK_LWIN: return DE_KEY_LSystem;
		case VK_RWIN: return DE_KEY_RSystem;
		case VK_APPS: return DE_KEY_Menu;
		case VK_OEM_1: return DE_KEY_SemiColon;
		case VK_OEM_2: return DE_KEY_Slash;
		case VK_OEM_PLUS: return DE_KEY_Equal;
		case VK_OEM_MINUS: return DE_KEY_Dash;
		case VK_OEM_4: return DE_KEY_LBracket;
		case VK_OEM_6: return DE_KEY_RBracket;
		case VK_OEM_COMMA: return DE_KEY_Comma;
		case VK_OEM_PERIOD: return DE_KEY_Period;
		case VK_OEM_7: return DE_KEY_Quote;
		case VK_OEM_5: return DE_KEY_BackSlash;
		case VK_OEM_3: return DE_KEY_Tilde;
		case VK_ESCAPE: return DE_KEY_ESC;
		case VK_SPACE: return DE_KEY_Space;
		case VK_RETURN: return DE_KEY_Return;
		case VK_BACK: return DE_KEY_BackSpace;
		case VK_TAB: return DE_KEY_Tab;
		case VK_PRIOR: return DE_KEY_PageUp;
		case VK_NEXT: return DE_KEY_PageDown;
		case VK_END: return DE_KEY_End;
		case VK_HOME: return DE_KEY_Home;
		case VK_INSERT: return DE_KEY_Insert;
		case VK_DELETE: return DE_KEY_Delete;
		case VK_ADD: return DE_KEY_Add;
		case VK_SUBTRACT: return DE_KEY_Subtract;
		case VK_MULTIPLY: return DE_KEY_Multiply;
		case VK_DIVIDE:   return DE_KEY_Divide;
		case VK_PAUSE:    return DE_KEY_Pause;
		case VK_F1:       return DE_KEY_F1;
		case VK_F2:       return DE_KEY_F2;
		case VK_F3:       return DE_KEY_F3;
		case VK_F4:       return DE_KEY_F4;
		case VK_F5:       return DE_KEY_F5;
		case VK_F6:       return DE_KEY_F6;
		case VK_F7:       return DE_KEY_F7;
		case VK_F8:       return DE_KEY_F8;
		case VK_F9:       return DE_KEY_F9;
		case VK_F10:      return DE_KEY_F10;
		case VK_F11:      return DE_KEY_F11;
		case VK_F12:      return DE_KEY_F12;
		case VK_F13:      return DE_KEY_F13;
		case VK_F14:      return DE_KEY_F14;
		case VK_F15:      return DE_KEY_F15;
		case VK_LEFT:     return DE_KEY_Left;
		case VK_RIGHT:    return DE_KEY_Right;
		case VK_UP:       return DE_KEY_Up;
		case VK_DOWN:     return DE_KEY_Down;
		case VK_NUMPAD0:  return DE_KEY_NUMPAD0;
		case VK_NUMPAD1:  return DE_KEY_NUMPAD1;
		case VK_NUMPAD2:  return DE_KEY_NUMPAD2;
		case VK_NUMPAD3:  return DE_KEY_NUMPAD3;
		case VK_NUMPAD4:  return DE_KEY_NUMPAD4;
		case VK_NUMPAD5:  return DE_KEY_NUMPAD5;
		case VK_NUMPAD6:  return DE_KEY_NUMPAD6;
		case VK_NUMPAD7:  return DE_KEY_NUMPAD7;
		case VK_NUMPAD8:  return DE_KEY_NUMPAD8;
		case VK_NUMPAD9:  return DE_KEY_NUMPAD9;
		case 'A': return DE_KEY_A;
		case 'Z': return DE_KEY_Z;
		case 'E': return DE_KEY_E;
		case 'R': return DE_KEY_R;
		case 'T': return DE_KEY_T;
		case 'Y': return DE_KEY_Y;
		case 'U': return DE_KEY_U;
		case 'I': return DE_KEY_I;
		case 'O': return DE_KEY_O;
		case 'P': return DE_KEY_P;
		case 'Q': return DE_KEY_Q;
		case 'S': return DE_KEY_S;
		case 'D': return DE_KEY_D;
		case 'F': return DE_KEY_F;
		case 'G': return DE_KEY_G;
		case 'H': return DE_KEY_H;
		case 'J': return DE_KEY_J;
		case 'K': return DE_KEY_K;
		case 'L': return DE_KEY_L;
		case 'M': return DE_KEY_M;
		case 'W': return DE_KEY_W;
		case 'X': return DE_KEY_X;
		case 'C': return DE_KEY_C;
		case 'V': return DE_KEY_V;
		case 'B': return DE_KEY_B;
		case 'N': return DE_KEY_N;
		case '0': return DE_KEY_NUM0;
		case '1': return DE_KEY_NUM1;
		case '2': return DE_KEY_NUM2;
		case '3': return DE_KEY_NUM3;
		case '4': return DE_KEY_NUM4;
		case '5': return DE_KEY_NUM5;
		case '6': return DE_KEY_NUM6;
		case '7': return DE_KEY_NUM7;
		case '8': return DE_KEY_NUM8;
		case '9': return DE_KEY_NUM9;
	}

	return DE_KEY_UKNOWN;
}

static LRESULT CALLBACK de_win32_window_proc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static int lastx, lasty;
	de_core_t* core = (de_core_t*)GetWindowLongPtr(wnd, GWLP_USERDATA);
	de_event_t evt;
	POINT pos;
	if (core) {
		switch (msg) {
			case WM_CLOSE:
			case WM_DESTROY:
				evt.type = DE_EVENT_TYPE_CLOSE;
				de_core_push_event(core, &evt);
				break;
			case WM_KEYDOWN:
				evt.type = DE_EVENT_TYPE_KEY_DOWN;
				evt.s.key_down.key = de_win32_remap_key(wParam, lParam);
				de_core_push_event(core, &evt);
				break;
			case WM_KEYUP:
				evt.type = DE_EVENT_TYPE_KEY_UP;
				evt.s.key_up.key = de_win32_remap_key(wParam, lParam);
				de_core_push_event(core, &evt);
				break;
			case WM_MOUSEMOVE:
				evt.type = DE_EVENT_TYPE_MOUSE_MOVE;
				evt.s.mouse_move.x = LOWORD(lParam);
				evt.s.mouse_move.y = HIWORD(lParam);
				evt.s.mouse_move.vx = evt.s.mouse_move.x - lastx;
				evt.s.mouse_move.vy = evt.s.mouse_move.y - lasty;
				lastx = evt.s.mouse_move.x;
				lasty = evt.s.mouse_move.y;
				de_core_push_event(core, &evt);
				break;
			case WM_LBUTTONDOWN:
				evt.type = DE_EVENT_TYPE_MOUSE_DOWN;
				evt.s.mouse_down.button = DE_BUTTON_LEFT;
				evt.s.mouse_down.x = LOWORD(lParam);
				evt.s.mouse_down.y = HIWORD(lParam);
				de_core_push_event(core, &evt);
				break;
			case WM_LBUTTONUP:
				evt.type = DE_EVENT_TYPE_MOUSE_UP;
				evt.s.mouse_up.button = DE_BUTTON_LEFT;
				evt.s.mouse_up.x = LOWORD(lParam);
				evt.s.mouse_up.y = HIWORD(lParam);
				de_core_push_event(core, &evt);
				break;
			case WM_RBUTTONDOWN:
				evt.type = DE_EVENT_TYPE_MOUSE_DOWN;
				evt.s.mouse_down.button = DE_BUTTON_RIGHT;
				evt.s.mouse_down.x = LOWORD(lParam);
				evt.s.mouse_down.y = HIWORD(lParam);
				de_core_push_event(core, &evt);
				break;
			case WM_RBUTTONUP:
				evt.type = DE_EVENT_TYPE_MOUSE_UP;
				evt.s.mouse_up.button = DE_BUTTON_RIGHT;
				evt.s.mouse_up.x = LOWORD(lParam);
				evt.s.mouse_up.y = HIWORD(lParam);
				de_core_push_event(core, &evt);
				break;
			case WM_MBUTTONDOWN:
				evt.type = DE_EVENT_TYPE_MOUSE_DOWN;
				evt.s.mouse_down.button = DE_BUTTON_MIDDLE;
				evt.s.mouse_down.x = LOWORD(lParam);
				evt.s.mouse_down.y = HIWORD(lParam);
				de_core_push_event(core, &evt);
				break;
			case WM_MBUTTONUP:
				evt.type = DE_EVENT_TYPE_MOUSE_UP;
				evt.s.mouse_up.button = DE_BUTTON_MIDDLE;
				evt.s.mouse_up.x = LOWORD(lParam);
				evt.s.mouse_up.y = HIWORD(lParam);
				de_core_push_event(core, &evt);
				break;
			case WM_MOUSEWHEEL:
				evt.type = DE_EVENT_TYPE_MOUSE_WHEEL;
				evt.s.mouse_wheel.delta = ((short)HIWORD(wParam)) / WHEEL_DELTA;
				pos.x = LOWORD(lParam);
				pos.y = HIWORD(lParam);
				ScreenToClient(core->platform.window, &pos);
				evt.s.mouse_wheel.x = pos.x;
				evt.s.mouse_wheel.y = pos.y;
				de_core_push_event(core, &evt);
				break;
			case WM_MOUSELEAVE:
				evt.type = DE_EVENT_TYPE_MOUSE_LEAVE;
				de_core_push_event(core, &evt);
				break;
			case WM_KILLFOCUS:
				evt.type = DE_EVENT_TYPE_LOST_FOCUS;
				de_core_push_event(core, &evt);
				break;
			case WM_SETFOCUS:
				evt.type = DE_EVENT_TYPE_GOT_FOCUS;
				de_core_push_event(core, &evt);
				break;
		}
	}
	return DefWindowProc(wnd, msg, wParam, lParam);
}

static void de_win32_load_wgl_extensions(void) {
	int pixel_format;
	WNDCLASSEX wcx;
	PIXELFORMATDESCRIPTOR pfd;
	HWND window;
	HGLRC glcontext;
	HDC dc;

	de_zero(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hInstance = GetModuleHandle(0);
	wcx.lpfnWndProc = DefWindowProc;
	wcx.lpszClassName = TEXT("Dummy");
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wcx);

	window = CreateWindow(TEXT("Dummy"), TEXT("Dummy"), WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, 0, 0, wcx.hInstance, 0);

	if (!window) {
		de_fatal_error("Win32: Failed to create dummy window!");
	}

	dc = GetDC(window);

	/* Prepare pixel format */
	de_zero(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	pixel_format = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pixel_format, &pfd);

	glcontext = wglCreateContext(dc);
	if (!glcontext) {
		de_fatal_error("Win32: Failed to create dummy OpenGL context!");
	}

	if (!wglMakeCurrent(dc, glcontext)) {
		de_fatal_error("Win32: Failed to make dummy OpenGL context current!");
	}

#define DE_GET_WGL_PROC_ADDRESS(type, func)                    \
	func = (type)wglGetProcAddress(#func);                     \
	if(!func) {                                                \
		de_fatal_error("Unable to load %s function pointer", #func); \
	} else {                                                   \
		de_log("Function %s is loaded!", #func);               \
	}

	/* Obtain function pointers */
	DE_GET_WGL_PROC_ADDRESS(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);
	DE_GET_WGL_PROC_ADDRESS(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);

#undef DE_GET_WGL_PROC_ADDRESS

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(glcontext);
	ReleaseDC(window, dc);
	DestroyWindow(window);
}

void de_core_platform_init(de_core_t* core) {
	DWORD style;
	int pixel_format;
	unsigned int format_count;
	RECT wRect;
	WNDCLASSEX wcx;
	PIXELFORMATDESCRIPTOR pfd;
	int pixel_fmt_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0
	};
	int attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	de_win32_load_wgl_extensions();

	de_zero(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hInstance = GetModuleHandle(0);
	wcx.lpfnWndProc = de_win32_window_proc;
	wcx.lpszClassName = TEXT("DEngine");
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wcx);

	if (core->params.flags & DE_CORE_FLAGS_BORDERLESS) {
		style = WS_VISIBLE | WS_POPUP;
	} else {
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	}

	wRect.left = 0;
	wRect.top = 0;
	wRect.right = core->params.video_mode.width;
	wRect.bottom = core->params.video_mode.height;
	AdjustWindowRect(&wRect, style, 0);

	/* Create window */
	de_log("Win32: Creating window");
	core->platform.window = CreateWindow(TEXT("DEngine"), TEXT("DEngine"), style, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, wcx.hInstance, 0);
	SetWindowLongPtr(core->platform.window, GWLP_USERDATA, (LONG)core);
	core->platform.device_context = GetDC(core->platform.window);

	if (!core->platform.window) {
		de_fatal_error("Failed to create window");
	}
	core->platform.device_context = GetDC(core->platform.window);
	ShowWindow(core->platform.window, SW_SHOW);
	UpdateWindow(core->platform.window);
	SetActiveWindow(core->platform.window);
	SetForegroundWindow(core->platform.window);

	/* Initialize OpenGL */
	de_log("Win32: Initializing OpenGL");

	/* Prepare pixel format */
	de_zero(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	wglChoosePixelFormatARB(core->platform.device_context, pixel_fmt_attribs, NULL, 1, &pixel_format, &format_count);

	SetPixelFormat(core->platform.device_context, pixel_format, &pfd);

	core->platform.gl_context = wglCreateContextAttribsARB(core->platform.device_context, NULL, attributes);
	if (!core->platform.gl_context) {
		de_fatal_error("Win32: Failed to create OpenGL context!");
	}
	if (!wglMakeCurrent(core->platform.device_context, core->platform.gl_context)) {
		de_fatal_error("Win32: Failed to make OpenGL context current!");
	}
}

void de_core_platform_shutdown(de_core_t* core) {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(core->platform.gl_context);
}

void de_core_platform_poll_events(de_core_t* core) {
	MSG message;
	DE_UNUSED(core);
	while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

de_proc de_core_platform_get_proc_address(const char* name) {
	return (de_proc)wglGetProcAddress(name);
}

void de_core_platform_swap_buffers(de_core_t* core) {
	SwapBuffers(core->platform.device_context);
}

double de_time_get_seconds() {
	LARGE_INTEGER frequency, time;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&time);
	return time.QuadPart / (double)frequency.QuadPart;
}

void de_core_platform_message_box(de_core_t* core, const char* msg, const char* title) {
	MessageBoxA(core->platform.window, msg, title, MB_OK | MB_ICONERROR);
}

void de_sleep(int milliseconds) {
	if (milliseconds >= 0) {
		Sleep(milliseconds);
	}
}

void de_core_platform_set_window_title(de_core_t* core, const char* title) {
	SetWindowTextA(core->platform.window, title);
}

void de_core_set_video_mode(de_core_t* core, const de_video_mode_t* vm) {
	HWND wnd = core->platform.window;	
	if (vm->fullscreen) {
		DEVMODE mode;
		mode.dmBitsPerPel = vm->bits_per_pixel;
		mode.dmPelsWidth = vm->width;
		mode.dmPelsHeight = vm->height;
		if (ChangeDisplaySettings(&mode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			de_log("unable to set fullscreen videomode: %dx%d%d", vm->width, vm->height, vm->bits_per_pixel);
			return;
		}
	}
	core->params.video_mode = *vm;
	if (vm->fullscreen) {		
		SetWindowLongA(wnd, GWL_STYLE, WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		SetWindowLongA(wnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
	}
	SetWindowPos(wnd, HWND_TOP, 0, 0, vm->width, vm->height, SWP_FRAMECHANGED);
	ShowWindow(wnd, SW_SHOW);
}

void de_get_desktop_video_mode(de_video_mode_t* vm) {
	DEVMODE mode;
	mode.dmSize = sizeof(mode);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);
	vm->fullscreen = false;
	vm->width = mode.dmPelsWidth;
	vm->height = mode.dmPelsHeight;
	vm->bits_per_pixel = mode.dmBitsPerPel;
}

bool de_enum_video_modes(de_video_mode_t* vm, int n) {
	DEVMODE mode;
	mode.dmSize = sizeof(mode);
	if (EnumDisplaySettings(NULL, n, &mode)) {
		vm->fullscreen = true;
		vm->width = mode.dmPelsWidth;
		vm->height = mode.dmPelsHeight;
		vm->bits_per_pixel = mode.dmBitsPerPel;
		return true;
	}
	return false;
}