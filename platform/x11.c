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

static int de_remap_key(KeySym symbol)
{
	switch (symbol) {
		case XK_Shift_L:      return DE_KEY_LSHIFT;
		case XK_Shift_R:      return DE_KEY_RShift;
		case XK_Control_L:    return DE_KEY_LCONTROL;
		case XK_Control_R:    return DE_KEY_RControl;
		case XK_Alt_L:        return DE_KEY_LAlt;
		case XK_Alt_R:        return DE_KEY_RAlt;
		case XK_Super_L:      return DE_KEY_LSystem;
		case XK_Super_R:      return DE_KEY_RSystem;
		case XK_Menu:         return DE_KEY_Menu;
		case XK_Escape:       return DE_KEY_ESC;
		case XK_semicolon:    return DE_KEY_SemiColon;
		case XK_slash:        return DE_KEY_Slash;
		case XK_equal:        return DE_KEY_Equal;
		case XK_minus:        return DE_KEY_Dash;
		case XK_bracketleft:  return DE_KEY_LBracket;
		case XK_bracketright: return DE_KEY_RBracket;
		case XK_comma:        return DE_KEY_Comma;
		case XK_period:       return DE_KEY_Period;
		case XK_apostrophe:   return DE_KEY_Quote;
		case XK_backslash:    return DE_KEY_BackSlash;
		case XK_grave:        return DE_KEY_Tilde;
		case XK_space:        return DE_KEY_Space;
		case XK_Return:       return DE_KEY_Return;
		case XK_KP_Enter:     return DE_KEY_Return;
		case XK_BackSpace:    return DE_KEY_BACKSPACE;
		case XK_Tab:          return DE_KEY_Tab;
		case XK_Prior:        return DE_KEY_PageUp;
		case XK_Next:         return DE_KEY_PageDown;
		case XK_End:          return DE_KEY_End;
		case XK_Home:         return DE_KEY_Home;
		case XK_Insert:       return DE_KEY_Insert;
		case XK_Delete:       return DE_KEY_DELETE;
		case XK_KP_Add:       return DE_KEY_Add;
		case XK_KP_Subtract:  return DE_KEY_Subtract;
		case XK_KP_Multiply:  return DE_KEY_Multiply;
		case XK_KP_Divide:    return DE_KEY_Divide;
		case XK_Pause:        return DE_KEY_Pause;
		case XK_F1:           return DE_KEY_F1;
		case XK_F2:           return DE_KEY_F2;
		case XK_F3:           return DE_KEY_F3;
		case XK_F4:           return DE_KEY_F4;
		case XK_F5:           return DE_KEY_F5;
		case XK_F6:           return DE_KEY_F6;
		case XK_F7:           return DE_KEY_F7;
		case XK_F8:           return DE_KEY_F8;
		case XK_F9:           return DE_KEY_F9;
		case XK_F10:          return DE_KEY_F10;
		case XK_F11:          return DE_KEY_F11;
		case XK_F12:          return DE_KEY_F12;
		case XK_F13:          return DE_KEY_F13;
		case XK_F14:          return DE_KEY_F14;
		case XK_F15:          return DE_KEY_F15;
		case XK_Left:         return DE_KEY_LEFT;
		case XK_Right:        return DE_KEY_RIGHT;
		case XK_Up:           return DE_KEY_UP;
		case XK_Down:         return DE_KEY_DOWN;
		case XK_KP_Insert:    return DE_KEY_NUMPAD0;
		case XK_KP_End:       return DE_KEY_NUMPAD1;
		case XK_KP_Down:      return DE_KEY_NUMPAD2;
		case XK_KP_Page_Down: return DE_KEY_NUMPAD3;
		case XK_KP_Left:      return DE_KEY_NUMPAD4;
		case XK_KP_Begin:     return DE_KEY_NUMPAD5;
		case XK_KP_Right:     return DE_KEY_NUMPAD6;
		case XK_KP_Home:      return DE_KEY_NUMPAD7;
		case XK_KP_Up:        return DE_KEY_NUMPAD8;
		case XK_KP_Page_Up:   return DE_KEY_NUMPAD9;
		case XK_a:            return DE_KEY_A;
		case XK_b:            return DE_KEY_B;
		case XK_c:            return DE_KEY_C;
		case XK_d:            return DE_KEY_D;
		case XK_e:            return DE_KEY_E;
		case XK_f:            return DE_KEY_F;
		case XK_g:            return DE_KEY_G;
		case XK_h:            return DE_KEY_H;
		case XK_i:            return DE_KEY_I;
		case XK_j:            return DE_KEY_J;
		case XK_k:            return DE_KEY_K;
		case XK_l:            return DE_KEY_L;
		case XK_m:            return DE_KEY_M;
		case XK_n:            return DE_KEY_N;
		case XK_o:            return DE_KEY_O;
		case XK_p:            return DE_KEY_P;
		case XK_q:            return DE_KEY_Q;
		case XK_r:            return DE_KEY_R;
		case XK_s:            return DE_KEY_S;
		case XK_t:            return DE_KEY_T;
		case XK_u:            return DE_KEY_U;
		case XK_v:            return DE_KEY_V;
		case XK_w:            return DE_KEY_W;
		case XK_x:            return DE_KEY_X;
		case XK_y:            return DE_KEY_Y;
		case XK_z:            return DE_KEY_Z;
		case XK_0:            return DE_KEY_NUM0;
		case XK_1:            return DE_KEY_NUM1;
		case XK_2:            return DE_KEY_NUM2;
		case XK_3:            return DE_KEY_NUM3;
		case XK_4:            return DE_KEY_NUM4;
		case XK_5:            return DE_KEY_NUM5;
		case XK_6:            return DE_KEY_NUM6;
		case XK_7:            return DE_KEY_NUM7;
		case XK_8:            return DE_KEY_NUM8;
		case XK_9:            return DE_KEY_NUM9;
	}

	return DE_KEY_UKNOWN;
}

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static bool isExtensionSupported(const char *extList, const char *extension)
{
	const char *start;
	const char *where, *terminator;

	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return false;

	/* It takes a bit of care to be fool-proof about parsing the
	   OpenGL extensions string. Don't be fooled by sub-strings,
	   etc. */
	for (start = extList;;) {
		where = strstr(start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;

		start = terminator;
	}

	return false;
}

static bool ctxErrorOccurred = false;
static int ctxErrorHandler(Display *dpy, XErrorEvent *ev)
{
	ctxErrorOccurred = true;
	return 0;
}

void de_core_platform_init(de_core_t* core)
{
	int glx_major, glx_minor;
	XSetWindowAttributes swa;
	Colormap cmap;
	int fbcount;
	Display *display;
	GLXFBConfig* fbc;
	int i;
	Window rootWindow;
	GLXFBConfig bestFbc;
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
	static int visual_attribs[] =
	{
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		None
	};

	display = XOpenDisplay(NULL);
	if (!display) {
		de_fatal_error("Failed to open X display\n");
	}

	/* FBConfigs were added in GLX version 1.3. */
	if (!glXQueryVersion(display, &glx_major, &glx_minor) || ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)) {
		de_fatal_error("Invalid GLX version");
	}

	fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
	if (!fbc) {
		de_fatal_error("Failed to retrieve a framebuffer config");
	}

	/* Pick the FB config/visual with the most samples per pixel */
	for (i = 0; i < fbcount; ++i) {
		XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
		if (vi) {
			int samp_buf, samples;
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

			if (best_fbc < 0 || (samp_buf && samples > best_num_samp)) {
				best_fbc = i;
				best_num_samp = samples;
			}
			if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp) {
				worst_fbc = i;
				worst_num_samp = samples;
			}
		}
		XFree(vi);
	}

	bestFbc = fbc[best_fbc];

	/* Be sure to free the FBConfig list allocated by glXChooseFBConfig() */
	XFree(fbc);

	/* Get a visual */
	XVisualInfo *vi = glXGetVisualFromFBConfig(display, bestFbc);

	rootWindow = RootWindow(display, vi->screen);

	swa.colormap = cmap = XCreateColormap(display, rootWindow, vi->visual, AllocNone);
	swa.background_pixmap = None;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | PointerMotionMask;

	Window win = XCreateWindow(display, rootWindow,
		0, 0, core->params.video_mode.width, core->params.video_mode.height, 0, vi->depth, InputOutput,
		vi->visual,
		CWBorderPixel | CWColormap | CWEventMask, &swa);
	if (!win) {
		de_fatal_error("Failed to create window.");
	}

	XFree(vi);

	XStoreName(display, win, "Dmitry's Engine");
	XMapWindow(display, win);

	/* Get the default screen's GLX extension list */
	const char *glxExts = glXQueryExtensionsString(display, DefaultScreen(display));

	/*
	 * NOTE: It is not necessary to create or make current to a context before
	 * calling glXGetProcAddressARB
	 */
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

	GLXContext ctx = 0;

	/*
	 * Install an X error handler so the application won't exit if GL 3.0
	 * context allocation fails.
	 *
	 * Note this error handler is global.  All display connections in all threads
	 * of a process use the same error handler, so be sure to guard against other
	 * threads issuing X commands while this code is running.
	 */
	ctxErrorOccurred = false;
	int(*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

	if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
		de_fatal_error("GLX_ARB_create_context is not supported!");
	} else {
		int context_attribs[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 3,
			GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			None
		};

		de_log("Creating context...");

		ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);

		/* Sync to ensure any errors generated are processed. */
		XSync(display, False);

		if (ctxErrorOccurred || !ctx) {
			de_fatal_error("Unable to create OpenGL 3.3 Core Context! It is required minimum!");
		}

		de_log("Core Context Created!");
	}

	/* Sync to ensure any errors generated are processed. */
	XSync(display, False);

	/* Restore the original error handler */
	XSetErrorHandler(oldHandler);

	if (ctxErrorOccurred || !ctx) {
		de_fatal_error("Failed to create an OpenGL context");
	}

	/* Verifying that context is a direct context */
	if (!glXIsDirect(display, ctx)) {
		de_log("Indirect GLX rendering context obtained");
	} else {
		de_log("Direct GLX rendering context obtained");
	}

	de_log("Making context current\n");
	glXMakeCurrent(display, win, ctx);

	core->platform.display = display;
	core->platform.glContext = ctx;
	core->platform.window = win;
}

void de_core_platform_shutdown(de_core_t* core)
{
	glXMakeCurrent(core->platform.display, 0, 0);
	glXDestroyContext(core->platform.display, core->platform.glContext);

	XDestroyWindow(core->platform.display, core->platform.window);
	XCloseDisplay(core->platform.display);
}

void de_core_platform_poll_events(de_core_t* core)
{
	int i, key;
	static int lastx, lasty;
	XEvent event;
	Display* dpy = core->platform.display;
	de_event_t evt;
	while (XEventsQueued(dpy, QueuedAfterFlush) != 0) {
		XNextEvent(dpy, &event);
		switch (event.type) {
			case DestroyNotify:
				evt.type = DE_EVENT_TYPE_CLOSE;
				de_core_push_event(core, &evt);
				break;
			case KeyRelease:
				for (i = 0; i < 4; ++i) {
					key = de_remap_key(XLookupKeysym(&event.xkey, i));
					if (key != DE_KEY_UKNOWN) {
						break;
					}
				}
				evt.type = DE_EVENT_TYPE_KEY_UP;
				evt.s.key.key = key;
				evt.s.key.alt = event.xkey.state & Mod1Mask;
				evt.s.key.control = event.xkey.state & ControlMask;
				evt.s.key.shift = event.xkey.state & ShiftMask;
				evt.s.key.system = event.xkey.state & Mod4Mask;
				de_core_push_event(core, &evt);
				break;
			case KeyPress:
				for (i = 0; i < 4; ++i) {
					key = de_remap_key(XLookupKeysym(&event.xkey, i));
					if (key != DE_KEY_UKNOWN) {
						break;
					}
				}
				evt.type = DE_EVENT_TYPE_KEY_DOWN;
				evt.s.key.key = key;
				evt.s.key.alt = event.xkey.state & Mod1Mask;
				evt.s.key.control = event.xkey.state & ControlMask;
				evt.s.key.shift = event.xkey.state & ShiftMask;
				evt.s.key.system = event.xkey.state & Mod4Mask;
				de_core_push_event(core, &evt);

				/* generate TEXT_ENTER here */
				break;
			case ButtonRelease:
				evt.type = DE_EVENT_TYPE_MOUSE_UP;
				evt.s.mouse_up.x = event.xbutton.x;
				evt.s.mouse_up.y = event.xbutton.y;
				switch (event.xbutton.button) {
					case Button1: evt.s.mouse_up.button = 0; break;
					case Button2: evt.s.mouse_up.button = 1; break;
					case Button3: evt.s.mouse_up.button = 2; break;
					case 8: evt.s.mouse_up.button = 3; break;
					case 9: evt.s.mouse_up.button = 4; break;
					default: evt.s.mouse_up.button = -1; break;
				}
				de_core_push_event(core, &evt);
				break;
			case ButtonPress:
				evt.type = DE_EVENT_TYPE_MOUSE_DOWN;
				evt.s.mouse_down.x = event.xbutton.x;
				evt.s.mouse_down.y = event.xbutton.y;
				switch (event.xbutton.button) {
					case Button1: evt.s.mouse_down.button = 0; break;
					case Button2: evt.s.mouse_down.button = 1; break;
					case Button3: evt.s.mouse_down.button = 2; break;
					case 8: evt.s.mouse_down.button = 3; break;
					case 9: evt.s.mouse_down.button = 4; break;
					default: evt.s.mouse_down.button = -1; break;
				}
				de_core_push_event(core, &evt);
				break;
			case MotionNotify:
				evt.type = DE_EVENT_TYPE_MOUSE_MOVE;
				evt.s.mouse_move.x = event.xmotion.x;
				evt.s.mouse_move.y = event.xmotion.y;
				evt.s.mouse_move.vx = evt.s.mouse_move.x - lastx;
				evt.s.mouse_move.vy = evt.s.mouse_move.y - lasty;
				lastx = evt.s.mouse_move.x;
				lasty = evt.s.mouse_move.y;
				de_core_push_event(core, &evt);
				break;
			case ResizeRequest:
				evt.type = DE_EVENT_TYPE_RESIZE;
				evt.s.resize.w = event.xresizerequest.width;
				evt.s.resize.h = event.xresizerequest.height;
				de_core_push_event(core, &evt);
				break;
			default:
				break;
		}
	}
}

de_proc de_get_proc_address(const char *name)
{
	return glXGetProcAddress((const GLubyte*)name);
}

void de_core_platform_swap_buffers(de_core_t* core)
{
	glXSwapBuffers(core->platform.display, core->platform.window);
}

void de_message_box(de_core_t* core, const char * msg, const char* title)
{
/* Stub */
	printf("%s", msg);
}

double de_time_get_seconds(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec + t.tv_nsec / 1000000000.0;
}

void de_sleep(int milliseconds)
{
	//usleep(milliseconds * 1000);
}

char* de_clipboard_get_text()
{
	return (char*)de_calloc(1, 1);
}

void de_clipboard_set_text(char* data)
{
	(void)data;
}

de_video_mode_array_t de_enum_video_modes()
{
	int i, j, k;
	de_video_mode_array_t modes;
	DE_ARRAY_INIT(modes);
	Display* display = XOpenDisplay(NULL);
	if (display) {
		int screen = DefaultScreen(display);
		int version;
		if (XQueryExtension(display, "RANDR", &version, &version, &version)) {
			XRRScreenConfiguration* config = XRRGetScreenInfo(display, RootWindow(display, screen));
			if (config) {
				int num_sizes;
				XRRScreenSize* sizes = XRRConfigSizes(config, &num_sizes);
				if (sizes && num_sizes > 0) {
					int num_depths = 0;
					int* depths = XListDepths(display, screen, &num_depths);
					if (depths && num_depths > 0) {
						for (i = 0; i < num_depths; ++i) {
							for (j = 0; j < num_sizes; ++j) {
								Rotation rotation;
								bool duplicate = false;
								de_video_mode_t vm;

								vm.width = sizes[j].width;
								vm.height = sizes[j].height;
								vm.bits_per_pixel = depths[i];
								vm.fullscreen = true;

								XRRConfigRotations(config, &rotation);
								if (rotation == RR_Rotate_90 || rotation == RR_Rotate_270) {
									int temp = vm.width;
									vm.width = vm.height;
									vm.height = temp;
								}

								for (k = 0; k < modes.size; ++k) {
									de_video_mode_t* existing = modes.data + k;
									if (existing->width == vm.width &&
										existing->height == vm.height &&
										existing->bits_per_pixel == vm.bits_per_pixel) {
										duplicate = true;
										break;
									}
								}

								if (!duplicate) {
									DE_ARRAY_APPEND(modes, vm);
								}
							}
						}
					}
					XFree(depths);
				}
				XRRFreeScreenConfigInfo(config);
			} else {
				de_log("failed to get desktop video mode");
			}
		} else {
			de_log("RANDR is not supported");
		}
		XCloseDisplay(display);
	} else {
		de_log("failed to connect to X11 server");
	}
	return modes;
}

void de_core_platform_set_video_mode(de_core_t* core, const de_video_mode_t* vm)
{
	XRRScreenConfiguration* config;
	int version, i, screen;

	if (!XQueryExtension(core->platform.display, "RANDR", &version, &version, &version)) {
		de_log("unable to set videomode, randr not supported");
		return;
	}

	screen = DefaultScreen(core->platform.display);

	config = XRRGetScreenInfo(core->platform.display, RootWindow(core->platform.display, screen));
	if (config) {
		int num_sizes;
		XRRScreenSize* sizes;
		sizes = XRRConfigSizes(config, &num_sizes);
		if (sizes) {
			for (i = 0; i < num_sizes; ++i) {
				Rotation rotation;
				XRRConfigRotations(config, &rotation);
				if (rotation == RR_Rotate_90 || rotation == RR_Rotate_270) {
					int temp = sizes[i].width;
					sizes[i].width = sizes[i].height;
					sizes[i].height = temp;
				}

				if (sizes[i].width == (int)vm->width && sizes[i].height == (int)vm->height) {
					XRRSetScreenConfig(core->platform.display, config, RootWindow(core->platform.display, screen), i, rotation, CurrentTime);
					break;
				}
			}
		}
		XRRFreeScreenConfigInfo(config);
	} else {
		de_log("unable to set videomode");
	}
}

void de_get_desktop_video_mode(de_video_mode_t* vm)
{
	Display* display = XOpenDisplay(NULL);
	if (display) {
		int screen = DefaultScreen(display);
		int version;
		if (XQueryExtension(display, "RANDR", &version, &version, &version)) {
			XRRScreenConfiguration* config = XRRGetScreenInfo(display, RootWindow(display, screen));
			if (config) {
				int num_sizes;
				Rotation rotation;
				XRRScreenSize* sizes;
				int current_mode = XRRConfigCurrentConfiguration(config, &rotation);

				sizes = XRRConfigSizes(config, &num_sizes);
				if (sizes && num_sizes > 0) {
					vm->width = sizes[current_mode].width;
					vm->height = sizes[current_mode].height;
					vm->bits_per_pixel = DefaultDepth(display, screen);
					vm->fullscreen = true;
				}

				XRRFreeScreenConfigInfo(config);
			} else {
				de_log("failed to get desktop video mode");
			}
		} else {
			de_log("RANDR is not supported");
		}
		XCloseDisplay(display);
	} else {
		de_log("failed to connect to X11 server");
	}
}