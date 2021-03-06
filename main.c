#include "main.h"

bool EXIT = false;

int ACTIVE_SYMBOL = 0;
int ACTIVE_COLOR = TB_WHITE;
int ACTIVE_BACK_COLOR = BACKGROUND_COLOR;


int main(int argv, char **argc)
{
	int code = tb_init();

	/* Check if termbox was initialized. */
	if(code < 0) {
		fprintf(stderr, "termbox init failed, code: %d\n", code);
		return -1;
	}

	/* Needed for nanosleep. */
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 16 * 100000L; /* 16 mili seconds ~= 60FPS */

	tb_select_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
	tb_select_output_mode(TB_OUTPUT_NORMAL);
	tb_clear();

	int HOTBAR_SIZE[2];
	HOTBAR_SIZE[0] = tb_width();
	HOTBAR_SIZE[1] = HOTBAR_HEIGHT;

	Pixel hotbar[HOTBAR_SIZE[0] * HOTBAR_SIZE[1]];

	/* Initialize a hotbar with only spaces. */
	for(int i = 0; i < HOTBAR_SIZE[0] * HOTBAR_SIZE[1]; i++) {
		hotbar[i].symbol = ' ';
		hotbar[i].color = TB_GREEN;
		hotbar[i].backColor = BACKGROUND_COLOR;
	}

	/* Initialize a border between canvas and hotbar. */
	for(int y = HOTBAR_SIZE[1] - 1, x = 0; x < HOTBAR_SIZE[0]; x++) {
		hotbar[y * HOTBAR_SIZE[0] + x].symbol = '-';
		hotbar[y * HOTBAR_SIZE[0] + x].color = TB_GREEN;
		hotbar[y * HOTBAR_SIZE[0] + x].backColor = BACKGROUND_COLOR;
	}

	int x = 2;
	/* Show possible drawing symbols in the hotbar. */
	for(int y = 2, i = 0; x < HOTBAR_SIZE[0] - 6 && i < 10; x += 6, i++) {
		hotbar[y * HOTBAR_SIZE[0] + x].symbol = '0' + i;
		hotbar[y * HOTBAR_SIZE[0] + x].color = TB_GREEN;
		hotbar[y * HOTBAR_SIZE[0] + x].backColor = BACKGROUND_COLOR;
		hotbar[y * HOTBAR_SIZE[0] + x + 1].symbol = ':';
		hotbar[y * HOTBAR_SIZE[0] + x + 1].color = TB_GREEN;
		hotbar[y * HOTBAR_SIZE[0] + x + 1].backColor = TB_BLACK;
		hotbar[y * HOTBAR_SIZE[0] + x + 3].symbol = POSSIBLE_SYMBOLS[i];
		hotbar[y * HOTBAR_SIZE[0] + x + 3].color = TB_GREEN;
		hotbar[y * HOTBAR_SIZE[0] + x + 3].backColor = BACKGROUND_COLOR;
	}
	x += 4;

	hotbar[2 * HOTBAR_SIZE[0] + x].symbol = 'E';
	hotbar[2 * HOTBAR_SIZE[0] + x].color = TB_GREEN;
	hotbar[2 * HOTBAR_SIZE[0] + x].backColor = BACKGROUND_COLOR;
	hotbar[2 * HOTBAR_SIZE[0] + x + 1].symbol = ':';
	hotbar[2 * HOTBAR_SIZE[0] + x + 1].color = TB_GREEN;
	hotbar[2 * HOTBAR_SIZE[0] + x + 1].backColor = BACKGROUND_COLOR;
	hotbar[2 * HOTBAR_SIZE[0] + x + 3].symbol = '-';
	hotbar[2 * HOTBAR_SIZE[0] + x + 3].color = TB_GREEN;
	hotbar[2 * HOTBAR_SIZE[0] + x + 3].backColor = BACKGROUND_COLOR;

	x += 7;

	/* Show possible colors in the hotbar. */
	for(int y = 2, i = 0; x < HOTBAR_SIZE[0] - 6 && i <= TB_WHITE - TB_BLACK; x += 6, i++) {
		hotbar[y * HOTBAR_SIZE[0] + x].symbol = 'a' + i;
		hotbar[y * HOTBAR_SIZE[0] + x].color = TB_GREEN;
		hotbar[y * HOTBAR_SIZE[0] + x].backColor = BACKGROUND_COLOR;
		hotbar[y * HOTBAR_SIZE[0] + x + 1].symbol = ':';
		hotbar[y * HOTBAR_SIZE[0] + x + 1].color = TB_GREEN;
		hotbar[y * HOTBAR_SIZE[0] + x + 1].backColor = BACKGROUND_COLOR;
		hotbar[y * HOTBAR_SIZE[0] + x + 3].symbol = ' ';
		hotbar[y * HOTBAR_SIZE[0] + x + 3].color = TB_BLACK + i;
		hotbar[y * HOTBAR_SIZE[0] + x + 3].backColor = TB_BLACK + i;
	}

	int CANVAS_SIZE[2];
	CANVAS_SIZE[0] = tb_width();
	CANVAS_SIZE[1] = tb_height();

	Pixel canvas[CANVAS_SIZE[0] * CANVAS_SIZE[1]];

	/* Initialize a canvas with only spaces. */
	for(int i = 0; i < CANVAS_SIZE[0] * CANVAS_SIZE[1]; i++) {
		canvas[i].symbol = ' ';
		canvas[i].color = TB_GREEN;
		canvas[i].backColor = BACKGROUND_COLOR;
	}

	/* Quit loop if exit is true. */
	while(!EXIT){
		tb_clear();

		/* Handle input. */
		int *point = input(CANVAS_SIZE);

		/* Check point validity. */
		if(point[0] != -1) {
			canvas[point[1] * CANVAS_SIZE[0] + point[0]].symbol = POSSIBLE_SYMBOLS[ACTIVE_SYMBOL];
			canvas[point[1] * CANVAS_SIZE[0] + point[0]].color = ACTIVE_COLOR;
			canvas[point[1] * CANVAS_SIZE[0] + point[0]].backColor = ACTIVE_BACK_COLOR;
		}

		/* Draw. */
		drawBackground();

		paint(canvas, CANVAS_SIZE);
		paint(hotbar, HOTBAR_SIZE);

		/* Draw to screen. */
		tb_present();

		/* Sleep. */
		nanosleep(&tim, &tim2);
	}

	tb_shutdown();

	save(canvas, CANVAS_SIZE);

	return 0;
}

void save(Pixel *canvas, int *CANVAS_SIZE)
{
	for(int y = 0; y < CANVAS_SIZE[1]; y++) {
		for(int x = 0; x < CANVAS_SIZE[0]; x++) {
			printf("%c", canvas[y * CANVAS_SIZE[0] + x].symbol);
		}

		printf("\n");
	}
}

int point[2];
int *input(int *CANVAS_SIZE)
{
	point[0] = -1; point[1] = -1;

	struct tb_event ev;

	/* Update input with a timeout of n ms. */
	tb_peek_event(&ev, 0);

	/* Mouse click. */
	if(ev.type == TB_EVENT_MOUSE) {
		if(ev.key == TB_KEY_MOUSE_LEFT) {
			if(ev.x <= CANVAS_SIZE[0] && ev.y >= tb_height() - CANVAS_SIZE[1]) {
				point[0] = ev.x;
				point[1] = ev.y;
			}
		}
	}

	if(ev.type == TB_EVENT_KEY) {
		if(ev.key == TB_KEY_ESC) {
			EXIT = true;
		} else {
			char new_symbol = ev.ch;
			if(new_symbol >= '0' && new_symbol < '0' + POSSIBLE_SYMBOLS_SIZE) {
				ACTIVE_SYMBOL = new_symbol - '0';
			}

			if(new_symbol >= 'a' && new_symbol < 'a' + TB_WHITE) {
				ACTIVE_COLOR = TB_BLACK + new_symbol - 'a';
			}

			if(new_symbol == '-') {
				ACTIVE_SYMBOL = ' ';
			}
		}
	}

	return point;
}

void paint(Pixel *canvas, int *CANVAS_SIZE)
{
	for(int y = 0; y < CANVAS_SIZE[1]; y++)
		for(int x = 0; x < CANVAS_SIZE[0]; x++)
			tb_change_cell(x, y, canvas[y * CANVAS_SIZE[0] + x].symbol, canvas[y * CANVAS_SIZE[0] + x].color, canvas[y * CANVAS_SIZE[0] + x].backColor);
}

void drawBackground()
{
	/* Draw the background. */
	for(int x = 0; x < tb_width(); x++){
		for(int y = 0; y < tb_height(); y++){
			tb_change_cell(x, y, ' ', BACKGROUND_COLOR,
			BACKGROUND_COLOR);
		}
	}
}
