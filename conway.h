#if !defined (_conway_h_)
# define _conway_h_
# include <stdio.h>
# include <stdint.h>
# include <stddef.h>
# include <stdlib.h>
# include <stdbool.h>
# include <X11/Xlib.h>

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

struct s_game {
	struct s_client		cli;	/* client */
	struct s_display	dsp;	/* display */
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

#endif /* _conway_h_ */
