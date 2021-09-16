
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "include.h"

// draw back buffer
ALIGNED u8 BackBuf[FRAMESIZE];

// transformation matrix
cMat2Df Mat;

// setup rotation transformation matrix
void SetMat(float a)
{
	// prepare transformation matrix
	// Image of real dimensions WIDTH * HEIGHT will be enlarged to square 400 * 400
	Mat.PrepDrawImg(WIDTH, HEIGHT, 0, 0, 400, 400, 0, 0, a, WIDTH/2, HEIGHT/2);
}

// main function
int main()
{
	int x, y, k;
	float r, a, y2;
	u8* d;

	// setup transformation matrix
	SetMat(0);

	// generate hypno image
	d = BackBuf;
	for (y = -HEIGHT/2; y < HEIGHT/2; y++)
	{
		for (x = -WIDTH/2; x < WIDTH/2; x++)
		{
			// radius (y is converted to ensure square image)
			y2 = (float)y*WIDTH/HEIGHT;
			r = sqrtf(x*x + y2*y2);

			// angle
			a = atan2(y2, x);

			// intensity (+ small raster dithering)
			k = (int)(127*sinf(r*0.17f + a) + 127);

			// store pixel, on red color (bits 5..7)
			*d++ = k & 0xe0;
		}
	}

	// main loop
	a = 0;
	while (true)
	{
		SetMat(a);
		a -= 0.05f;
		if (a < 0) a += PI*2;

		// draw image with transformation
		DrawImgMat(BackBuf, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, &Mat, DRAWIMG_CLAMP, 0); 
	}
}
