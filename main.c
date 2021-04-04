/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define GREY 0xC618
#define BLACK 0x0000
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define GRID_LINE_WIDTH 8
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

volatile int pixel_buffer_start; // global variable
int grid[4][4] = {{0, 0, 0, 0},
				  {0, 0, 0, 0},
				  {0, 0, 0, 0},
				  {0, 0, 0, 0}};

// subroutines
void swap(int *x, int *y);
void wait_for_vsync();
void draw_line(int x0, int y0, int x1, int y1, short int colour);
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void draw_grid();
void draw_box(int minX, int maxX, int minY, int maxY, short int colour);
void black_screen();
void draw_tile(int x, int y, int num);
void draw_all_tiles();

//variables
#define n 8
/*
#define midScreenX RESOLUTION_X / 2
#define midScreenY RESOLUTION_Y / 2
#define gridMinX midScreenX - midScreenY
#define gridMaxX midScreenX + midScreenY
#define gridSideLength gridMaxX - gridMinX
#define boxSideLength (gridSideLength - (5 * GRID_LINE_WIDTH)) / 4
*/

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
	black_screen();
    //black_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	
	black_screen();
	
	int xPos = rand()%4;
	int yPos = rand()%4;
	grid[yPos][xPos] = 2;
	char moves[4] = {'U', 'D', 'L', 'R'};

    while (1)
    {
        /* Erase any boxes and lines that were drawn in the last iteration */
		draw_grid();
		draw_all_tiles();
		move_tiles(moves[rand()%4]);
		
		wait_for_vsync();
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		
    }
}

//shifts all tiles on the grid depending on which direction
void move_tiles(char input){
	for(int row = 0; row < 4; row++){
		for(int col = 0; col < 4; col++){
			//Found tile
			if(grid[row][col] != 0){
				if(input == 'U' && row != 0){
					grid[0][col] = grid[row][col];
					grid[row][col] = 0;
				}
				else if(input == 'D' && row != 3){
					grid[3][col] = grid[row][col];
					grid[row][col] = 0;
				}
				else if(input == 'L' && col != 0){
					grid[row][0] = grid[row][col];
					grid[row][col] = 0;
				}
				else if(input == 'R' && col != 3){
					grid[row][3] = grid[row][col];
					grid[row][col] = 0;
				}	
			}
		}
	}		
}

//draws all the tiles on the current screen
void draw_all_tiles(){
	for(int row = 0; row < 4; row++){
		for(int col = 0; col < 4; col++){
			if(grid[row][col] != 0){
				draw_tile(col, row, grid[row][col]);
			}
		}
	}
}

//draws a specific tile at a given location
void draw_tile(int x, int y, int num){
	int midScreenX = RESOLUTION_X / 2;
    int midScreenY = RESOLUTION_Y / 2;
	int gridMinX = midScreenX - midScreenY;
	int gridMaxX = midScreenX + midScreenY;
	int gridSideLength = gridMaxX - gridMinX;
	int boxSideLength = (gridSideLength - (5 * GRID_LINE_WIDTH)) / 4;
	
	int xStart = gridMinX + (GRID_LINE_WIDTH * (x + 1)) + (boxSideLength * x);
	int yStart = (GRID_LINE_WIDTH * (y + 1)) + (boxSideLength * y);
	
	draw_box(xStart, xStart + boxSideLength, yStart, yStart + boxSideLength, ORANGE);
	
}

// draws the grid of the game
void draw_grid() {
	// main constants, should move them to global

	int midScreenX = RESOLUTION_X / 2;
    int midScreenY = RESOLUTION_Y / 2;
	int gridMinX = midScreenX - midScreenY;
	int gridMaxX = midScreenX + midScreenY;
	int gridSideLength = gridMaxX - gridMinX;
	int boxSideLength = (gridSideLength - (5 * GRID_LINE_WIDTH)) / 4;

	// draw grid background
	draw_box(gridMinX, gridMaxX, 0, RESOLUTION_Y, GREY);
	
	// draw box spaces
	for (int row = 0; row < 4; row ++) {
		for (int col = 0; col < 4; col++) {
			
			int xStart = gridMinX + (GRID_LINE_WIDTH * (col + 1)) + (boxSideLength * col);
			int yStart = (GRID_LINE_WIDTH * (row + 1)) + (boxSideLength * row);
			draw_box(xStart, xStart + boxSideLength, yStart, yStart + boxSideLength, WHITE);  
		}
	}
}
	
void draw_box(int minX, int maxX, int minY, int maxY, short int colour) {
	for (int y = minY; y < maxY; y++) {
		draw_line(minX, y, maxX, y, colour);
	}
}

// swaps two values
void swap(int *x, int *y) {
	int temp = *x;
	*x = *y;
	*y = temp;
}

// launches swap cycle and waits for swap to happen before returning
void wait_for_vsync() {
	volatile int *pixel_ctrl_ptr = (int*)0xff203020;
	int status;
	// launch the swap process
	*pixel_ctrl_ptr = 1;
	//poll status bit
	status = *(pixel_ctrl_ptr + 3);
	while (!(status & 0x01) == 0) {
		status = *(pixel_ctrl_ptr + 3);
	}
	// status bit s = 0, can exit
}


// draws a line from (x1, y1) to (x2, y2)
void draw_line(int x0, int y0, int x1, int y1, short int colour) {
	bool is_steep = abs(y1-y0) > abs(x1-x0);
	if (is_steep) {
		swap(&x0, &y0);
		swap(&x1, &y1);
	}
	
	if (x0 > x1) {
		swap(&x0, &x1);
		swap(&y0, &y1);
	}
	
	int dx = x1 - x0;
	int dy = abs(y1 - y0);
	int error = -(dx / 2);
	int y = y0;
	int y_step = -1;
	
	if (y0 < y1) {
		y_step = 1;
	}
	
	for (int x = x0; x <= x1; x++) {
		if (is_steep) {
			plot_pixel(y, x, colour);
		} else {
			plot_pixel(x, y, colour);
		}
		error = error + dy;
		if(error >= 0) {
			y = y + y_step;
			error = error - dx;
		}
	}
}



// clears the VGA screen
void black_screen() {
	for (int x = 0; x < RESOLUTION_X; x++) {
		for (int y = 0; y < RESOLUTION_Y; y++) {
			plot_pixel(x, y, BLACK);
		}
	}
}

// draws a pixel on screen
void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}
