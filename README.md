# Conway's Game of Life

## Intro

This is a software-reasterized simulation of Conway's Game of Life for GNU/Linux, written in C.

## Building

```console
$ git clone --recurse-submodules https://github.com/itsYakub/Conway-s-Game-of-Life.git conways-game-of-life/
$ cd conways-game-of-life/
$ gcc -Wall -Wextra -Werror conway.c -o conway
$ ./conway
```

To get the help message for the game, use:

```console
$ ./conway --help
```

To print the version of the game, use:

```console
$ ./conway --version
```

## Controls

For the controls list you can use command:

```console
$ ./conway --help
```

Here's a list of controls:

```
- LEFT MOUSE BUTTON (PRESS)  - Toggle cell state (alive/dead);
- RIGHT MOUSE BUTTON (PRESS) - Camera panning;
- MOUSE SCROLL               - Camera zooming in/out;
- SPACE (PRESS)              - Update the simulation (once);
- SPACE (HOLD)               - Update the simulation (continuous);
- C (PRESS)                  - Clears the simulation;
```
