#ifndef __GAME_DESIGN_H_
#define __GAME_DESIGN_H_

#include <xbasic_types.h>
#include <xparameters.h>

#define RAND_BASEADDR  	XPAR_RANDOM_NUMBER_GENERA_0_S00_AXI_BASEADDR


/* ---------------------------------------------------------------------------- *
 * 						Prototype Functions								*
 * ---------------------------------------------------------------------------- */

int generateRandom();

#endif
