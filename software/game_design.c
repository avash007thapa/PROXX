#include "game_design.h"

Xuint32 *baseaddr_rand = (Xuint32 *)RAND_BASEADDR;

//GIVES A RANDOM NUMBER BETWEEN 0 AND 255
int generateRandom() {
//	xil_printf("Randomization Test\n\r");

	*(baseaddr_rand + 0) = 0x40000000;
//	xil_printf("New random number: \n\r", *(baseaddr_p + 0));

	int tmp;
	tmp = (int)* (baseaddr_rand + 1);
	return tmp;
}

