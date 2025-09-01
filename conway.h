#if !defined (_conway_h_)
# define _conway_h_
# include <stdio.h>
# include <stdint.h>
# include <stddef.h>
# include <stdlib.h>
# include <stdbool.h>
# include <X11/Xlib.h>
# include <sys/time.h>

struct s_display {
	uint32_t	*data;
	uint32_t	width;
	uint32_t	height;
};

struct s_client {
	Display		*dsp;
	XImage		*img;
	GC			gc;

	Atom		wm_delete_window;

	Window		w_id;
	Window		r_id;
};

struct s_time {
	double		t_curr;
	double		t_prev;
	double		t_delta;
};

struct s_input {
	int32_t	motion[2];
	bool	key[65536];
	bool	mouse[8];
};

struct s_game {
	struct s_client		cli;	/* client */
	struct s_display	dsp;	/* display */
	struct s_time		time;
	struct s_input		input;
	bool				exit;
};

/* Global 'game' object
 * */
extern struct s_game	g_game;

/* SECTION:
 *  Game
 * */
bool	gameInit(const uint32_t, const uint32_t, const char *);
bool	gameShouldQuit(void);
bool	gamePollEvents(void);
bool	gameSwapBuffers(void);
bool	gameQuit(void);

/* SECTION:
 *  Graphics
 * */
bool	gameClearColor(const uint32_t);

/* SECTION:
 *  Time
 * */
double	gameDeltaTime(void);
double	gameTime(void);

/* SECTION:
 *  Input
 * */
bool		gameKeyPressed(const uint32_t);
bool		gameButtonPress(const uint32_t);
bool		gameMotion(uint32_t *, uint32_t *);
uint32_t	gameMotionX(void);
uint32_t	gameMotionY(void);

#endif /* _conway_h_ */
