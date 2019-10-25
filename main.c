#include "main.h"

int GROUND;
int WALL;
bool EXIT = false;

int elapsed_time = 0;


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

	char canvas[tb_width() * tb_height()];

	/* Initialize a canvas with only spaces. */
	for(int i = 0; i < tb_width() * tb_height(); i++)
		canvas[i] = ' ';

	int canvasSize[2];
	canvasSize[0] = tb_width();
	canvasSize[1] = tb_height();

	/* Quit loop if exit is true. */
	while(!EXIT){
		tb_clear();

		/* Update the simulation. */
		if(elapsed_time % UPDATE_SPEED == 0) {
			/* Handle input. */
			int *point = input();

			/* Check point validity. */
			if(point[0] != -1) {
				canvas[point[1] * tb_width() + point[0]] = '*';
			}
		}

		/* Draw. */
		if(elapsed_time % DRAW_SPEED == 0) {
			drawBackground();

			drawCanvas(canvas, canvasSize);

			/* Draw to screen. */
			tb_present();
		}

		/* Increase time to ensure that timed loops won't
		 * be called twice. */
		elapsed_time++;
	}

	tb_shutdown();

	save(canvas, canvasSize);

	return 0;
}

void save(char *canvas, int *canvasSize)
{
	for(int y = 0; y < canvasSize[1]; y++) {
		for(int x = 0; x < canvasSize[0]; x++) {
			printf("%c", canvas[y * canvasSize[0] + x]);
		}

		printf("\n");
	}
}

int point[2];
int *input()
{
	point[0] = -1; point[1] = -1;

	struct tb_event ev;

	/* Update input with a timeout of n ms. */
	tb_peek_event(&ev, 0);

	if(ev.key == TB_KEY_ESC)
		EXIT = true;

	/* Mouse click. */
	if(ev.type == TB_EVENT_MOUSE) {
		if(ev.key == TB_KEY_MOUSE_LEFT) {
			point[0] = ev.x;
			point[1] = ev.y;
		}
	}

	return point;
}

void drawCanvas(char *canvas, int *canvasSize)
{
	for(int y = 0; y < canvasSize[1]; y++)
		for(int x = 0; x < canvasSize[0]; x++)
			tb_change_cell(x, y, canvas[y * canvasSize[0] + x], TB_GREEN, BACKGROUND_COLOR);
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
