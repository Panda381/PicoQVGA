
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "include.h"

// main function
int main()
{
	int x, x2, y;
	float phase;

	// draw sky and mast
	memcpy(FrameBuf, CloudsImg, FRAMESIZE);
	DrawBlit(MastImg, MASTX, HEIGHT - MASTH, MASTW, MASTH, MASTW, COL_WHITE);

#define WAVELEN	0.03f	// wave length
#define WAVESPEED 0.18f	// animation speed
#define WAVEAMP 0.1f	// wave amplitude

	// main loop
	phase = 0;
	while (true)
	{
		// draw flag
		for (x = 0; x < FLAGW; x++)
		{
			// calculate vertical coordinate of the flag
			y = (int)(FLAGY + WAVEAMP*sin(phase - x*WAVELEN)*x);
			x2 = x + FLAGX + MASTX;

			// draw flag
			DrawImg(FlagImg+x, x2, y, 1, FLAGH, FLAGW);

			// redraw sky
			if (x2 >= MASTX + MASTW) // to avoid overwrite top part of the mast
			{
				DrawImg(CloudsImg+x2, x2, 0, 1, y, WIDTH);
				DrawImg(CloudsImg+x2+(y+FLAGH)*WIDTH, x2, y + FLAGH, 1, HEIGHT - y - FLAGH, WIDTH);
			}
		}

		// increase phase
		phase += WAVESPEED;
		if (phase > PI*2) phase -= PI*2;
	}
}
