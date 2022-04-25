#include "fb.h"

// Each pair of color is assigned with SPRITEPX, from left to right. Max size is the size of fb.
// Eg. SPRITEPX(R, R), SPRITEPX(G, B), SPRITEPX(Y, K) gives a line of RRGBY (pad with K for single color)

const Sprite player = (Sprite) {
	.pxMap = {SPRITEPX(W, G), SPRITEPX(W,
		   G), SPRITEPX(C, G),
		   SPRITEPX(W, G), SPRITEPX(W, K)},
	.xSize = 3,
	.ySize = 3
};

const Sprite obstacle = (Sprite) {
	.pxMap = {SPRITEPX(Y, Y),
		  SPRITEPX(Y, Y),
		  SPRITEPX(Y, Y),
		  SPRITEPX(Y, Y)},
	.xSize = 2;
	.ySize = 4;
};
