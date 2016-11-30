#include "myLib.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;

void setPixel(int row, int col, unsigned short color)
{
	videoBuffer[OFFSET(row,col, 240)] = color;
}


void drawRect(int row, int col, int height, int width, volatile unsigned short color)
{
	for(int r=0; r<height; r++)
	{

		REG_DMA3SAD = (u32)&color;
		REG_DMA3DAD = (u32)(&videoBuffer[OFFSET(row+r, col, 240)]);
		REG_DMA3CNT = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

void resetScreen(volatile unsigned short color) {
	REG_DMA3SAD = (u32)&color;
	REG_DMA3DAD = (u32)(videoBuffer);
	REG_DMA3CNT = 38400 | DMA_ON | DMA_SOURCE_FIXED;
}

//separate into how the ball hits the brick to know which way it should move next
//returns int telling where the ball was hit
int collisionBrick(BALL ball, BRICK * bricks) {
	BRICK currentBrick;
	int x1 = ball.col;
	int y1 = ball.row;
	int x2;
	int y2;
	int out = 0;
	int i = 0;
	int flag = 1;
	while ((i < NUMBRICKS) & (flag)) {
		currentBrick = bricks[i];
		if (currentBrick.valid) {
			x2 = currentBrick.col;
			y2 = currentBrick.row;
			if ((x1 <= x2 + BRICKWIDTH) & (x1 >= x2 + BRICKWIDTH -2)) {
				if ((y1 <= y2 + BRICKHEIGHT) & (y1 >= y2 +BRICKHEIGHT - 2)) {
					out = 1;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y1 < y2 + BRICKHEIGHT - 2) & (y1 >= y2 + 2)) {
					out = 2;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y1 < y2 + 2) & (y1 >= y2)) {
					out = 3;
					bricks[i].valid = 0;
					flag = 0;
				}
			} else if ((x1 < x2 + BRICKWIDTH - 2) & (x1 >= x2 + 2)) {
				if ((y1 <= y2 + BRICKHEIGHT) & (y1 >= y2 +BRICKHEIGHT - 2)) {
					out = 4;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y1 < y2 + BRICKHEIGHT - 2) & (y1 >= y2 + 2)) {
					out = 5;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y1 < y2 + 2) & (y1 >= y2)) {
					out = 6;
					bricks[i].valid = 0;
					flag = 0;
				}
			} else if ((x1 < x2 + 2) & (x1 >= x2)) {
				if ((y1 <= y2 + BRICKHEIGHT) & (y1 >= y2 +BRICKHEIGHT - 2)) {
					out = 7;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y1 < y2 + BRICKHEIGHT - 2) & (y1 >= y2 + 2)) {
					out = 8;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y1 < y2 + 2) & (y1 >= y2)) {
					out = 9;
					bricks[i].valid = 0;
					flag = 0;
				}
			} else if ((x2 <= x1 + BALLSIZE) & (x2 >= x1 + BALLSIZE -2)) {
				if ((y2 <= y1 + BALLSIZE) & (y2 >= y1 +BALLSIZE - 2)) {
					out = 10;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y2 < y1 + BALLSIZE - 2) & (y2 >= y1 + 2)) {
					out = 11;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y2 < y1 + 2) & (y2 >= y1)) {
					out = 12;
					bricks[i].valid = 0;
					flag = 0;
				}
			} else if ((x2 < x1 + BALLSIZE - 2) & (x2 >= x1 + 2)) {
				if ((y2 <= y1 + BALLSIZE) & (y2 >= y1 +BALLSIZE - 2)) {
					out = 13;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y2 < y1 + BALLSIZE - 2) & (y2 >= y1 + 2)) {
					out = 14;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y2 < y1 + 2) & (y2 >= y1)) {
					out = 15;
					bricks[i].valid = 0;
					flag = 0;
				}
			} else if ((x2 < x1 + 2) & (x2 >= x1)) {
				if ((y2 <= y1 + BALLSIZE) & (y2 >= y1 +BALLSIZE - 2)) {
					out = 16;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y2 < y1 + BALLSIZE - 2) & (y2 >= y1 + 2)) {
					out = 17;
					bricks[i].valid = 0;
					flag = 0;
				} else if ((y2 < y1 + 2) & (y2 >= y1)) {
					out = 18;
					bricks[i].valid = 0;
					flag = 0;
				}
			}
		} else {
			out = 0;
		}
		i++;
	}
	return out;
}

//return if ball collides with plank
int collisionPlank(BALL ball, int plankcol) {
	int ballx1 = ball.col;
	int ballx2 = ball.col + BALLSIZE;
	int bally1 = ball.row + BALLSIZE;
	int plankx1 = plankcol;
	int plankx2 = plankcol + PLANKWIDTH;
	int planky1 = PLANKROW;
	int out = 0;

	if (bally1 >= planky1) {
		if (((ballx2 >= plankx1) & (ballx2 <= plankx2)) | ((ballx1 >= plankx1) & (ballx1 <= plankx2))) {
			out = 1;
		}
	}
	
	return out;
}

void drawImage3(int row, int col, int height, int width, const unsigned short *image)
{
	for(int r=0; r<height; r++)
	{

		REG_DMA3SAD = (u32)&image[OFFSET(row+r, col, 240)];
		REG_DMA3DAD = (u32)&videoBuffer[OFFSET(row+r, col, 240)];
		REG_DMA3CNT = width | DMA_ON | DMA_SOURCE_INCREMENT;
	}
}

void drawImage3Plank(int row, int col, int height, int width, const unsigned short *image)
{
	for(int r=0; r<height; r++)
	{

		REG_DMA3SAD = (u32)&image[OFFSET(r, 0, 80)];
		REG_DMA3DAD = (u32)&videoBuffer[OFFSET(row+r, col, 240)];
		REG_DMA3CNT = width | DMA_ON | DMA_SOURCE_INCREMENT;
	}
}