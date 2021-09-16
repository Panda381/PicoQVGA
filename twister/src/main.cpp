
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "include.h"

#define MIDX	(WIDTH/2) // middle X coordinate

int main()
{
	float aa;
	float ang; // base angle
	float amp; // wave amplitude
	float yamp;
	int y, ys, x, x1, x2, x3, x4;

	// main loop
	aa = 0; // angle counter
	while (true)
	{
		ang = (sin(aa)-1)*PI;
		amp = sin(cos(aa)*0.3)*1000 + 350;

		// draw twister
		for (y = 0; y < HEIGHT; y++)
		{
			yamp = y/amp + ang;
			ys = y & (IMGHEIGHT-1);

			// 4 edges
			x1 = sin(yamp)*TWISTW + MIDX;
			x2 = sin(yamp+PI/2)*TWISTW + MIDX;
			x3 = sin(yamp+PI)*TWISTW + MIDX;
			x4 = sin(yamp+PI*3/2)*TWISTW + MIDX;

			// draw 4 segment on one line
			if (x2 - x1 > 1) DrawImgLine(CobbleStoneImg+ys*IMGWIDTH, x1, y, x2-x1, IMGWIDTH, IMGWIDTH);
			if (x3 - x2 > 1) DrawImgLine(FabricImg+ys*IMGWIDTH, x2, y, x3-x2, IMGWIDTH, IMGWIDTH);
			if (x4 - x3 > 1) DrawImgLine(SoilImg+ys*IMGWIDTH, x3, y, x4-x3, IMGWIDTH, IMGWIDTH);
			if (x1 - x4 > 1) DrawImgLine(WoodImg+ys*IMGWIDTH, x4, y, x1-x4, IMGWIDTH, IMGWIDTH);

			// clear left part of the screen
			x = x1;
			if (x2 < x) x = x2;
			if (x3 < x) x = x3;
			if (x4 < x) x = x4;
			DrawRect(0, y, x, 1, COL_BLACK);

			// clear right part of the screen
			x = x1;
			if (x2 > x) x = x2;
			if (x3 > x) x = x3;
			if (x4 > x) x = x4;
			DrawRect(x, y, WIDTH-x, 1, COL_BLACK);
		}

		// increment angle of animation
		aa += 0.02f;
	}
}
