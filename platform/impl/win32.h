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

static int de_remap_key(WPARAM key, LPARAM flags)
{
	switch (key)
	{
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
	case VK_NUMPAD0:  return DE_KEY_Numpad0;
	case VK_NUMPAD1:  return DE_KEY_Numpad1;
	case VK_NUMPAD2:  return DE_KEY_Numpad2;
	case VK_NUMPAD3:  return DE_KEY_Numpad3;
	case VK_NUMPAD4:  return DE_KEY_Numpad4;
	case VK_NUMPAD5:  return DE_KEY_Numpad5;
	case VK_NUMPAD6:  return DE_KEY_Numpad6;
	case VK_NUMPAD7:  return DE_KEY_Numpad7;
	case VK_NUMPAD8:  return DE_KEY_Numpad8;
	case VK_NUMPAD9:  return DE_KEY_Numpad9;
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

static LRESULT CALLBACK de_win32_dummy_winproc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(wnd, msg, wParam, lParam);
}

static LRESULT CALLBACK de_window_proc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	de_core_t* core = (de_core_t*)GetWindowLongPtr(wnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		core->running = DE_FALSE;
		break;
	case WM_KEYDOWN:
		core->keys[de_remap_key(wParam, lParam)] = DE_TRUE;
		break;
	case WM_KEYUP:
		core->keys[de_remap_key(wParam, lParam)] = DE_FALSE;
		break;
	case WM_MOUSEMOVE:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		core->mouse_vel.x = x - core->mouse_pos.x;
		core->mouse_vel.y = y - core->mouse_pos.y;

		core->mouse_pos.x = (float)x;
		core->mouse_pos.y = (float)y;
		break;
	}
	case WM_LBUTTONDOWN:
		core->mouse_buttons[DE_BUTTON_LEFT] = DE_TRUE;
		break;
	case WM_LBUTTONUP:
		core->mouse_buttons[DE_BUTTON_LEFT] = DE_FALSE;
		break;
	case WM_RBUTTONDOWN:
		core->mouse_buttons[DE_BUTTON_RIGHT] = DE_TRUE;
		break;
	case WM_RBUTTONUP:
		core->mouse_buttons[DE_BUTTON_RIGHT] = DE_FALSE;
		break;
	case WM_MBUTTONDOWN:
		core->mouse_buttons[DE_BUTTON_MIDDLE] = DE_TRUE;
		break;
	case WM_MBUTTONUP:
		core->mouse_buttons[DE_BUTTON_MIDDLE] = DE_FALSE;
		break;
	case WM_MOUSEWHEEL:
		core->mouse_wheel = ((short)HIWORD(wParam)) / WHEEL_DELTA;
		break;
	case WM_NCMOUSEMOVE:
	case WM_MOUSELEAVE:
		memset(&core->mouse_buttons, 0, sizeof(core->mouse_buttons));
		core->mouse_vel.x = 0;
		core->mouse_vel.y = 0;
		break;
	case WM_KILLFOCUS:
		core->keyboard_focus = DE_FALSE;
		break;
	case WM_SETFOCUS:
		core->keyboard_focus = DE_TRUE;
		break;
	}
	return DefWindowProc(wnd, msg, wParam, lParam);
}

static void de_win32_load_wgl_extensions(de_core_t* core)
{
	int pixel_format;
	WNDCLASSEX wcx = { 0 };
	PIXELFORMATDESCRIPTOR pfd = { 0 };

	wcx.cbSize = sizeof(wcx);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hInstance = GetModuleHandle(0);
	wcx.lpfnWndProc = de_win32_dummy_winproc;
	wcx.lpszClassName = TEXT("Dummy");
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wcx);

	core->platform.dummy_window = CreateWindow(TEXT("Dummy"), TEXT("Dummy"), WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, 0, 0, wcx.hInstance, 0);

	if (!core->platform.dummy_window)
	{
		de_fatal_error("Win32: Failed to create dummy window!");
	}

	core->platform.dummy_dc = GetDC(core->platform.dummy_window);

	/* Prepare pixel format */
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	pixel_format = ChoosePixelFormat(core->platform.dummy_dc, &pfd);
	SetPixelFormat(core->platform.dummy_dc, pixel_format, &pfd);

	core->platform.dummy_context = wglCreateContext(core->platform.dummy_dc);
	if (!core->platform.dummy_context)
	{
		de_fatal_error("Win32: Failed to create dummy OpenGL context!");
	}

	if (!wglMakeCurrent(core->platform.dummy_dc, core->platform.dummy_context))
	{
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
	wglDeleteContext(core->platform.dummy_context);
	ReleaseDC(core->platform.dummy_window, core->platform.dummy_dc);
	DestroyWindow(core->platform.dummy_window);
}

void de_engine_platform_init(de_core_t* core)
{
	DWORD style;
	int pixel_format;
	unsigned int format_count;
	RECT wRect = { 0 };
	WNDCLASSEX wcx = { 0 };
	PIXELFORMATDESCRIPTOR pfd = { 0 };
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

	de_win32_load_wgl_extensions(core);

	/* Zero means auto detection of current screen mode */
	if (core->params.width == 0)
	{
		core->params.width = GetSystemMetrics(SM_CXSCREEN);
	}
	if (core->params.height == 0)
	{
		core->params.height = GetSystemMetrics(SM_CYSCREEN);
	}

	wcx.cbSize = sizeof(wcx);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hInstance = GetModuleHandle(0);
	wcx.lpfnWndProc = de_window_proc;
	wcx.lpszClassName = TEXT("DEngine");
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wcx);

	style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	wRect.right = core->params.width;
	wRect.bottom = core->params.height;
	AdjustWindowRect(&wRect, style, 0);

	/* Create window */
	de_log("Win32: Creating window");
	core->platform.window = CreateWindow(TEXT("DEngine"), TEXT("DEngine"), style, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, wcx.hInstance, 0);
	SetWindowLongPtr(core->platform.window, GWLP_USERDATA, (LONG)core);
	core->platform.device_context = GetDC(core->platform.window);

	if (!core->platform.window)
	{
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
	if (!core->platform.gl_context)
	{
		de_fatal_error("Win32: Failed to create OpenGL context!");
	}
	if (!wglMakeCurrent(core->platform.device_context, core->platform.gl_context))
	{
		de_fatal_error("Win32: Failed to make OpenGL context current!");
	}
}

void de_engine_platform_shutdown(de_core_t* core)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(core->platform.gl_context);
}

void de_engine_platform_message_queue(de_core_t* core)
{
	MSG message;

	core->mouse_vel.x = 0;
	core->mouse_vel.y = 0;

	while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

de_proc de_engine_platform_get_proc_address(const char* name)
{
	return (de_proc)wglGetProcAddress(name);
}

void de_engine_platform_swap_buffers(de_core_t* core)
{
	SwapBuffers(core->platform.device_context);
}

float de_time_get_seconds()
{
	LARGE_INTEGER frequency, time;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&time);
	return (float)time.QuadPart / frequency.QuadPart;
}

void de_engine_platform_message_box(const char* msg)
{
	MessageBoxA(NULL, msg, "Error", MB_OK | MB_ICONERROR);
}

void de_sleep(int milliseconds)
{
	if (milliseconds >= 0)
	{
		Sleep(milliseconds);
	}
}