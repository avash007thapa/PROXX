#ifndef SPRITE_DEV_H_
#define SPRITE_DEV_H_

/* ---------------------------------------------------------------------------- *
 * 								Header Files									*
 * ---------------------------------------------------------------------------- */
#include <stdio.h>
#include "xil_types.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"
#include <cstdlib>

/* ---------------------------------------------------------------------------- *
 * 							Initialize Structs									*
 * ---------------------------------------------------------------------------- */
// Sprite structure
typedef struct {
    int x, y; // Position
    unsigned int color; // Color
} Sprite;

/* ---------------------------------------------------------------------------- *
 * 							Prototype Functions									*
 * ---------------------------------------------------------------------------- */
void DrawSprite(int *image_buffer_pointer, Sprite sprite);
void generate_clear_blocks_ez(int *image_buffer_pointer);
void generate_one_blocks_ez(int *image_buffer_pointer);

/* ---------------------------------------------------------------------------- *
 * 							Screen Information									*
 * ---------------------------------------------------------------------------- */
// Screen width / height
const int WIDTH 	  = 1280;
const int HEIGHT 	  = 1024;


#endif /* SPRITE_DEV_H_ */
