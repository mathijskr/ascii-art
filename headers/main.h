#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "termbox.h"

#include "pixel.h"

#define BACKGROUND_COLOR TB_DEFAULT

#define HOTBAR_HEIGHT 4

#define POSSIBLE_SYMBOLS_SIZE 10

void save(Pixel *canvas, int *CANVAS_SIZE);

int *input();

void paint(Pixel *canvas, int *CANVAS_SIZE);

/* Draw a space character with a color that is specified by BACKGROUND_COLOR
 * at all possible screen coordinates. */
void drawBackground();

char POSSIBLE_SYMBOLS[POSSIBLE_SYMBOLS_SIZE] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };

#endif
