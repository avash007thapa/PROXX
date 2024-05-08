#ifndef __GAME_DESIGN_H_
#define __GAME_DESIGN_H_

#include <xbasic_types.h>
#include <xparameters.h>

#define RAND_BASEADDR  	XPAR_RANDOM_NUMBER_GENERA_0_S00_AXI_BASEADDR

const int diff_EZ = 8; // 8 by 8 blocks

/* ---------------------------------------------------------------------------- *
 * 							Initialize Structs									*
 * ---------------------------------------------------------------------------- */
// Sprite structure
typedef struct {
    int index; // Calculate position according to numbered blocks
    unsigned int *color;
    bool mine; // Is this block a mine or not?
    bool flagged;
    bool revealed;
    int proxx; // This is the count of each block according to mines in their proximity
} Block;


/* ---------------------------------------------------------------------------- *
 * 						Prototype Functions								*
 * ---------------------------------------------------------------------------- */
void initializeBlockArray();
int generateRandom();
void assign_mine();
void update_blocks();
int showBlock(int xPos, int yPos);
void reveal_clear_block(int index);
void print_grid();
void assign_flag_block(int xPos, int yPos);
void iterate_over_flag(int index);
int check_flags_adj(int index, int adjacent_offsets[8], int local_flag_count);
int winCheck();

#endif
