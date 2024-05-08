#include "sprite_dev.h"
#include "usb_example.h"
#include "usb.h"
//#include "color_codes.h"
//

#define sev() __asm__("sev")
#define ARM1_STARTADR 0xFFFFFFF0
#define ARM1_BASEADDR 0x10080000
#define COMM_VAL (*(volatile unsigned long *)(0xFFFF0000))

unsigned int mouseCursor[] = {
	0xFFFFFF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFFFF, 0x000000, 0x000000
};

unsigned int prev_mouse[64] = {NULL};

// Initialize buffer that writes to VGA
int *image_buffer_pointer = (int *)0x00900000;
Page curr = {1};

int main()
{

	UsbInstancePtr = &usbWithHostInstance.usbInstance;
	USB_Setup(&INTCInst, &usbWithHostInstance, XPAR_XUSBPS_0_DEVICE_ID, XPAR_XUSBPS_0_INTR, reinterpret_cast<void*>(&UsbIntrHandler));
	USB_SetupPolling(&usbWithHostInstance);
	COMM_VAL = 0;

	//Disable cache on OCM
	// S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
	Xil_SetTlbAttributes(0xFFFF0000,0x14de2);
	Xil_Out32(ARM1_STARTADR, ARM1_BASEADDR);
	xil_printf("IN FIRST CORE MAIN\r\n");

	dmb(); //waits until write has finished
	sev();

	Xil_DCacheDisable();
	// Set background to main menu
	DrawMenu();
	storeCurr(); //store start of game mouse cursor position

	gameEz();

	while(1) {
		// NOP
	}
	return 0;
}

void gameEz() {
	initializeBlockArray();
	for (int i = 0; i< 10; i++) {
		draw_mine_block_ez();
	}
}


// Prints Cursor into the Screen
void DrawCursor(Mouse sprite) {
	// Draw a sprite at its position
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            int pixel_color = sprite.color[y*16+x]; // Get sprite color
            // Draw sprite pixel at its position
            *((int *)(image_buffer_pointer + (sprite.y + y) * WIDTH + (sprite.x + x))) = pixel_color;
        }
    }
}

void storeCurr() {
	memcpy(prev_mouse, image_buffer_pointer, sizeof(mouseCursor));
}

void storePrev(Mouse store) {
	for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            prev_mouse[y*16+x] = *((int *)(image_buffer_pointer + (store.y + y) * WIDTH + (store.x + x)));
        }
    }
}

int gameStatus = 0;
void generate_cursor(int xPos, int yPos, int wasX, int wasY, bool left, bool right, bool wasL, bool wasR) {
	Mouse wasMouse = {wasX, wasY, prev_mouse};
	DrawCursor(wasMouse); // Draw previous mouse position (Erase last mouse)

	Mouse background = {xPos, yPos, prev_mouse};
	storePrev(background); // Store new mouse position background

	Mouse mice = {xPos, yPos, &mouseCursor[0]};
	DrawCursor(mice); // Draw mouse cursor at new position

	//button clicks
	if (left) {
		if(curr.pageNum == 1) { // currently on menu page
			if ((yPos >= 910)&&(yPos <= 980)&&(xPos >= 480)&&(xPos <= 800)) { // Instructions button clicked.
				DrawInstrPage();
				storePrev(wasMouse);
				DrawCursor(background);
			} else {
				xil_printf("Clicked on menu background.\n");
			}

		} else if(curr.pageNum == 2) { // currently on instruction page
			if((xPos<=1275) && (yPos<=1020) && (xPos>=1070) && (yPos>=932)) {
				DrawMenu();
				storePrev(wasMouse);
				DrawCursor(background);
			} else {
				xil_printf("clicked background.\n");
			}
		} else if(curr.pageNum == 5) { // currently on game over screen
			if((xPos>=110) && (yPos>=885) && (xPos<=460) && (yPos<=970)) { //play again
				DrawGame();
				gameEz();
				storePrev(wasMouse);
				DrawCursor(background);
				curr.pageNum = 3;
			} else if((xPos>=785) && (yPos>=885) && (xPos<=1170) && (yPos<=970)) { //back to main menu
				DrawMenu();
				storePrev(wasMouse);
				DrawCursor(background);
			}
		}

		else if(curr.pageNum == 3) { // currently on game page
			DrawCursor(wasMouse);
			gameStatus = showBlock(xPos, yPos);

			if(gameStatus == 1) { // game over
				DrawGameOver();
				print_grid();
				storePrev(wasMouse);
				DrawCursor(background);
			} else if(gameStatus == 2) { // game win
				DrawGameComplete();
				print_grid();
				storePrev(wasMouse);
				DrawCursor(background);
			}
			storePrev(wasMouse);
			DrawCursor(background);
		}
		else if(curr.pageNum == 4) { // currently on game win page
			if((xPos>=110) && (yPos>=885) && (xPos<=460) && (yPos<=970)) { //play again
				DrawGame();
				gameEz();
				storePrev(wasMouse);
				DrawCursor(background);
			}else if((xPos>=785) && (yPos>=885) && (xPos<=1170) && (yPos<=970)) { //back to main menu
				DrawMenu();
				storePrev(wasMouse);
				DrawCursor(background);
			}
		}

	}

	if(right) {
		if(curr.pageNum == 3) { // currently on game page
			if((xPos >= 484) && (xPos <= 804) && (yPos >= 350) &&(yPos <= 694) ) { //within game grid bounds
				DrawCursor(wasMouse);
				assign_flag_block(xPos, yPos);
				storePrev(wasMouse);
				DrawCursor(background);
			}
		} else { // start game
			DrawGame();
			gameEz();
			storePrev(wasMouse);
			DrawCursor(background);
		}

	}
}


void DrawSprite(Block block) {
    int i = (block.index % diff_EZ) + 1;
    int j = (block.index / diff_EZ);

    int posX = i*(WIDTH / 32) - (SQUARE_SIDE / 2) + ((WIDTH - 360) / 2);
    int posY = j*(HEIGHT / 24) - (SQUARE_SIDE / 2) + ((HEIGHT - 292) / 2);

    // Draw a Sprite- pixel by pixel at given position
    for (int y = 0; y < SQUARE_SIDE; ++y) {
        for (int x = 0; x < SQUARE_SIDE; ++x) {
            unsigned int pixel_color = block.color[y*SQUARE_SIDE+x]; // Get sprite color
            // Draw sprite pixel at its position
            *((int *)(image_buffer_pointer + (posY + y) * WIDTH + (posX + x))) = pixel_color;
        }
    }

}

void DrawNumbers(Block block) {
    int i = (block.index % diff_EZ) + 1;
    int j = (block.index / diff_EZ);

    int posX = i*(WIDTH / 32) - (SQUARE_SIDE / 2) + ((WIDTH - 360) / 2);
    int posY = j*(HEIGHT / 24) - (SQUARE_SIDE / 2) + ((HEIGHT - 292) / 2);

	// Draw a Sprite- pixel by pixel at given position
    for (int y = 0; y < SQUARE_SIDE; ++y) {
        for (int x = 0; x < SQUARE_SIDE; ++x) {
            unsigned int pixel_color = block.color[y*SQUARE_SIDE+x]; // Get sprite color
            if (pixel_color <= 0x7F7F7F) pixel_color = 0xFFFFFF; // change later for background
            else pixel_color = 0x000000;

            // Draw sprite pixel at its position
            *((int *)(image_buffer_pointer + (posY + y) * WIDTH + (posX + x))) = pixel_color;
        }
    }
}

void DrawFlagged(Block block) {
	int i = (block.index % diff_EZ) + 1;
	int j = (block.index / diff_EZ);

	int posX = i*(WIDTH / 32) - (SQUARE_SIDE / 2) + ((WIDTH - 360) / 2);
	int posY = j*(HEIGHT / 24) - (SQUARE_SIDE / 2) + ((HEIGHT - 292) / 2);

	// Draw a Sprite- pixel by pixel at given position
	for (int y = 0; y < SQUARE_SIDE; ++y) {
		for (int x = 0; x < SQUARE_SIDE; ++x) {
			unsigned int pixel_color = block.color[y*SQUARE_SIDE+x]; // Get sprite color
			if (pixel_color >= 0x3C3C3C) pixel_color = 0xFFFF00; // change later for background
			else pixel_color = 0x000000;

			// Draw sprite pixel at its position
			*((int *)(image_buffer_pointer + (posY + y) * WIDTH + (posX + x))) = pixel_color;
		}
	}
}
//Create MINE blocks on easy difficulty
void draw_mine_block_ez() {
	assign_mine();
}

/* for 8 by 8 configuration- EASY
 * 				8			+	(32*8)+(5*(8-1)) = 256+35 =  291	+			8
 * 		space on the left		blocks with 5 pixel spacing			space on the right
 */

/* for 24 by 24 configuration- HARD
 * 				8			+	(32*24)+(5*24) = 786+115 = 	906		+			8
 * 		space on the left		blocks with 5 pixel spacing			space on the right
 */

void DrawGame() {
	int *image1_pointer = (int *)0x018D2008;
	int NUM_BYTES_BUFFER = 5542880;
	memcpy(image_buffer_pointer, image1_pointer, NUM_BYTES_BUFFER);
	curr.pageNum = 3;
	xil_printf("END OF FUNCTION\r\n");
}

void DrawGameComplete() {
	int *image1_pointer = (int *)0x03876018;
	int NUM_BYTES_BUFFER = 5542880;
	memcpy(image_buffer_pointer, image1_pointer, NUM_BYTES_BUFFER);
	curr.pageNum = 4;
	xil_printf("END OF FUNCTION\r\n");
}

void DrawGameOver() {
	int *image1_pointer = (int *)0x0308D014;
	int NUM_BYTES_BUFFER = 5542880;
	memcpy(image_buffer_pointer, image1_pointer, NUM_BYTES_BUFFER);
	curr.pageNum = 5;
	xil_printf("END OF FUNCTION\r\n");
}

void DrawMenu() {
	int *image1_pointer = (int *)0x020BB00C;
	int NUM_BYTES_BUFFER = 5542880;
	memcpy(image_buffer_pointer, image1_pointer, NUM_BYTES_BUFFER);
	curr.pageNum = 1;
	xil_printf("END OF FUNCTION\r\n");
}

void DrawInstrPage() {
	int *image1_pointer = (int *)0x028A4010;
	int NUM_BYTES_BUFFER = 5542880;
	memcpy(image_buffer_pointer, image1_pointer, NUM_BYTES_BUFFER);
	curr.pageNum = 2;
	xil_printf("END OF FUNCTION\r\n");
}

