#ifndef SPRITE_DEV_H_
#define SPRITE_DEV_H_

/* ---------------------------------------------------------------------------- *
 * 								Header Files									*
 * ---------------------------------------------------------------------------- */

#include "xil_types.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xuartps.h"
#include "xgpio.h"
#include "xil_cache.h"
#include "xil_mmu.h"
#include "xil_printf.h"
#include "xpseudo_asm.h"
#include "xil_exception.h"


//#include "audio.h"
#include "game_design.h"

#include <stdio.h>
#include <unistd.h>
#include <cstdlib>


#define UART_BASEADDR 			XPAR_PS7_UART_1_BASEADDR
#define INTC_DEVICE_ID 			XPAR_PS7_SCUGIC_0_DEVICE_ID
#define BTNS_DEVICE_ID			XPAR_AXI_GPIO_DEVICE_ID
#define INTC_GPIO_INTERRUPT_ID 	XPAR_FABRIC_AXI_GPIO_IP2INTC_IRPT_INTR
#define BTN_INT 				XGPIO_IR_CH1_MASK


/* ---------------------------------------------------------------------------- *
 * 							PROTOTYPE FUNCTIONS									*
 * ---------------------------------------------------------------------------- */
// Mouse structure
typedef struct {
    int x, y; // Position
    unsigned int *color;
} Mouse;

// Mouse structure
typedef struct {
	int pageNum;
	// page 1 is menu
	// page 2 is instruction
	// page 3 is game page
	// page 4 is game win
	// page 5 is game over
} Page;

void uart_input();
void DrawSprite(Block block);
void DrawMenu();
void DrawInstrPage();
void DrawGameOver();
void DrawGameComplete();
void DrawGame();
void draw_mine_block_ez();
void gameEz();

void generate_cursor(int xPos, int yPos, int wasX, int wasY, bool left, bool right, bool wasL, bool wasR);
void DrawCursor(Mouse sprite);
void storePrev(Mouse store);
void storeCurr();
void DrawNumbers(Block block);
void DrawFlagged(Block block);

/* ---------------------------------------------------------------------------- *
 * 							DISPLAY Information									*
 * ---------------------------------------------------------------------------- */

const int WIDTH 	  = 1280;
const int HEIGHT 	  = 1024;
const int SQUARE_SIDE = 32;

#endif /* SPRITE_DEV_H_ */















