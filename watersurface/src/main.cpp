
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "include.h"

int main()
{
	float phase;
	int y, y0, y2, duckx;

	// initialize sound output
	PWMSndInit();

	// play sound
	PlaySound(RiverSnd, sizeof(RiverSnd), True);

	// display top part of the screen
	memcpy(FrameBuf, ForestImg, sizeof(ForestImg));

	// main loop
	phase = 0;
	duckx = 330;
	while (true)
	{
		// to avoid duck flickering
		WaitVSync();

		// animate water
		for (y = IMGH; y < HEIGHT; y++)
		{

#define WAVEAMP		0.1f	// wave amplitude
#define WAVEPERSP	20.0f	// perspective deformation
#define WAVELEN		100	// wave length
#define ANIMSPEED	0.4f	// animation speed

			y0 = y - IMGH; // Y relative to top of water
			y2 = (int)(IMGH - 1 - (sinf(phase + WAVELEN/sqrtf(y0/WAVEPERSP + 1))*WAVEAMP + 1)*y0);
			if (y2 < 0) y2 = 0;
			if (y2 >= IMGH) y2 = IMGH-1;
			memcpy(&FrameBuf[y*WIDTH], &ForestImg[y2*WIDTH], WIDTH);
		}

		// display duck
		DrawBlit(DuckImg+(duckx & 3)*64, duckx, 180, 64, 29, 256, 0x86);		

		// increase animation phase
		phase += ANIMSPEED;

		// shift duck
		duckx--;
		if (duckx < -100) duckx = 330;

		// delay
		sleep_ms(50);
	}
}
