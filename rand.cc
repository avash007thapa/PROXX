#include <stdio.h>
#include <xil_io.h>
#include <xil_printf.h>
#include <xparameters.h>
#include <xbasic_types.h>
#include <unistd.h>
#include "xuartps.h"
#include "stdlib.h"

#define RAND_BASEADDR  	XPAR_RANDOM_NUMBER_GENERA_0_S00_AXI_BASEADDR
#define UART_BASEADDR 	XPAR_PS7_UART_1_BASEADDR

void uart_input();
void generateRandom();

Xuint32 *baseaddr_p = (Xuint32 *)RAND_BASEADDR;

int main()
{
	uart_input();
	return 0;
}

void uart_input() {
	//	 FOR UART DEVELOPMENT-- from adventures_with_ip.c
	u8 inp = 0x00;
	u32 CntrlRegister;
	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);
	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
			((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) | XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));
	xil_printf("\r\n\r\n");
	xil_printf("Embedded Sprite development Demo\r\n");
	xil_printf("Press enter to generate new random number\r");
	xil_printf("----------------------------------------\r\n");

	while (!XUartPs_IsReceiveData(UART_BASEADDR));

	inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	// Select function based on UART input
	switch(inp) {
	case '\r':
		generateRandom();
		break;
	default:
		uart_input();
		break;
	} // switch
}

void generateRandom() {
//	xil_printf("Randomization Test\n\r");

	*(baseaddr_p + 0) = 0x40000000;
//	xil_printf("New random number: \n\r", *(baseaddr_p + 0));

	xil_printf("%d ", (int)*(baseaddr_p + 1));

	while (!XUartPs_IsReceiveData(UART_BASEADDR));
//	xil_printf("Press enter to generate new random number\r");
	if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == '\r'){
		generateRandom();
	} else {
		uart_input();
	}
}
