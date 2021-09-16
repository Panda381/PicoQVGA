
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "include.h"

int main()
{
	int y;

	// draw rainbow gradient
	for (y = 0; y < HEIGHT; y++) GenGrad(&FrameBuf[y*WIDTH], WIDTH);

	// draw text
	y = 110;
#define DX 24
	DrawRect(0, y, WIDTH, 16, COL_BLACK);
	y += 4;
	DrawText("Red", DX*1, y, COL_RED);
	DrawText("Yellow", DX*3-10, y, COL_YELLOW);
	DrawText("Green", DX*5-4, y, COL_GREEN);
	DrawText("Cyan", DX*7, y, COL_CYAN);
	DrawText("Blue", DX*9+3, y, COL_BLUE);
	DrawText("Magenta", DX*11-6, y, COL_MAGENTA);

	// gradient
	memcpy(&FrameBuf[126*WIDTH], GradientImg, sizeof(GradientImg));

	while (true) {}
}
