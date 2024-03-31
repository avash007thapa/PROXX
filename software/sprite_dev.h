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

//#include "audio.h"
#include "game_design.h"

#include <stdio.h>
#include <unistd.h>
#include <cstdlib>

extern uint8_t track[];
extern int track_size;

#define UART_BASEADDR 			XPAR_PS7_UART_1_BASEADDR
#define INTC_DEVICE_ID 			XPAR_PS7_SCUGIC_0_DEVICE_ID
#define BTNS_DEVICE_ID			XPAR_AXI_GPIO_DEVICE_ID
#define INTC_GPIO_INTERRUPT_ID 	XPAR_FABRIC_AXI_GPIO_IP2INTC_IRPT_INTR
#define BTN_INT 				XGPIO_IR_CH1_MASK
#define	I2S_DATA_TX_L_REG
#define	I2S_DATA_TX_R_REG

/* ---------------------------------------------------------------------------- *
 * 							Initialize Structs									*
 * ---------------------------------------------------------------------------- */
// Sprite structure
typedef struct {
//    int x, y; // Position
    int index; // Calculate position according to numbered blocks
    unsigned int *color;
    bool mine; // Is this block a mine or not?

} Sprite;

/* ---------------------------------------------------------------------------- *
 * 							Prototype Functions									*
 * ---------------------------------------------------------------------------- */
void uart_input(int* image_buffer_pointer);
void DrawSprite(int *image_buffer_pointer, Sprite sprite);
void DrawMenu(int *image_buffer_pointer);
void DrawInstrPage(int* image_buffer_pointer);
void generate_two_blocks_ez(int *image_buffer_pointer);
void generate_one_blocks_ez(int *image_buffer_pointer);
void assign_mine_blocks_ez(int *image_buffer_pointer);

/* ---------------------------------------------------------------------------- *
 * 							Screen Information									*
 * ---------------------------------------------------------------------------- */
// Screen width / height
const int WIDTH 	  = 1280;
const int HEIGHT 	  = 1024;



#endif /* SPRITE_DEV_H_ */















