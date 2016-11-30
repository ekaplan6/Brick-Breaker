#include <stdlib.h>
#include <stdio.h>
#include "myLib.h"
#include "text.h"
#include "background1.h"
#include "background2.h"
#include "plank.h"


// State enum definition
enum GBAState {
	STARTSCREEN,
	INITIALIZE_GAME,
	GAME,
	GAME_OVER
};

int main()
{
	REG_DISPCTL = MODE3 | BG2_ENABLE;

	enum GBAState state = STARTSCREEN;
	char *buffer1;
	char *buffer2;
	char buffer3[100];
	char oldBuffer3[100];

	BRICK bricks[NUMBRICKS];
	u16 brickColors[] = {RED, GREEN, BLUE, MAGENTA, CYAN, YELLOW, WHITE};
	int numBrickColors = sizeof(brickColors)/sizeof(brickColors[0]);


	int plankcol;
	int oldPlankCol;

	BALL ball;
	BALL oldBall;

	int counter = 0;

	for(int i=0; i < BRICKROWS; i++) {
		for (int j = 0; j < BRICKCOLS ;j++) {
			bricks[i*BRICKCOLS + j].row = BRICKHEIGHT*i;
			bricks[i*BRICKCOLS + j].col = BRICKWIDTH*j;
			bricks[i*BRICKCOLS + j].valid =  1;
			bricks[i*BRICKCOLS + j].color = brickColors[rand()%numBrickColors];
		}
	}

	plankcol = 90;
	oldPlankCol = plankcol;
	
	ball.row = 100;
	ball.col = 110;
	ball.rd = -BALLSPEED;
	ball.cd = -BALLSPEED;
	oldBall = ball;

	int Go = 1;

	int hitSelect = 0;

	while(1) {
		waitForVblank();
		switch(state) {
		case STARTSCREEN:
			buffer1 = "Welcome to Brick!!!! Press A.";
			buffer2 = "Made by Evan Kaplan!";
			//sprintf(buffer3, "Your score is %d", counter);
			waitForVblank();
			resetScreen(BLACK);

			drawImage3(0, 0, COOLWALLPAPER1_HEIGHT, COOLWALLPAPER1_WIDTH, coolwallpaper1);
			drawString(130, 40, buffer1, WHITE);
			drawString(140, 65, buffer2, YELLOW);
			//drawString(86, 60, buffer3, YELLOW);
			while (KEY_DOWN_NOW(BUTTON_A) != 0) {}
			while (KEY_DOWN_NOW(BUTTON_A) == 0) {}

			state = INITIALIZE_GAME;
			break;

		case INITIALIZE_GAME:
			counter = 0;
			sprintf(buffer3, "Your score is %d", counter);
			drawString(86, 78, buffer3, YELLOW);
			Go = 1;
			hitSelect = 0;
			resetScreen(BLACK);
			//initialize bricks and stuff
			//BRICK *currentBrick;

			//initializing bricks
			for(int i=0; i < BRICKROWS; i++) {
				for (int j = 0; j < BRICKCOLS ;j++) {
					bricks[i*BRICKCOLS + j].row = BRICKHEIGHT*i;
					bricks[i*BRICKCOLS + j].col = BRICKWIDTH*j;
					bricks[i*BRICKCOLS + j].valid =  1;
					bricks[i*BRICKCOLS + j].color = brickColors[rand()%numBrickColors];
				}
			}

			plankcol = 90;
			oldPlankCol = plankcol;
			
			ball.row = 100;
			ball.col = 110;
			ball.rd = -BALLSPEED;
			ball.cd = -BALLSPEED;
			oldBall = ball;

			for(int i=0; i<NUMBRICKS; i++) {
				drawRect(bricks[i].row, bricks[i].col, BRICKHEIGHT-1, BRICKWIDTH-1, bricks[i].color);
			}

			drawImage3Plank(PLANKROW, plankcol, PLANKHEIGHT, PLANKWIDTH, plank);
			//drawRect(PLANKROW, plankcol, PLANKHEIGHT, PLANKWIDTH, PLANKCOLOR);
			drawRect(ball.row, ball.col, BALLSIZE, BALLSIZE, BALLCOLOR);


			state = GAME;
			break;


		case GAME:
			//game play
			while (Go) {

				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					Go = 0;
					hitSelect = 1;
					state = STARTSCREEN;
				}

				//move plank
				if(KEY_DOWN_NOW(BUTTON_LEFT))
				{
					plankcol = plankcol - PLANKSPEED;
					if(plankcol<0) plankcol = 0;
				}
				if(KEY_DOWN_NOW(BUTTON_RIGHT))
				{
					plankcol = plankcol + PLANKSPEED;
					if(plankcol>239-PLANKWIDTH) plankcol = 239 - PLANKWIDTH;
				}

				ball.row += ball.rd;
				ball.col += ball.cd;
				
				//ball does not go out of bounds
				if(ball.row < 0)
				{
					ball.row = 0;
					ball.rd = -ball.rd;
				}
				if(ball.row > 159 - BALLSIZE)
				{
					ball.row = 159 - BALLSIZE;
					ball.rd = -ball.rd;
					Go = 0;
				}
				if(ball.col < 0)
				{
					ball.col = 0;
					ball.cd = -ball.cd;
				}
				if(ball.col > 239-BALLSIZE)
				{
					ball.col = 239-BALLSIZE;
					ball.cd = -ball.cd;
				}


				//collisions with bricks
				if (ball.row <= BRICKROWS*BRICKHEIGHT + 5) {
					int coll = collisionBrick(ball, bricks);
					if (coll) {
						if (coll <= 9) {
							if ((coll != 2) & (coll != 8)) {

								ball.row = oldBall.row;
								ball.rd = -ball.rd;

							}
							if ((coll != 4) & (coll != 6)) {

								ball.col = oldBall.col;
								ball.cd = -ball.cd;
							}
						} else {
							if ((coll != 10) & (coll != 16)) {

								ball.row = oldBall.row;
								ball.rd = -ball.rd;

							}
							if ((coll != 12) & (coll != 14)) {

								ball.col = oldBall.col;
								ball.cd = -ball.cd;
							}
						}
						counter++;
						if (counter == NUMBRICKS) {
							Go = 0;
						}
						sprintf(buffer3, "Your score is %d", counter);
					}
				}

				//collision with plank
				int coll2 = collisionPlank(ball, plankcol);

				if (coll2) {
					ball.row = PLANKROW - BALLSIZE;
					ball.rd = -BALLSPEED;
				}

				waitForVblank();

				drawString(86, 78, oldBuffer3, BLACK);
				drawString(86, 78, buffer3, YELLOW);
				
				for (int i = 0; i <100; i++) {
					oldBuffer3[i] = buffer3[i];
				}

				for (int i = 0; i < NUMBRICKS; i++) {
					if (bricks[i].valid) {
						drawRect(bricks[i].row, bricks[i].col, BRICKHEIGHT-1, BRICKWIDTH-1, bricks[i].color);
					} else {
						drawRect(bricks[i].row, bricks[i].col, BRICKHEIGHT, BRICKWIDTH, BLACK);
					}
				}
				
				
				drawRect(PLANKROW, oldPlankCol, PLANKHEIGHT, PLANKWIDTH, BLACK);
				//drawRect(PLANKROW, plankcol, PLANKHEIGHT, PLANKWIDTH, PLANKCOLOR);
				drawImage3Plank(PLANKROW, plankcol, PLANKHEIGHT, PLANKWIDTH, plank);

				drawRect(oldBall.row, oldBall.col, BALLSIZE, BALLSIZE, BLACK);
				drawRect(ball.row, ball.col, BALLSIZE, BALLSIZE, BALLCOLOR); //put a picture here


				oldBall = ball;
				oldPlankCol = plankcol;
			}

			if (hitSelect == 0) {
				state = GAME_OVER;
			}

			break;

		case GAME_OVER:
			if (counter != NUMBRICKS) {
				buffer1 = "You Lost :(  Press A";
			} else {
				buffer1 = "NICE JOB!!! You Win!";
			}
			buffer2 = "Made by Evan Kaplan!";
			sprintf(buffer3, "Your final score is %d!", counter);
			waitForVblank();

			resetScreen(BLACK);
			drawImage3(0, 0, ENDSCREEN_HEIGHT, ENDSCREEN_WIDTH, endscreen);
			drawString(110, 65, buffer1, YELLOW);
			drawString(95, 60, buffer3, WHITE);

			drawString(130, 70, buffer2, YELLOW);
			while ((KEY_DOWN_NOW(BUTTON_A) == 0) & (KEY_DOWN_NOW(BUTTON_SELECT) == 0)) {

			}

			state = STARTSCREEN;
		}

	}

}