#include "sprite_dev.h"

const int SQUARE_SIDE = 32;
const int diff_EZ = 8; // 8 by 8 blocks

int main()
{
//	int status;

	//----------------------------------------------------
	// INITIALIZE THE PERIPHERALS & SET DIRECTIONS OF GPIO
	//----------------------------------------------------
	// Initialize Push Buttons
//	status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);
//	if(status != XST_SUCCESS) return XST_FAILURE;

	// Set all buttons direction to inputs
//	XGpio_SetDataDirection(&BTNInst, 1, 0xFF);

	// Initialize interrupt controller
//	status = IntcInitFunction(INTC_DEVICE_ID, &BTNInst);
//	if(status != XST_SUCCESS) return XST_FAILURE;

	Xil_DCacheDisable();

	// Initialize buffer that writes to VGA
	int *image_buffer_pointer = (int *)0x00900000;

	uart_input(image_buffer_pointer);
	return 0;
}


void uart_input(int* image_buffer_pointer) {
	//	 FOR UART DEVELOPMENT-- from adventures_with_ip.c
	u8 inp = 0x00;
	u32 CntrlRegister;
	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);
	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
			((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) | XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));
	xil_printf("\r\n\r\n");
	xil_printf("Embedded Sprite development Demo\r\n");
	xil_printf("Enter the number to generate\r");
	xil_printf("OR type 'm' for mines\r\n");
	xil_printf("----------------------------------------\r\n");

	// Currently sets background to vertical colored lines
	DrawBackground(image_buffer_pointer);

	while (!XUartPs_IsReceiveData(UART_BASEADDR));

	inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	// Select function based on UART input
	switch(inp) {
	case '1':
		xil_printf("GENERATING ONE BLOCKS\r\n");
		// Create 1 digits square box
		generate_one_blocks_ez(image_buffer_pointer);
		break;
	case '2':
		xil_printf("GENERATING TWO BLOCKS\r\n");
		// Create white square box
		generate_two_blocks_ez(image_buffer_pointer);
		break;
	case 'm':
			xil_printf("GENERATING TWO BLOCKS\r\n");
			// Create white square box
			generate_mine_blocks_ez(image_buffer_pointer);
			break;
	default:
		uart_input(image_buffer_pointer);
		break;
	} // switch
}


// Prints Sprite into the Screen
void DrawSprite(int *image_buffer_pointer, Sprite sprite) {
    // Draw a sprite at its position
    for (int y = 0; y < SQUARE_SIDE; ++y) {
        for (int x = 0; x < SQUARE_SIDE; ++x) {
            int pixel_color = sprite.color[y*SQUARE_SIDE+x]; // Get sprite color
            // Draw sprite pixel at its position
            *((int *)(image_buffer_pointer + (sprite.y + y) * WIDTH + (sprite.x + x))) = pixel_color;
        }
    }
}

int bar_div = 5;
void DrawBackground(int* image_buffer_pointer) {
	for (int bar = 0; bar < bar_div; ++bar) {
		// Calculate the start and end positions of the current bar
		int startX = bar * WIDTH / bar_div;
		int endX = (bar + 1) * WIDTH / bar_div;
		// Loop over the pixels in the current bar
		for (int y = 0; y < HEIGHT; ++y) {
			for (int x = startX; x < endX; ++x) {
				// Set color based on the position of each bar
				*((int*) ((image_buffer_pointer + y * WIDTH + x))) =
						COLOR_ARRAY[bar];
			}
		}
	}
}

//Create ONE digit blocks on easy difficulty
void generate_one_blocks_ez(int *image_buffer_pointer) {
	for (int j = 0; j < diff_EZ; ++j) {
		for (int i = 1; i <= diff_EZ; ++i) {
			Sprite sprite = { i*(WIDTH / 32) - (SQUARE_SIDE / 2) + ((WIDTH - 360) / 2),
							j*(HEIGHT / 24) - (SQUARE_SIDE / 2) + ((HEIGHT - 292) / 2),
							&oneDigitBlock[0]
							};
			DrawSprite(image_buffer_pointer, sprite);
		}
	}

	while (!XUartPs_IsReceiveData(UART_BASEADDR));
	xil_printf("Type 'q' to go back to background\r\n");
	if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q'){
		uart_input(image_buffer_pointer);
	} else {
		generate_one_blocks_ez(image_buffer_pointer);
	}
}

// Create TWO square boxes for easy difficulty
void generate_two_blocks_ez(int *image_buffer_pointer) {
	for (int j = 0; j < diff_EZ; ++j) {
		for (int i = 1; i <= diff_EZ; ++i) {
			Sprite sprite = { i*(WIDTH / 32) - (SQUARE_SIDE / 2) + ((WIDTH - 360) / 2),
								j*(HEIGHT / 24) - (SQUARE_SIDE / 2) + ((HEIGHT - 292) / 2),
								&twoDigitBlock[0]  // WHITE COLOR
							};

			DrawSprite(image_buffer_pointer, sprite);
		}
	}

	while (!XUartPs_IsReceiveData(UART_BASEADDR));
	xil_printf("Type 'q' to go back to background\r\n");
	if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q'){
		uart_input(image_buffer_pointer);
	} else {
		generate_two_blocks_ez(image_buffer_pointer);
	}
}

//Create MINE blocks on easy difficulty
void generate_mine_blocks_ez(int *image_buffer_pointer) {
	for (int j = 0; j < diff_EZ; ++j) {
		for (int i = 1; i <= diff_EZ; ++i) {
			Sprite sprite = { i*(WIDTH / 32) - (SQUARE_SIDE / 2) + ((WIDTH - 360) / 2),
							j*(HEIGHT / 24) - (SQUARE_SIDE / 2) + ((HEIGHT - 292) / 2),
							&mineBlock[0]
							};
			DrawSprite(image_buffer_pointer, sprite);
		}
	}

	while (!XUartPs_IsReceiveData(UART_BASEADDR));
	xil_printf("Type 'q' to go back to background\r\n");
	if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q'){
		uart_input(image_buffer_pointer);
	} else {
		generate_mine_blocks_ez(image_buffer_pointer);
	}
}


/* for 8 by 8 configuration- HARD
 * 				8			+	(32*8)+(5*(8-1)) = 256+35 =  291	+			8
 * 		space on the left		blocks with 5 pixel spacing			space on the right
 */

/* for 24 by 24 configuration- HARD
 * 				8			+	(32*24)+(5*24) = 786+115 = 	906		+			8
 * 		space on the left		blocks with 5 pixel spacing			space on the right
 */
