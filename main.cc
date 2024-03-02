#include "xgpio.h"

#include "sprite_dev.h"
#include "xil_cache.h"
#include "xuartps.h"

// Parameter definitions
#define INTC_DEVICE_ID 			XPAR_PS7_SCUGIC_0_DEVICE_ID
#define BTNS_DEVICE_ID			XPAR_AXI_GPIO_DEVICE_ID
#define INTC_GPIO_INTERRUPT_ID 	XPAR_FABRIC_AXI_GPIO_IP2INTC_IRPT_INTR
#define BTN_INT 				XGPIO_IR_CH1_MASK
#define UART_BASEADDR 			XPAR_PS7_UART_1_BASEADDR

//----------------------------------------------------
// PROTOTYPE FUNCTIONS
//----------------------------------------------------
static void BTN_Intr_Handler(void *baseaddr_p);
static int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr);
void DrawBackground(int *image_buffer_pointer);

// Button variables
XGpio BTNInst;
XScuGic INTCInst;
static int btn_value;

// Colors mask
unsigned int WHITE   = 0xFFFFFF;
unsigned int BLUE    = 0xFF0000;
unsigned int RED     = 0x0000FF;
unsigned int PURPLE  = 0xFF00FF;
unsigned int GREEN   = 0x00FF00;
unsigned int CYAN    = 0xFFFF00;
unsigned int COLOR_ARRAY[] = {BLUE, RED, PURPLE, CYAN, GREEN};

//int NUM_BYTES_BUFFER  = 5242880;

void BTN_Intr_Handler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&BTNInst, BTN_INT);
	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&BTNInst) & BTN_INT) !=
			BTN_INT) {
			return;
		}
	btn_value = XGpio_DiscreteRead(&BTNInst, 1);
	// Increment counter based on button value
	// Reset if center button pressed

    (void)XGpio_InterruptClear(&BTNInst, BTN_INT);
    // Enable GPIO interrupts
    XGpio_InterruptEnable(&BTNInst, BTN_INT);
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
	xil_printf(
			"Enter 'c' to generate pure block sprites AND '1' to generate 1 blocks\r\n");
	xil_printf("----------------------------------------\r\n");
	while (!XUartPs_IsReceiveData(UART_BASEADDR))
		;

	inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	// Select function based on UART input
	switch (inp) {
	case '1':
		xil_printf("GENERATING ONE BLOCKS\r\n");
		xil_printf("Press 'q' to go back\r\n");
		// Create 1 digits square box
		generate_one_blocks_ez(image_buffer_pointer);
		xil_printf("printing 1\r\n");
		break;
	case '2':
		xil_printf("GENERATING CLEAR BLOCKS\r\n");
		xil_printf("Press 'q' to go back\r\n");
		// Create white square box
		generate_clear_blocks_ez(image_buffer_pointer);
		xil_printf("Printing 2\r\n");
		break;
	default:
		xil_printf("Printing background\r\n");
		DrawBackground(image_buffer_pointer);
		xil_printf("Going to main\r\n");
		uart_input(image_buffer_pointer);
		xil_printf("Already in main!\r\n");
		break;
	} // switch
}

int main()
{
	int status;

	//----------------------------------------------------
	// INITIALIZE THE PERIPHERALS & SET DIRECTIONS OF GPIO
	//----------------------------------------------------
	// Initialize Push Buttons
	status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Set all buttons direction to inputs
	XGpio_SetDataDirection(&BTNInst, 1, 0xFF);

	// Initialize interrupt controller
	status = IntcInitFunction(INTC_DEVICE_ID, &BTNInst);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Initialize buffer that writes to VGA
	int *image_buffer_pointer = (int *)0x00900000;

	// Set background to vertical colored lines
	DrawBackground(image_buffer_pointer);

//	 FOR UART DEVELOPMENT-- from adventures_with_ip.c

	uart_input(image_buffer_pointer);
	return 0;
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

//----------------------------------------------------
// INITIAL SETUP FUNCTIONS
//----------------------------------------------------

int InterruptSystemSetup(XScuGic *XScuGicInstancePtr)
{
	// Enable interrupt
	XGpio_InterruptEnable(&BTNInst, BTN_INT);
	XGpio_InterruptGlobalEnable(&BTNInst);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 	 	 	 	 	 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
			 	 	 	 	 	 XScuGicInstancePtr);
	Xil_ExceptionEnable();

	Xil_DCacheDisable();

	return XST_SUCCESS;

}

int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr)
{
	XScuGic_Config *IntcConfig;
	int status;

	// Interrupt controller initialization
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Call to interrupt setup
	status = InterruptSystemSetup(&INTCInst);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(&INTCInst,
					  	  	 INTC_GPIO_INTERRUPT_ID,
					  	  	 (Xil_ExceptionHandler)BTN_Intr_Handler,
					  	  	 (void *)GpioInstancePtr);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// Enable GPIO and timer interrupts in the controller
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID);

	return XST_SUCCESS;
}


