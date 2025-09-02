#if !defined (_conway_h_)
# define _conway_h_
# include <stdio.h>
# include <stdint.h>
# include <stddef.h>
# include <stdlib.h>
# include <stdbool.h>
# include <X11/Xlib.h>
# include <sys/time.h>

/* SECTION:
 *  Display
 * */

struct s_display {
	uint32_t	*data;
	uint32_t	width;
	uint32_t	height;
};

bool	gameClearColor(const uint32_t);
bool	gameDrawRect(const uint32_t, const uint32_t, const uint32_t, const uint32_t, const uint32_t);
bool	gameDrawRectLines(const uint32_t, const uint32_t, const uint32_t, const uint32_t, const uint32_t);

struct s_client {
	Display		*dsp;
	XImage		*img;
	GC			gc;

	Atom		wm_delete_window;

	Window		w_id;
	Window		r_id;
};

/* SECTION:
 *  Time
 * */

struct s_time {
	double		t_curr;
	double		t_prev;
	double		t_delta;
};

double	gameDeltaTime(void);
double	gameTime(void);

/* SECTION:
 *  Input
 * */

struct s_input {
	int32_t	m_curr[2];		/* motion (current) */
	bool	k_curr[1024];	/* key (current) */
	bool	k_prev[1024];	/* key (previous) */
	bool	b_curr[8];		/* button (current) */
	bool	b_prev[8];		/* button (previous) */
};

bool		gameKeyPressed(const uint32_t);
bool		gameKeyReleased(const uint32_t);
bool		gameKeyDown(const uint32_t);
bool		gameKeyUp(const uint32_t);
bool		gameButtonPressed(const uint32_t);
bool		gameButtonReleased(const uint32_t);
bool		gameButtonDown(const uint32_t);
bool		gameButtonUp(const uint32_t);
bool		gameMotion(uint32_t *, uint32_t *);
uint32_t	gameMotionX(void);
uint32_t	gameMotionY(void);

/* SECTION:
 *  Game
 * */

struct s_game {
	struct s_client		cli;	/* client */
	struct s_display	dsp;	/* display */
	struct s_time		time;
	struct s_input		input;
	bool				exit;
};

bool	gameInit(const uint32_t, const uint32_t, const char *);
bool	gameShouldQuit(void);
bool	gamePollEvents(void);
bool	gameSwapBuffers(void);
bool	gameQuit(void);

extern struct s_game	g_game;

/* SECTION:
 *  Conway
 * */

struct s_conway {
	uint32_t	*data0;
	uint32_t	*data1;
	
	uint32_t	width;
	uint32_t	height;
	uint32_t	cell_size;

	uint32_t	generation;
	uint32_t	population;

	double		time_default;
	double		time_tick;
	double		time_current;
};

bool	gameConwayInit(struct s_conway *, const uint32_t);
bool	gameConwayTogglePixel(struct s_conway *, const uint32_t, const uint32_t);
bool	gameConwayGetState(struct s_conway *, const uint32_t, const uint32_t);
bool	gameConwayGetNeighboursCount(struct s_conway *, const uint32_t, const uint32_t, uint32_t *);
bool	gameConwayClear(struct s_conway *);
bool	gameConwayProceed(struct s_conway *);
bool	gameConwayRender(struct s_conway *);
bool	gameConwayRenderGrid(struct s_conway *);
bool	gameConwayTerminate(struct s_conway *);

#endif /* _conway_h_ */
