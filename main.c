#include "main.h"

int GROUND;
int WALL;
bool EXIT = false;

int elapsed_time = 0;

int ACTIVE_SYMBOL = 0;


int main(int argv, char **argc)
{
	int code = tb_init();

	/* Check if termbox was initialized. */
	if(code < 0) {
		fprintf(stderr, "termbox init failed, code: %d\n", code);
		return -1;
	}

	tb_select_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
	tb_select_output_mode(TB_OUTPUT_NORMAL);
	tb_clear();

	pthread_t timer_thread;
	pthread_create(&timer_thread, NULL, timer, NULL);

	int HOTBAR_SIZE[2];
	HOTBAR_SIZE[0] = tb_width();
	HOTBAR_SIZE[1] = HOTBAR_HEIGHT;

	char hotbar[HOTBAR_SIZE[0] * HOTBAR_SIZE[1]];

	/* Initialize a hotbar with only space. **/
	for(int i = 0; i < HOTBAR_SIZE[0] * HOTBAR_SIZE[1]; i++)
		hotbar[i] = ' ';

	/* Initialize a border between canvas and hotbar. */
	for(int y = HOTBAR_SIZE[1] - 1, x = 0; x < HOTBAR_SIZE[0]; x++)
		hotbar[y * HOTBAR_SIZE[0] + x] = '-';

	/* Show possible drawing symbols in the hotbar. */
	for(int y = 2, x = 2, i = 0; x < HOTBAR_SIZE[0] - 6 && i < 10; x +=6) {
		hotbar[y * HOTBAR_SIZE[0] + x] = '0' + i++;
		hotbar[y * HOTBAR_SIZE[0] + x + 1] = ':';
		hotbar[y * HOTBAR_SIZE[0] + x + 3] = POSSIBLE_SYMBOLS[i];
	}

	int CANVAS_SIZE[2];
	CANVAS_SIZE[0] = tb_width();
	CANVAS_SIZE[1] = tb_height();

	char canvas[CANVAS_SIZE[0] * CANVAS_SIZE[1]];

	/* Initialize a canvas with only spaces. */
	for(int i = 0; i < CANVAS_SIZE[0] * CANVAS_SIZE[1]; i++)
		canvas[i] = ' ';

	/* Quit loop if exit is true. */
	while(!EXIT){
		tb_clear();

		/* Update the simulation. */
		if(elapsed_time % UPDATE_SPEED == 0) {
			/* Handle input. */
			int *point = input(CANVAS_SIZE);

			/* Check point validity. */
			if(point[0] != -1) {
				canvas[point[1] * CANVAS_SIZE[0] + point[0]] = POSSIBLE_SYMBOLS[ACTIVE_SYMBOL];
			}
		}

		/* Draw. */
		if(elapsed_time % DRAW_SPEED == 0) {
			drawBackground();

			paint(canvas, CANVAS_SIZE);
			paint(hotbar, HOTBAR_SIZE);

			/* Draw to screen. */
			tb_present();
		}

		/* Increase time to ensure that timed loops won't
		 * be called twice. */
		elapsed_time++;
	}

	tb_shutdown();

	save(canvas, CANVAS_SIZE);

	return 0;
}

void save(char *canvas, int *CANVAS_SIZE)
{
	for(int y = 0; y < CANVAS_SIZE[1]; y++) {
		for(int x = 0; x < CANVAS_SIZE[0]; x++) {
			printf("%c", canvas[y * CANVAS_SIZE[0] + x]);
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
		}
	}

	return point;
}

void paint(char *canvas, int *CANVAS_SIZE)
{
	for(int y = 0; y < CANVAS_SIZE[1]; y++)
		for(int x = 0; x < CANVAS_SIZE[0]; x++)
			tb_change_cell(x, y, canvas[y * CANVAS_SIZE[0] + x], TB_GREEN, BACKGROUND_COLOR);
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

void *timer()
{
	/* Needed for nanosleep. */
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 1000000;

	while(!EXIT){
		nanosleep(&tim, &tim2);
		elapsed_time++;

		if(elapsed_time == 160)
			elapsed_time = 0;
	}

	return &EXIT;
}
