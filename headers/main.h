#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include "termbox.h"

#define UPDATE_SPEED 20 /* Must be at least 10 */
#define DRAW_SPEED 16 /* 16 := 60 ~fps */

#define BACKGROUND_COLOR TB_DEFAULT

#define HOTBAR_HEIGHT 4

void save(char *canvas, int *CANVAS_SIZE);

int *input();

void paint(char *canvas, int *CANVAS_SIZE);

/* Draw a space character with a color that is specified by BACKGROUND_COLOR
 * at all possible screen coordinates. */
void drawBackground();

/* Increase the elapsed_time variable every mili second. */
void *timer();

char POSSIBLE_SYMBOLS[10] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')' };

#endif
