
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "include.h"

int main()
{
	Bool slow = True;
	u32 t, t2;

	// main loop
	t = time_us_32();
	while (true)
	{
		switch (RandU8Max(7))
		{
		case 0:
			DrawRect(
				RandS16MinMax(-50, WIDTH),		// x
				RandS16MinMax(-50, HEIGHT),		// y
				RandU16MinMax(1, 100),		// w
				RandU16MinMax(1, 100),		// h
				RandU8());				// col
			break;

		case 1:
			DrawFrame(
				RandS16MinMax(-50, WIDTH),		// x
				RandS16MinMax(-50, HEIGHT),		// y
				RandU16MinMax(4, 100),		// w
				RandU16MinMax(4, 100),		// h
				RandU8());				// col
			break;

		case 2:
			DrawPoint(
				RandS16MinMax(-50, WIDTH+50), 	// x
				RandS16MinMax(-50, HEIGHT+50),	// y
				RandU8());				// col
			break;

		case 3:
			DrawLine(
				RandS16MinMax(-50, WIDTH+50),	// x1
				RandS16MinMax(-50, HEIGHT+50),	// y1
				RandS16MinMax(-50, WIDTH+50),	// x2
				RandS16MinMax(-50, HEIGHT+50),	// y2
				RandU8());				// col
			break;

		case 4:
			DrawFillCircle(
				RandS16MinMax(-50, WIDTH+50),	// x0
				RandS16MinMax(-50, HEIGHT+50),	// y0
				RandU16MinMax(1, 50),		// r
				RandU8());				// col
			break;

		case 5:
			DrawCircle(
				RandS16MinMax(-50, WIDTH+50),	// x0
				RandS16MinMax(-50, HEIGHT+50),	// y0
				RandU16MinMax(1, 50),		// r
				RandU8());				// col
			break;

		case 6:
			if (RandU8Max(1)==0)
				DrawText(
					"Hello!",			// text
					RandS16MinMax(-50, WIDTH+50), // x
					RandS16MinMax(-50, HEIGHT+50), // y
					RandU8());			// col
			else
				DrawTextBg(
					"Hello!",			// text
					RandS16MinMax(-50, WIDTH+50), // x
					RandS16MinMax(-50, HEIGHT+50), // y
					RandU8(),			// col
					RandU8());			// bgcol
			break;

		case 7:
			if (RandU8Max(1)==0)
				DrawImg(
					Peter8Img,			// src
					RandS16MinMax(-50, WIDTH-10), // xd
					RandS16MinMax(-50, HEIGHT-10), // yd
					32,				// w
					32,				// h
					32);				// ws
			else
				DrawBlit(
					RPi8Img,			// src
					RandS16MinMax(-50, WIDTH-10), // xd
					RandS16MinMax(-50, HEIGHT-10), // yd
					32,				// w
					40,				// h
					32,				// ws
					COL_WHITE);			// col
			break;
		}

		// change mode after some time
		t2 = time_us_32();
		if (slow)
		{
			if ((t2 - t) > 8000000)
			{
				slow = False;
				t = t2;
			}
			sleep_ms(50);
		}
		else
		{
			if ((t2 - t) > 2000000)
			{
				slow = True;
				t = t2;
			}
		}
	}
}
