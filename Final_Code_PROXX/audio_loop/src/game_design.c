#include "game_design.h"
#include "color_codes.h"
#include "sprite_dev.h"
#include <unistd.h>
// Initialize base address for random number generator
Xuint32 *baseaddr_rand = (Xuint32 *)RAND_BASEADDR;

const int ez_mine_configuration = 10;
Block blockArray[64];
int mine_indices[ez_mine_configuration];

void initializeBlockArray() {
	int index = 0;
	for (int j = 0; j < diff_EZ; ++j) {
    	for (int i = 0; i <= diff_EZ - 1; ++i) {
    		// Initialize each struct in the array
    		blockArray[index].index = index;
    		blockArray[index].color = state[0];
   			blockArray[index].mine = FALSE;
   			blockArray[index].flagged = FALSE;
   			blockArray[index].proxx = 0;
   			blockArray[index].revealed = FALSE;

   			DrawSprite(blockArray[index]);
   			index++;
    	}
    }

	for (int j = 0; j <= ez_mine_configuration; j++){
		mine_indices[j] = 255;
	}
}

//GIVES A RANDOM NUMBER BETWEEN 0 AND 255
int generateRandom() {
//	xil_printf("Randomization Test\n\r");

	*(baseaddr_rand + 0) = 0x40000000;
//	xil_printf("New random number: \n\r", *(baseaddr_p + 0));

	int tmp;
	tmp = (int)* (baseaddr_rand + 1);
	return tmp;
}

// assigns number around mine after random generation
void assign_mine() {
	int temp_index = abs((generateRandom() / 4) - 1);

	bool restart = true;
	while (restart) {

		restart = false; // Reset restart flag
	    for (int i = 0; i <= ez_mine_configuration; i++) {
	    	if (temp_index == mine_indices[i]) {

	    		temp_index = abs((generateRandom() / 4) - 1);
	        	restart = true;
	            break;       // Break out of the inner loop
	    	} else if (mine_indices[i] == 255){

	    		mine_indices[i] = temp_index;
	    		break;
	    	}
	    }
	}

	xil_printf("MINE INDEX saved %d %d %d %d %d %d %d %d %d %d\r\n",
			mine_indices[0], mine_indices[1], mine_indices[2], mine_indices[3], mine_indices[4],
			mine_indices[5], mine_indices[6], mine_indices[7], mine_indices[8], mine_indices[9]);


	blockArray[temp_index].index = temp_index;
	blockArray[temp_index].color = state[9];
	blockArray[temp_index].mine = TRUE;

	// assign numbers to tiles to the right side of mine
	if (temp_index % diff_EZ != 7) {
		//ABLE TO increase right block count
		blockArray[temp_index + 1].proxx++;

		// increase up-right proxx count by 1 if mine is in second row and below
		if(temp_index >= diff_EZ){
			blockArray[temp_index - diff_EZ + 1].proxx++; // up -> right

		}

		// increase down-right proxx count by 1 if mine is in second last row and above
		if(temp_index < (diff_EZ*diff_EZ) - diff_EZ - 1){
			// increase down-right block count by 1
			blockArray[temp_index + diff_EZ + 1].proxx++;
		}
	}

	// assign numbers to tiles to left side of mine
	if (temp_index % diff_EZ != 0) {
		//ABLE TO increase left block count
		blockArray[temp_index - 1].proxx++;

		// increase up-left proxx count by 1 if mine is in second row and below
		if(temp_index > diff_EZ){
			blockArray[temp_index - diff_EZ - 1].proxx++; // up -> left
		}

		// increase down-left proxx count by 1 if mine is in second last row and above
		if(temp_index < (diff_EZ*diff_EZ)- diff_EZ ){
			// increase down-LEFT block count by 1
			blockArray[temp_index + diff_EZ - 1].proxx++; // down -> left
		}
	}

	if(temp_index >= diff_EZ)
			blockArray[temp_index - diff_EZ].proxx++; 	// up

	if(temp_index < (diff_EZ*diff_EZ)- diff_EZ)
			blockArray[temp_index + diff_EZ].proxx++;	//down

	update_blocks();
}

// assigns proximity values to blocks around mine after random generation
void update_blocks() {

	// Draw 0-63 indexed sprites- block by block at given position
	for (int ii = 0; ii < (diff_EZ*diff_EZ)-1; ii++) {
		if (!blockArray[ii].mine) {
			if(blockArray[ii].proxx != 0) {
				blockArray[ii].color = state[blockArray[ii].proxx];
			}
			else{
				blockArray[ii].color = state[10];
			}
		}
	}
}

void print_grid() { //print entire grid revealed at end of game
	for (int ii = 0; ii < (diff_EZ*diff_EZ); ii++) {
		if (!blockArray[ii].mine) {
			if(blockArray[ii].proxx != 0) {
				blockArray[ii].color = state[blockArray[ii].proxx];
				DrawNumbers(blockArray[ii]);
			}
			else{
				blockArray[ii].color = state[10];
				DrawSprite(blockArray[ii]);
			}
		} else {
			DrawSprite(blockArray[ii]);
		}
	}

}

int winCheck() { //check for win conditions met
	int mineCount = 0;
	int mineCountf = 0;
	int notR = 0;
	for (int ii = 0; ii < (diff_EZ*diff_EZ); ii++) {
		if(blockArray[ii].mine && !blockArray[ii].revealed) { //check for mines not revealed
			mineCount++;
		} else if(blockArray[ii].mine && blockArray[ii].flagged) { //check for mines flagged
			mineCountf++;
		} else if(!blockArray[ii].revealed) { //check for blocks still to be revealed
			notR++;
		}
	}
	if(((mineCount == 10) || (mineCountf == 10)) && (notR == 0)) {
		return 2;
	}

	return 0;
}

int showBlock(int xPos, int yPos) { //check whether or not to reveal the block
	int i = (xPos - ((WIDTH - 360) / 2) + (SQUARE_SIDE / 2))/(WIDTH / 32);
    int j = (yPos - ((HEIGHT - 292) / 2) + (SQUARE_SIDE / 2))/(HEIGHT / 24);
    int index = j * diff_EZ + i - 1;

    int won = winCheck(); //check win condition
    if(won==2) {
    	return 2;
    }
	if (!blockArray[index].flagged && !blockArray[index].revealed) {

		if((xPos >= 484) && (xPos <= 804) && (yPos >= 350) &&(yPos <= 694) ) {
			xil_printf("proxx value is: %d\n", blockArray[index].proxx);

			// TRIPPED MINE- GAME OVER
			if(blockArray[index].mine) {
				xil_printf("HIT A MINE\n");
				return 1;
			}

			// CLEAR BLOCK
			else if(blockArray[index].proxx==0) {
				xil_printf("\n\nREVEALING CLEAR BLOCK \n\n");
				reveal_clear_block(index);
				return 0;
			}

			// PRINT DIGIT BLOCK
			else {
				blockArray[index].revealed = TRUE;
				blockArray[index].color = state[blockArray[index].proxx];
				DrawNumbers(blockArray[index]);
				xil_printf("Did not hit a mine.\n");
				return 0;
			}
		}
	} else if (blockArray[index].flagged && blockArray[index].revealed && !blockArray[index].mine) {
		// Array of offsets to represent adjacent blocks indices: {-9, -8, -7, -1, 1, 7, 8, 9}
		int adjacent_offsets[8] = {-diff_EZ - 1, -diff_EZ, -diff_EZ + 1,
									-1,                     		1,
									diff_EZ - 1,  diff_EZ,  diff_EZ + 1};

		// Iterate over the adjacent blocks
		for (int j = 0; j < 8; j++) {

			// no need to check left blocks if flagged blocks is left-most block
		    if(index%diff_EZ == 0)
		    	if (j == 0 || j == 3 || j == 5)
		    		continue;

		    // no need to check right blocks if flagged blocks is right-most block
		    if(index%diff_EZ == 1)
		    	if(j == 2 || j == 4 || j == 7)
		    		continue;

		    int new_index = index + adjacent_offsets[j];

		    // Check if the new index is within the grid bounds
		    if (new_index >= 0 && new_index < diff_EZ * diff_EZ) {
		    	// TRIPPED MINE- GAME OVER
		    	if(blockArray[index].mine) {
		    		xil_printf("Hit a mine.\n");
		    		return 1;
		    	}
		    }

		    // CLEAR BLOCK
		    else if(blockArray[index].proxx==0) {
		    	xil_printf("\n\nREVEALING CLEAR BLOCK \n\n");
		    	reveal_clear_block(index);
		    	return 0;
		    }

		    // PRINT DIGIT BLOCK
			else {
				blockArray[index].revealed = TRUE;
				blockArray[index].color = state[blockArray[index].proxx];
				DrawNumbers(blockArray[index]);

		    	xil_printf("Did not hit a mine.\n");
		    	return 0;
		    }
		}
	}
	return 0;
}


void assign_flag_block(int xPos, int yPos) {
    // Calculate the index from xPos and yPos
    int index = ((yPos - ((HEIGHT - 292) / 2) + (SQUARE_SIDE / 2)) / (HEIGHT / 24)) * diff_EZ +
                ((xPos - ((WIDTH - 360) / 2) + (SQUARE_SIDE / 2)) / (WIDTH / 32)) - 1;

    if (index >= 0 && index < diff_EZ * diff_EZ && !blockArray[index].revealed) {
        // Toggle the flag status of the block
        blockArray[index].flagged = !blockArray[index].flagged;
        unsigned int* temp_state = blockArray[index].color;

        // make sure flag toggle shows clear block
        blockArray[index].color = state[0];
        // draw the flag or sprite based on the flag status
        if (blockArray[index].flagged) {
            DrawFlagged(blockArray[index]);
        } else {
            DrawSprite(blockArray[index]);
        }

        // Put value back to what it was
        blockArray[index].color = temp_state;


        // Initialize a local flag count for the current block
		iterate_over_flag(index);
    }
}

void iterate_over_flag(int index) {
	// Initialize a local flag count for the current block
	int local_flag_count = 1;
	// Array of offsets to represent adjacent blocks indices: {-9, -8, -7, -1, 1, 7, 8, 9}
	int adjacent_offsets[8] = { -diff_EZ - 1, -diff_EZ, -diff_EZ + 1,
								-1, 							1,
								diff_EZ - 1, diff_EZ, diff_EZ + 1 };


	// Iterate once over the adjacent blocks
	local_flag_count = check_flags_adj(index, adjacent_offsets, local_flag_count);


	// Iterate over the adjacent blocks again
	for (int j = 0; j < 8; j++) {
		// no need to check left blocks if flagged blocks is left-most block
		if (index % diff_EZ == 0)
			if (j == 0 || j == 3 || j == 5)
				continue;

		// no need to check right blocks if flagged blocks is right-most block
		if (index % diff_EZ == 1)
			if (j == 2 || j == 4 || j == 7)
				continue;

		int new_index = index + adjacent_offsets[j];
		// Check if the new index is within the grid bounds
		if (new_index >= 0 && new_index < diff_EZ * diff_EZ) {
			// Check if the adjacent block is revealed and if its proxx value matches the flag count
			if (blockArray[new_index].revealed && blockArray[new_index].proxx == local_flag_count) {
				if (!blockArray[new_index].flagged) {
					// Flag the revealed neighbor if its proxx value matches the flag count
					blockArray[new_index].flagged = true;
					DrawFlagged(blockArray[new_index]);
				} else {
					// If toggled, print the regular digit
					blockArray[new_index].flagged = false;
					DrawNumbers(blockArray[new_index]);
				}
			}
		}
	}
}

int check_flags_adj(int index, int adjacent_offsets[8], int local_flag_count) {
	// Iterate over the adjacent blocks
	for (int j = 0; j < 8; j++) {
		// no need to check left blocks if flagged blocks is left-most block
		if (index % diff_EZ == 0)
			if (j == 0 || j == 3 || j == 5)
				continue;

		// no need to check right blocks if flagged blocks is right-most block
		if (index % diff_EZ == 1)
			if (j == 2 || j == 4 || j == 7)
				continue;

		int new_index = index + adjacent_offsets[j];
		// Check if the new index is within the grid bounds
		if (new_index >= 0 && new_index < diff_EZ * diff_EZ) {
			// Check if the adjacent block is flagged
			if (blockArray[new_index].flagged) {
				local_flag_count++;
//				iterate_over_flag(new_index);
			}
		}
	}
	return local_flag_count;
}

void reveal_clear_block(int index) {
    // Check if the index is valid and if the block is not revealed yet
    if (index >= 0 && index < diff_EZ * diff_EZ && !blockArray[index].revealed) {
        // Mark the block as revealed
        blockArray[index].revealed = true;

        // If the block has no adjacent mines (proxx == 0), recursively reveal its neighbors
        if (blockArray[index].proxx == 0) {
        	// Draw the block
        	DrawSprite(blockArray[index]);

			if (blockArray[index].index==63) {
				// index 63 not printing
				xil_printf("MINE INDEX %d \nMINE: %d \n proximity to mine %d \r\n\n",
				blockArray[index].index, blockArray[index].mine, blockArray[index].proxx);
				DrawSprite(blockArray[index]);
			}

            // Define indices of adjacent blocks
            int row = index / diff_EZ;
            int col = index % diff_EZ;

            // Array of offsets to represent adjacent blocks: {row_offset, col_offset}
            int adjacent_offsets[8][2] = {
                {-1, -1}, {-1, 0}, {-1, 1},
                {0, -1},            {0, 1},
                {1, -1},  {1, 0},  {1, 1}
            };

            // Recursively call reveal_clear_block for adjacent blocks that are within the grid and unrevealed
            for (int i = 0; i < 8; i++) {
                int new_row = row + adjacent_offsets[i][0];
                int new_col = col + adjacent_offsets[i][1];

                // Check if the new indices are within the grid bounds
                if (new_row >= 0 && new_row < diff_EZ && new_col >= 0 && new_col < diff_EZ) {
                    int new_index = new_row * diff_EZ + new_col;

                    // Check if the adjacent block is unrevealed
                    if (!blockArray[new_index].revealed) {
                        reveal_clear_block(new_index);
                    }
                }
            }
        } else {
        	// If the block has adjacent mines, just reveal it without recursion
        	DrawNumbers(blockArray[index]);
        }
    }
}

