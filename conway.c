#include "./conway.h"

#define DL_IMPL
#include "./dl/dl-xlib.h"

int	main(void) {
	dl_loadXlib();
	gameInit(800, 600, "Conway's Game of Life");

	while (!gameShouldQuit()) {
		gameClearColor(0xff0f0f0f);

		gameSwapBuffers();
		gamePollEvents();
	}

	gameQuit();
	dl_unloadXlib();
}

/* SECTION:
 *  Game
 * */
bool	gameInit(const uint32_t w, const uint32_t h, const char *t) {
	XWindowAttributes	_attr;

	/* SECTION:
	 *  Display Creations
	 * */
	{
		g_game.dsp.width = w;
		g_game.dsp.height = h;
		g_game.dsp.data = (uint32_t *) calloc(w * h, sizeof(uint32_t));
		if (!g_game.dsp.data) {
			return (false);
		}
	}

	/* SECTION:
	 *  Client Creation
	 * */
	{
		g_game.cli.dsp = XOpenDisplay(0);
		if (!g_game.cli.dsp) {
			return (false);
		}

		g_game.cli.r_id = DefaultRootWindow(g_game.cli.dsp);
		if (!g_game.cli.r_id) {
			return (false);
		}

		g_game.cli.w_id = XCreateSimpleWindow(g_game.cli.dsp, g_game.cli.r_id, 0, 0, w, h, 0, 0, *g_game.dsp.data);
		if (!g_game.cli.w_id) {
			return (false);
		}
		
		XGetWindowAttributes(g_game.cli.dsp, g_game.cli.w_id, &_attr);

		g_game.cli.img = XCreateImage(g_game.cli.dsp, _attr.visual, _attr.depth, ZPixmap, 0, (char *) g_game.dsp.data, w, h, 32, w * sizeof(uint32_t));
		if (!g_game.cli.img) {
			return (false);
		}

		g_game.cli.gc = XCreateGC(g_game.cli.dsp, g_game.cli.w_id, 0, false);
		if (!g_game.cli.gc) {
			return (false);
		}

		g_game.cli.wm_delete_window = XInternAtom(g_game.cli.dsp, "WM_DELETE_WINDOW", 0);
		if (!g_game.cli.wm_delete_window) {
			return (false);
		}

		XSetWMProtocols(g_game.cli.dsp, g_game.cli.w_id, &g_game.cli.wm_delete_window, true);
		XStoreName(g_game.cli.dsp, g_game.cli.w_id, t);
		XSelectInput(g_game.cli.dsp, g_game.cli.w_id, KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask);
		XMapWindow(g_game.cli.dsp, g_game.cli.w_id);
	}
	return (true);
}

bool	gameShouldQuit(void) {
	return (g_game.exit);
}

bool	gamePollEvents(void) {
	XEvent	_event;

	while (XPending(g_game.cli.dsp)) {
		XNextEvent(g_game.cli.dsp, &_event);
		switch (_event.type) {
			case (ClientMessage): {
				if (_event.xclient.data.l[0] == g_game.cli.wm_delete_window) {
					g_game.exit = true;
				}
			} break;

			case (ButtonPress):
			case (ButtonRelease): {

			} break;

			case (MotionNotify): {

			} break;

			case (KeyPress):
			case (KeyRelease): {

			} break;
		}
	}
	return (true);
}

bool	gameSwapBuffers(void) {
	XPutImage(g_game.cli.dsp, g_game.cli.w_id, g_game.cli.gc, g_game.cli.img, 0, 0, 0, 0, g_game.dsp.width, g_game.dsp.height);
	return (true);
}

bool	gameQuit(void) {
	XDestroyImage(g_game.cli.img);
	XUnmapWindow(g_game.cli.dsp, g_game.cli.w_id);
	XDestroyWindow(g_game.cli.dsp, g_game.cli.w_id);
	XCloseDisplay(g_game.cli.dsp);
	return (true);
}

/* SECTION:
 *  Graphics
 * */
bool	gameClearColor(const uint32_t pix) {
	for (register size_t i = 0, siz = g_game.dsp.width * g_game.dsp.height; i < siz; i++) {
		g_game.dsp.data[i] = pix;
	}
	return (true);
}

/* SECTION:
 *  Global objects
 * */
struct s_game	g_game = { 0 };
