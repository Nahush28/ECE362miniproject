#include "fb.h"

// Each pair of color is assigned with SPRITEPX, from left to right. Max size is the size of fb.
// Eg. SPRITEPX(R, R), SPRITEPX(G, B), SPRITEPX(Y, K) gives a line of RRGBY (pad with K for single color)
// Minimum sprite size 2x2

const Sprite player = (Sprite) {
	.pxMap = {SPRITEPX(R, G), 
		SPRITEPX(B, Y)},
	.xSize = 2,
	.ySize = 2
};