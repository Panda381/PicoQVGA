
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "include.h"

// display back buffer
ALIGNED u8 BackBuf[FRAMESIZE];

// hot-air balloons
int hotx[HOTAIR_NUM];
int hoty[HOTAIR_NUM];
s8 hotdx[HOTAIR_NUM];
s8 hotdy[HOTAIR_NUM];

// party balloons
const u8* balimg_src[4] = { BlueImg, GreenImg, RedImg, YellowImg };
const u8* balimg[BALLOON_NUM];
int balx[BALLOON_NUM];
int baly[BALLOON_NUM];
int baldy[BALLOON_NUM];

// main function
int main()
{
	int i, x, y;

	// current sky animation
	int skyx = 0;
	int skyy = 0;

	// hot-air balloons coordinates
	for (i = 0; i < HOTAIR_NUM; i++)
	{
		hotx[i] = RandU16Max(WIDTH-HOTAIRW);
		hoty[i] = RandU16Max(HEIGHT-HOTAIRH);
		hotdx[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
		if (RandU8() < 0x80) hotdx[i] = -hotdx[i];
		hotdy[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
		if (RandU8() < 0x80) hotdy[i] = -hotdy[i];
	}

	// prepare party balloons
	for (i = 0; i < BALLOON_NUM; i++)
	{
		balimg[i] = balimg_src[RandU8Max(3)];
		balx[i] = RandU16Max(WIDTH-BALLOONW);
		baly[i] = RandS16MinMax(-BALLOONH, HEIGHT-1);
		baldy[i] = RandU8MinMax(BALLOON_SPEED-1, BALLOON_SPEED+1);
	}

	// set pointer to current frame buffer
	pDrawBuf = BackBuf; // back buffer
	bool buf = 0; // current front buffer

	// main loop
	while (true)
	{
		// animate sky
		DrawImg(CloudsImg, skyx - CLOUDSW, skyy - CLOUDSH, CLOUDSW, CLOUDSH, CLOUDSW);
		DrawImg(CloudsImg, skyx, skyy - CLOUDSH, CLOUDSW, CLOUDSH, CLOUDSW);
		DrawImg(CloudsImg, skyx - CLOUDSW, skyy, CLOUDSW, CLOUDSH, CLOUDSW);
		DrawImg(CloudsImg, skyx, skyy, CLOUDSW, CLOUDSH, CLOUDSW);
		skyx += 4;
		if (skyx >= CLOUDSW) skyx -= CLOUDSW;
		skyy += 1;
		if (skyy >= CLOUDSH) skyy -= CLOUDSH;

		// animate hot-air balloons
		for (i = 0; i < HOTAIR_NUM; i++)
		{
			x = hotx[i] + hotdx[i];
			if (x < 0)
			{
				x = 0;
				hotdx[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}

			if (x > WIDTH - HOTAIRW)
			{			
				x = WIDTH - HOTAIRW;
				hotdx[i] = -RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}
			hotx[i] = x;

			y = hoty[i] + hotdy[i];
			if (y < 0)
			{
				y = 0;
				hotdy[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}

			if (y > HEIGHT - HOTAIRH)
			{			
				y = HEIGHT - HOTAIRH;
				hotdy[i] = -RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}
			hoty[i] = y;

			DrawBlit(HotairImg, hotx[i], hoty[i], HOTAIRW, HOTAIRH, HOTAIRW, COL_WHITE);
		}

		// animate party balloons
		for (i = 0; i < BALLOON_NUM; i++)
		{
			y = baly[i] - baldy[i];
			if (y < -BALLOONH) y += HEIGHT+BALLOONH;
			baly[i] = y;

			DrawBlit(balimg[i], balx[i], baly[i], BALLOONW, BALLOONH, BALLOONW, COL_WHITE);
		}

		// flip buffers
		buf ^= 1;
		if (buf == 0)
		{
			pFrameBuf = FrameBuf; // front buffer
			pDrawBuf = BackBuf; // back buffer
		}
		else
		{
			pFrameBuf = BackBuf; // front buffer
			pDrawBuf = FrameBuf; // back buffer
		}

		// delay
		sleep_ms(20);
	}
}
