#include "./conway.h"

#define DL_IMPL
#include "./dl/dl-xlib.h"

int	main(int ac, char **av) {
	struct s_conway	conway;

	if (ac > 1) {
		if (!strcmp(av[1], "-v") || !strcmp(av[1], "--version")) {
			printf("[ INFO ] GAME: Version: 1.0\n");
		}
		else if (!strcmp(av[1], "-h") || !strcmp(av[1], "--help")) {
			printf("[ INFO ] GAME: Help message:\n");
			printf("\n1. KEYBOARD:\n");
			printf("- SPACE (press)  - Update the simulation (once);\n");
			printf("- SPACE (hold)   - Update the simulation (conitnuous);\n");
			printf("- C (press)      - Clear the simulation grid and data;\n");
			printf("\n2. MOUSE:\n");
			printf("- LEFT (press)   - Toggle cell state (alive/dead)\n");
			printf("\n3. FLAGS:\n");
			printf("- -h / --help    - prints this help message and quits the program\n");
			printf("- -v / --version - prints the game's version and quits the program\n");
		}
		return (0);
	}

	dl_loadXlib();
	gameInit(960, 640, "Conway's Game of Life");

	conway = (struct s_conway) { 0 };
	gameConwayInit(&conway, 16);
	while (!gameShouldQuit()) {
		/* SECTION: Update
		 * */
	
		if (gameButtonPressed(Button1)) {
			gameConwayTogglePixel(
				&conway,
				(gameMotionX() + (conway.x * -1)) / conway.s,
				(gameMotionY() + (conway.y * -1)) / conway.s
			);
		}
	
		if (gameButtonDown(Button3)) {
			conway.x += gameMotionDeltaX();
			conway.y += gameMotionDeltaY();
		}

		
		
		if (gameKeyPressed('c')) { gameConwayClear(&conway); }

		if (gameKeyPressed(' ')) {
			gameConwayProceed(&conway);
		}
		if (gameKeyDown(' ')) {
			if (conway.time_current > 0.0) {
				conway.time_current -= gameDeltaTime();
			}
			else {
				conway.time_current = conway.time_tick;
				gameConwayProceed(&conway);
			}
		}
		if (gameKeyReleased(' ')) {
			conway.time_current = conway.time_default;
		}

		/* SECTION: Render
		 * */
	
		gameClearColor(0xff0f0f0f);

		gameConwayRender(&conway);
		gameSwapBuffers();

		/* SECTION: Event polling
		 * */
	
		gamePollEvents();
	}

	gameConwayTerminate(&conway);
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
		printf("[ INFO ] GAME: DISPLAY: Created successfully | w.:%d | h.:%d\n", w, h);
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
		
		printf("[ INFO ] GAME: WINDOW: Created successfully | w.:%d | h.:%d\n", w, h);
	}
	
	/* SECTION:
	 *  Client Creation
	 * */
	{
		g_game.time.t_curr = g_game.time.t_prev = gameTime();
	}
	return (true);
}

bool	gameShouldQuit(void) {
	return (g_game.exit);
}

bool	gamePollEvents(void) {
	register uint32_t	i;
	register uint32_t	s;
	XEvent				_event;

	for (i = 0, s = sizeof(g_game.input.b_curr); i < s; i++) { g_game.input.b_prev[i] = g_game.input.b_curr[i]; }
	for (i = 0, s = sizeof(g_game.input.k_curr); i < s; i++) { g_game.input.k_prev[i] = g_game.input.k_curr[i]; }
	g_game.input.m_prev[0] = g_game.input.m_curr[0];
	g_game.input.m_prev[1] = g_game.input.m_curr[1];

	while (XPending(g_game.cli.dsp)) {
		XNextEvent(g_game.cli.dsp, &_event);
		switch (_event.type) {
			case (ClientMessage): {
				if (_event.xclient.data.l[0] == (int64_t) g_game.cli.wm_delete_window) {
					g_game.exit = true;
				}
			} break;

			case (ButtonPress):
			case (ButtonRelease): {
				if (_event.xbutton.button >= sizeof(g_game.input.b_curr)) { break; }
				g_game.input.b_curr[_event.xbutton.button] = (_event.xbutton.type == ButtonPress ? true : false);
			} break;

			case (MotionNotify): {
				g_game.input.m_curr[0] = _event.xmotion.x;
				g_game.input.m_curr[1] = _event.xmotion.y;
			} break;

			case (KeyPress):
			case (KeyRelease): {
				KeySym	_keysym;
				
				_keysym = XkbKeycodeToKeysym(g_game.cli.dsp, _event.xkey.keycode, 0, _event.xkey.state & ShiftMask ? 1 : 0);
				if (_keysym >= sizeof(g_game.input.k_curr)) { break; }
				g_game.input.k_curr[_keysym] = (_event.xkey.type == KeyPress ? true : false);
			} break;
		}
	}

	/* Update game timing
	 * */
	g_game.time.t_prev = g_game.time.t_curr;
	g_game.time.t_curr = gameTime();
	g_game.time.t_delta = (g_game.time.t_curr - g_game.time.t_prev) / 1000.0;
	return (true);
}

bool	gameSwapBuffers(void) {
	XPutImage(g_game.cli.dsp, g_game.cli.w_id, g_game.cli.gc, g_game.cli.img, 0, 0, 0, 0, g_game.dsp.width, g_game.dsp.height);
	return (true);
}

bool	gameQuit(void) {
	XDestroyImage(g_game.cli.img);
	XFreeGC(g_game.cli.dsp, g_game.cli.gc);
	XUnmapWindow(g_game.cli.dsp, g_game.cli.w_id);
	XDestroyWindow(g_game.cli.dsp, g_game.cli.w_id);
	XCloseDisplay(g_game.cli.dsp);
	printf("[ INFO ] GAME: Terminated successfully\n");
	return (true);
}

/* SECTION:
 *  Display
 * */

bool	gameClearColor(const uint32_t pix) {
	for (register size_t i = 0, siz = g_game.dsp.width * g_game.dsp.height; i < siz; i++) {
		g_game.dsp.data[i] = pix;
	}
	return (true);
}

bool	gameDrawRect(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const uint32_t p) {
	for (register uint32_t y0 = y, y1 = y + h; y0 < g_game.dsp.height && y0 < y1; y0++) {
		for (register uint32_t x0 = x, x1 = x + w; x0 < g_game.dsp.width && x0 < x1; x0++) {
			g_game.dsp.data[y0 * g_game.dsp.width + x0] = p;
		}
	}
	return (true);
}

bool	gameDrawRectLines(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const uint32_t p) {
	for (register uint32_t y0 = y, y1 = y + h; y0 < g_game.dsp.height && y0 < y1; y0++) {
		for (register uint32_t x0 = x, x1 = x + w; x0 < g_game.dsp.width && x0 < x1; x0++) {
			if ((y0 == 0 || y0 == y1 - 1) ||
				(x0 == 0 || x0 == x1 - 1)
			) {
				g_game.dsp.data[y0 * g_game.dsp.width + x0] = p;
			}
		}
	}
	return (true);
}

/* SECTION:
 *  Time
 * */

double	gameDeltaTime(void) {
	return (g_game.time.t_delta);
}

double	gameTime(void) {
	struct timeval	_timeval;

	if (gettimeofday(&_timeval, 0) < 0) {
		return (0.0);
	}
	return (_timeval.tv_sec * 1000.0 + _timeval.tv_usec / 1000.0);
}

/* SECTION:
 *  Input
 * */

bool	gameKeyPressed(const uint32_t index) {
	return (g_game.input.k_curr[index] && !g_game.input.k_prev[index]);
}

bool	gameKeyReleased(const uint32_t index) {
	return (!g_game.input.k_curr[index] && g_game.input.k_prev[index]);
}

bool	gameKeyDown(const uint32_t index) {
	return (g_game.input.k_curr[index]);
}

bool	gameKeyUp(const uint32_t index) {
	return (!g_game.input.k_curr[index]);
}

bool	gameButtonPressed(const uint32_t index) {
	return (g_game.input.b_curr[index] && !g_game.input.b_prev[index]);
}

bool	gameButtonReleased(const uint32_t index) {
	return (!g_game.input.b_curr[index] && g_game.input.b_prev[index]);
}

bool	gameButtonDown(const uint32_t index) {
	return (g_game.input.b_curr[index]);
}

bool	gameButtonUp(const uint32_t index) {
	return (!g_game.input.b_curr[index]);
}

bool	gameMotion(uint32_t *xptr, uint32_t *yptr) {
	if (xptr) { *xptr = g_game.input.m_curr[0]; }
	if (yptr) { *yptr = g_game.input.m_curr[1]; }
	return (true);
}

uint32_t	gameMotionX(void) {
	return (g_game.input.m_curr[0]);
}

uint32_t	gameMotionY(void) {
	return (g_game.input.m_curr[1]);
}

bool	gameMotionDelta(uint32_t *xptr, uint32_t *yptr) {
	if (xptr) { *xptr = g_game.input.m_curr[0] - g_game.input.m_prev[0]; }
	if (yptr) { *yptr = g_game.input.m_curr[1] - g_game.input.m_prev[1]; }
	return (true);
}

uint32_t	gameMotionDeltaX(void) {
	return (g_game.input.m_curr[0] - g_game.input.m_prev[0]);
}

uint32_t	gameMotionDeltaY(void) {
	return (g_game.input.m_curr[1] - g_game.input.m_prev[1]);
}

/* SECTION:
 *  Conway
 * */

bool	gameConwayInit(struct s_conway *conway, const uint32_t s) {
	if (!conway) { return (false); }


	conway->w = 256;
	conway->h = 256;
	conway->s = s;
	conway->data0 = (uint32_t *) calloc(conway->w * conway->h, sizeof(uint32_t));
	if (!conway->data0) {
		return (false);
	}
	/* NOTE(yakub):
	 *  This is a little double-buffering trick which works like that:
	 *  - we iterate over every cell in data0;
	 *  - if we can apply the rules for cells in data0, we copy it to data1;
	 *  - at the end of the iteration we copy all the data1 cells into data0;
	 * */
	conway->data1 = (uint32_t *) calloc(conway->w * conway->h, sizeof(uint32_t));
	if (!conway->data1) {
		return (false);
	}

	/* Set the position to the center of the grid
	 * */
	conway->x = 0;
	conway->y = 0;

	/* Setup timinig variables of the simulation
	 * */
	conway->time_default = 0.2;
	conway->time_tick = 0.1;
	conway->time_current = conway->time_default;

	printf("[ INFO ] CONWAY: Created successfully | w.:%d | h.:%d | s.:%d\n", conway->w, conway->h, conway->s);
	return (true);
}

bool	gameConwayTogglePixel(struct s_conway *conway, const uint32_t x, const uint32_t y) {
	if (!conway) { return (false); }

	if (x >= conway->w) { return (false); }
	if (y >= conway->h) { return (false); }
	conway->data0[y * conway->w + x] = !conway->data0[y * conway->w + x];
	printf("[ INFO ] CONWAY: Pixel toggled | x.:%d | y.:%d\n", x, y);
	return (true);
}

bool	gameConwayGetState(struct s_conway *conway, const uint32_t x, const uint32_t y) {
	if (!conway) { return (false); }
	
	if (x >= conway->w) { return (false); }
	if (y >= conway->h) { return (false); }
	return (conway->data0[y * conway->w + x]);
}

bool	gameConwayGetNeighboursCount(struct s_conway *conway, const uint32_t x, const uint32_t y, uint32_t *ptr) {
	if (!conway) { return (false); }
	else if (!ptr) { return (false); }

	if (gameConwayGetState(conway, x - 1, y - 1)) { (*ptr)++; }
	if (gameConwayGetState(conway, x - 1, y)) { (*ptr)++; }
	if (gameConwayGetState(conway, x - 1, y + 1)) { (*ptr)++; }
	if (gameConwayGetState(conway, x, y + 1)) { (*ptr)++; }
	if (gameConwayGetState(conway, x + 1, y + 1)) { (*ptr)++; }
	if (gameConwayGetState(conway, x + 1, y)) { (*ptr)++; }
	if (gameConwayGetState(conway, x + 1, y - 1)) { (*ptr)++; }
	if (gameConwayGetState(conway, x, y - 1)) { (*ptr)++; }

	return (true);
}

bool	gameConwayClear(struct s_conway *conway) {
	if (!conway) { return (false); }

	for (register uint32_t i = 0, s = conway->w * conway->h; i < s; i++) {
		conway->data0[i] = false;
	}
	conway->generation = conway->population = 0;
	printf("[ INFO ] CONWAY: Grid cleared\n");
	return (true);
}

bool	gameConwayProceed(struct s_conway *conway) {
	if (!conway) { return (false); }

	conway->generation++;
	conway->population = 0;
	for (register uint32_t i = 0, s = conway->w * conway->h; i < s; i++) { conway->data1[i] = false; }

	for (register uint32_t y0 = 0, y1 = conway->h; y0 < y1; y0++) {
		for (register uint32_t x0 = 0, x1 = conway->w; x0 < x1; x0++) {
			uint32_t	_neighbours;

			_neighbours = 0;
			if (!gameConwayGetNeighboursCount(conway, x0, y0, &_neighbours)) {
				continue;
			}

			/* CHECK: Dead cell
			 * - If neightbours == 3: set alive (reproduction);
			 * */
			if (!gameConwayGetState(conway, x0, y0)) {
				if (_neighbours == 3) {
					conway->data1[y0 * x1 + x0] = true;
				}
			}
			/* CHECK: Alive cell
			 * - If neighbours < 2: set dead (underpopulation);
			 * - If neighbours == (2, 3): set alive;
			 * - If neighbours > 3: set dead (overpopulation);
			 * */
			else {
				switch (_neighbours) {
					case (0):
					case (1): {
						conway->data1[y0 * x1 + x0] = false; /* underpopulation */
					} break;

					case (2):
					case (3): {
						conway->data1[y0 * x1 + x0] = true; /* remain alive */
					} break;

					default: {
						conway->data1[y0 * x1 + x0] = false; /* overpopulation */
					} break;
				}
			}
		}
	}
	for (register uint32_t i = 0, s = conway->w * conway->h; i < s; i++) {
		if (conway->data1[i]) {
			conway->population++;
		}
		conway->data0[i] = conway->data1[i];
	}

	printf("[ INFO ] CONWAY: Generation: %d\n", conway->generation);
	printf("[ INFO ] CONWAY: Population: %d\n", conway->population);
	return (true);
}

bool	gameConwayRender(struct s_conway *conway) {
	if (!conway) { return (false); }

	for (uint32_t y0 = 0; y0 < conway->h; y0++) {
		for (uint32_t x0 = 0; x0 < conway->w; x0++) {
			if (conway->data0[y0 * conway->w + x0]) {
				gameDrawRect(
					(x0 * conway->s) + conway->x,
					(y0 * conway->s) + conway->y,
					conway->s, conway->s,
					0xffffffff
				);
			}
			gameDrawRectLines(
				(x0 * conway->s) + conway->x,
				(y0 * conway->s) + conway->y,
				conway->s, conway->s,
				0xff000000
			);
		}
	}
	return (true);
}

bool	gameConwayTerminate(struct s_conway *conway) {
	if (!conway) { return (false); }

	free(conway->data0), conway->data0 = 0;
	free(conway->data1), conway->data1 = 0;
	conway->s = conway->w = conway->h = 0;
	printf("[ INFO ] CONWAY: Terminated successfully\n");
	return (true);
}

/* SECTION:
 *  Global objects
 * */
struct s_game	g_game = { 0 };
