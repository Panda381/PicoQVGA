
// ****************************************************************************
//
//                                   Draw
//
// ****************************************************************************

#include "include.h"

// pointer to draw frame buffer
u8* pDrawBuf = FrameBuf;

// Draw rectangle
void DrawRect(int x, int y, int w, int h, u8 col)
{
	// limit x
	if (x < 0)
	{
		w += x;
		x = 0;
	}

	// limit w
	if (x + w > WIDTH) w = WIDTH - x;
	if (w <= 0) return;

	// limit y
	if (y < 0)
	{
		h += y;
		y = 0;
	}

	// limit h
	if (y + h > HEIGHT) h = HEIGHT - y;
	if (h <= 0) return;

	// draw
	u8* d = &pDrawBuf[x + y*WIDTH];
	int wb = WIDTH - w;
	int i;
	for (; h > 0; h--)
	{
		for (i = w; i > 0; i--) *d++ = col;
		d += wb;
	}
}

// Draw frame
void DrawFrame(int x, int y, int w, int h, u8 col)
{
	if ((w <= 0) || (h <= 0)) return;
	DrawRect(x, y, w-1, 1, col);
	DrawRect(x+w-1, y, 1, h-1, col);
	DrawRect(x+1, y+h-1, w-1, 1, col);
	DrawRect(x, y+1, 1, h-1, col);
}

// clear canvas (fill with black color)
void DrawClear()
{
	memset(pDrawBuf, COL_BLACK, FRAMESIZE);
}

// Draw point
void DrawPoint(int x, int y, u8 col)
{
	// check coordinates
	if (((u32)x >= (u32)WIDTH) || ((u32)y >= (u32)HEIGHT)) return;

	// draw pixel
	pDrawBuf[x + y*WIDTH] = col;
}

// Draw line
void DrawLine(int x1, int y1, int x2, int y2, u8 col)
{
	// difference of coordinates
	int dx = x2 - x1;
	int dy = y2 - y1;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
	}

	// increment Y
	int sy = 1;
	int ddy = WIDTH;
	if (dy < 0)
	{
		sy = -1;
		ddy = -ddy;
		dy = -dy;
	}

	// destination address
	u8* d = &pDrawBuf[x1 + y1*WIDTH];

	// steeply in X direction, X is prefered as base
	if (dx > dy)
	{
		int m = 2*dy;
		int p = m - dx;
		dx = 2*dx;

		// slow mode, check coordinates
		if (((u32)x1 >= (u32)WIDTH) || ((u32)x2 >= (u32)WIDTH) ||
			((u32)y1 >= (u32)HEIGHT) || ((u32)y2 >= (u32)HEIGHT))
		{
			x2 += sx;
			for (; x1 != x2; x1 += sx)
			{
				if (((u32)x1 < (u32)WIDTH) && ((u32)y1 < (u32)HEIGHT)) *d = col;
				if (p > 0)
				{
					d += ddy;
					y1 += sy;;
					p -= dx;
				}
				p += m;
				d += sx;
			}
		}

		// fast mode, do not check coordinates
		else
		{
			x2 += sx;
			for (; x1 != x2; x1 += sx)
			{
				*d = col;
				if (p > 0)
				{
					d += ddy;
					p -= dx;
				}
				p += m;
				d += sx;
			}
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		int m = 2*dx;
		int p = m - dy;
		dy = 2*dy;

		// slow mode, check coordinates
		if (((u32)x1 >= (u32)WIDTH) || ((u32)x2 >= (u32)WIDTH) ||
			((u32)y1 >= (u32)HEIGHT) || ((u32)y2 >= (u32)HEIGHT))
		{
			y2 += sy;
			for (; y1 != y2; y1 += sy)
			{
				if (((u32)x1 < (u32)WIDTH) && ((u32)y1 < (u32)HEIGHT)) *d = col;
				if (p > 0)
				{
					d += sx;
					x1 += sx;
					p -= dy;
				}
				p += m;
				d += ddy;
			}
		}

		// fast mode, do not check coordinates
		else
		{
			y2 += sy;
			for (; y1 != y2; y1 += sy)
			{
				*d = col;
				if (p > 0)
				{
					d += sx;
					p -= dy;
				}
				p += m;
				d += ddy;
			}
		}
	}
}

// Draw filled circle
void DrawFillCircle(int x0, int y0, int r, u8 col)
{
	int x, y;
	if (r <= 0) return;
	int r2 = r*(r-1);
	r--;

	// full circle
	for (y = -r; y <= r; y++)
	{
		for (x = -r; x <= r; x++)
		{
			if ((x*x + y*y) <= r2) DrawPoint(x+x0, y+y0, col);
		}
	}
}

// Draw circle
void DrawCircle(int x0, int y0, int r, u8 col)
{
	int x, y;
	if (r <= 0) return;
	r--;

	x = 0;
	y = r;
	int p = 1 - r;

	while (x <= y)
	{
		DrawPoint(x0+y, y0-x, col);
		DrawPoint(x0+x, y0-y, col);
		DrawPoint(x0-x, y0-y, col);
		DrawPoint(x0-y, y0-x, col);
		DrawPoint(x0-y, y0+x, col);
		DrawPoint(x0-x, y0+y, col);
		DrawPoint(x0+x, y0+y, col);
		DrawPoint(x0+y, y0+x, col);

		x++;
		if (p > 0)
		{
			y--;
			p -= 2*y;
		}
		p += 2*x + 1;
	}
}

// Draw text (transparent background)
void DrawText(const char* text, int x, int y, u8 col)
{
	// prepare
	int x0 = x;
	int y0 = y;
	u8 ch;
	int i, j;
	const u8* s;

	// loop through characters of text
	for (;;)
	{
		// get next character of the text
		ch = (u8)*text++;
		if (ch == 0) break;

		// prepare pointer to font sample
		s = &Font8x8[ch];

		// loop through lines of one character
		y = y0;
		for (i = 8; i > 0; i--)
		{
			// get one font sample
			ch = *s;
			s += 256;

			// loop through pixels of one character line
			x = x0;
			for (j = 8; j > 0; j--)
			{
				// pixel is set
				if ((ch & 0x80) != 0) DrawPoint(x, y, col);
				x++;
				ch <<= 1;
			}
			y++;
		}

		// shift to next character position
		x0 += 8;
	}
}

// Draw text double sized (transparent background)
void DrawText2(const char* text, int x, int y, u8 col)
{
	// prepare
	int x0 = x;
	int y0 = y;
	u8 ch;
	int i, j;
	const u8* s;

	// loop through characters of text
	for (;;)
	{
		// get next character of the text
		ch = (u8)*text++;
		if (ch == 0) break;

		// prepare pointer to font sample
		s = &Font8x8[ch];

		// loop through lines of one character
		y = y0;
		for (i = 8; i > 0; i--)
		{
			// get one font sample
			ch = *s;
			s += 256;

			// loop through pixels of one character line
			x = x0;
			for (j = 8; j > 0; j--)
			{
				// pixel is set
				if ((ch & 0x80) != 0)
				{
					DrawPoint(x, y, col);
					DrawPoint(x+1, y, col);
					DrawPoint(x, y+1, col);
					DrawPoint(x+1, y+1, col);
				}
				x += 2;
				ch <<= 1;
			}
			y += 2;
		}

		// shift to next character position
		x0 += 16;
	}
}

// Draw text with background
void DrawTextBg(const char* text, int x, int y, u8 col, u8 bgcol)
{
	// prepre
	int x0 = x;
	int y0 = y;
	u8 ch, c;
	int i, j;
	const u8* s;

	// loop through characters of text
	for (;;)
	{
		// get next character of the text
		ch = (u8)*text++;
		if (ch == 0) break;

		// prepare pointer to font sample
		s = &Font8x8[ch];

		// loop through lines of one character
		y = y0;
		for (i = 8; i > 0; i--)
		{
			// get one font sample
			ch = *s;
			s += 256;

			// loop through pixels of one character line
			x = x0;
			for (j = 8; j > 0; j--)
			{
				// pixel is set
				c = ((ch & 0x80) != 0) ? col : bgcol;

				// draw pixel
				DrawPoint(x, y, c);
				x++;
				ch <<= 1;
			}
			y++;
		}

		// shift to next character position
		x0 += 8;
	}
}

// Draw text buffer (size TEXTSIZE)
void DrawTextBuf(const char* textbuf, u8 col, u8 bgcol)
{
	// prepre
	u8 ch, c;
	int num, i, j;
	const u8* s;
	int y0, x0;
	u8* d;

	// loop through characters of text
	for (y0 = 0; y0 < HEIGHT; y0 += 8)
	{
		d = &pDrawBuf[y0*WIDTH];

		for (x0 = 0; x0 < WIDTH; x0 += 8)
		{
			// get next character of the text
			ch = (u8)*textbuf++;

			// prepare pointer to font sample
			s = &Font8x8[ch];

			// loop through lines of one character
			for (i = 8; i > 0; i--)
			{
				// get one font sample
				ch = *s;
				s += 256;

				// loop through pixels of one character line
				for (j = 8; j > 0; j--)
				{
					// draw pixel
					*d++ = ((ch & 0x80) != 0) ? col : bgcol;
					ch <<= 1;
				}
				d += WIDTH - 8;
			}
			d += 8 - 8*WIDTH;
		}
	}
}

// Draw text buffer with foreground color (size FTEXTSIZE)
void DrawFTextBuf(const char* textbuf, u8 bgcol)
{
	// prepre
	u8 ch, c, col;
	int num, i, j;
	const u8* s;
	int y0, x0;
	u8* d;

	// loop through characters of text
	for (y0 = 0; y0 < HEIGHT; y0 += 8)
	{
		d = &pDrawBuf[y0*WIDTH];

		for (x0 = 0; x0 < WIDTH; x0 += 8)
		{
			// get next character of the text
			ch = (u8)*textbuf++;

			// get foreground color
			col = (u8)*textbuf++;

			// prepare pointer to font sample
			s = &Font8x8[ch];

			// loop through lines of one character
			for (i = 8; i > 0; i--)
			{
				// get one font sample
				ch = *s;
				s += 256;

				// loop through pixels of one character line
				for (j = 8; j > 0; j--)
				{
					// draw pixel
					*d++ = ((ch & 0x80) != 0) ? col : bgcol;
					ch <<= 1;
				}
				d += WIDTH - 8;
			}
			d += 8 - 8*WIDTH;
		}
	}
}

// Draw image
void DrawImg(const u8* src, int xd, int yd, int w, int h, int ws)
{
	// limit coordinate X
	if (xd < 0)
	{
		w += xd;
		src += -xd;
		xd = 0;
	}

	// limit w
	if (xd + w > WIDTH) w = WIDTH - xd;
	if (w <= 0) return;

	// limit coordinate Y
	if (yd < 0)
	{
		h += yd;
		src += -yd*ws;
		yd = 0;
	}

	// limit h
	if (yd + h > HEIGHT) h = HEIGHT - yd;
	if (h <= 0) return;

	// draw image
	u8* d = &pDrawBuf[xd + yd*WIDTH];
	for (; h > 0; h--)
	{
		memcpy(d, src, w);
		d += WIDTH;
		src += ws;
	}
}

// Draw image with transparency (col = transparency key color)
void DrawBlit(const u8* src, int xd, int yd, int w, int h, int ws, u8 col)
{
	// limit coordinate X
	if (xd < 0)
	{
		w += xd;
		src += -xd;
		xd = 0;
	}

	// limit w
	if (xd + w > WIDTH) w = WIDTH - xd;
	if (w <= 0) return;

	// limit coordinate Y
	if (yd < 0)
	{
		h += yd;
		src += -yd*ws;
		yd = 0;
	}

	// limit h
	if (yd + h > HEIGHT) h = HEIGHT - yd;
	if (h <= 0) return;

	// draw image
	u8* d = &pDrawBuf[xd + yd*WIDTH];
	int i;
	u8 c;
	int wbd = WIDTH - w;
	int wbs = ws - w;
	for (; h > 0; h--)
	{
		for (i = w; i > 0; i--)
		{
			c = *src++;
			if (c != col) *d = c;
			d++;
		}
		d += wbd;
		src += wbs;
	}
}

// draw 8-bit image with 2D transformation matrix
//  src ... source image
//  ws ... source image width
//  hs ... source image height
//  x ... destination coordinate X
//  y ... destination coordinate Y
//  w ... destination width
//  h ... destination height
//  m ... transformation matrix (should be prepared using PrepDrawImg or PrepDrawPersp function)
//  mode ... draw mode DRAWIMG_*
//  color ... key or border color (DRAWIMG_PERSP mode: horizon offset)
// Note to wrap and perspective mode: Width and height of source image must be power of 2!
void DrawImgMat(const u8* src, int ws, int hs, int x, int y, int w, int h, const cMat2Df* m, u8 mode, u8 color)
{
	// limit x
	int x0 = -w/2; // start X coordinate
	if (x < 0)
	{
		w += x;
		x0 -= x;
		x = 0;
	}
	if (x + w > WIDTH) w = WIDTH - x;
	if (w <= 0) return;

	// limit y
	int h0 = h;
	int y0 = (mode == DRAWIMG_PERSP) ? (-h) : (-h/2); // start Y coordinate
	if (y < 0)
	{
		h += y;
		y0 -= y;
		y = 0;
	}
	if (y + h > HEIGHT) h = HEIGHT - y;
	if (h <= 0) return;

	// load transformation matrix and convert to integer fractional number
	int m11 = (int)(m->m11*FRACTMUL+0.5f);
	int m12 = (int)(m->m12*FRACTMUL+0.5f);
	int m13 = (int)(m->m13*FRACTMUL+0.5f);
	int m21 = (int)(m->m21*FRACTMUL+0.5f);
	int m22 = (int)(m->m22*FRACTMUL+0.5f);
	int m23 = (int)(m->m23*FRACTMUL+0.5f);

	// zero size image
	if ((m11 == 0) || (m22 == 0)) return;

	// prepare variables
	int xy0m, yy0m; // temporary Y members
	u8* d = &pDrawBuf[WIDTH*y + x]; // destination image
	int wbd = WIDTH - w; // destination width bytes
	int i, x2, y2;

	// wrap image
	if (mode == DRAWIMG_WRAP)
	{
		// coordinate mask
		int xmask = ws - 1;
		int ymask = hs - 1;

		for (; h > 0; h--)
		{
			xy0m = x0*m11 + y0*m12 + m13;
			yy0m = x0*m21 + y0*m22 + m23;
		
			for (i = w; i > 0; i--)
			{
				x2 = (xy0m>>FRACT) & xmask;
				y2 = (yy0m>>FRACT) & ymask;
				*d++ = src[x2 + y2*ws];
				xy0m += m11; // x0*m11
				yy0m += m21; // x0*m21
			}

			y0++;
			d += wbd;
		}
	}

	// no border
	else if (mode == DRAWIMG_NOBORDER)
	{
		// source image dimension
		u32 ww = ws;
		u32 hh = hs;

		for (; h > 0; h--)
		{
			xy0m = x0*m11 + y0*m12 + m13;
			yy0m = x0*m21 + y0*m22 + m23;

			for (i = w; i > 0; i--)
			{
				x2 = xy0m>>FRACT;
				y2 = yy0m>>FRACT;
				if (((u32)x2 < ww) && ((u32)y2 < hh)) *d = src[x2 + y2*ws];
				d++;
				xy0m += m11; // x0*m11
				yy0m += m21; // x0*m21
			}
			y0++;
			d += wbd;
		}
	}

	// clamp image
	else if (mode == DRAWIMG_CLAMP)
	{
		// source image dimension
		u32 ww = ws - 1;
		u32 hh = hs - 1;

		for (; h > 0; h--)
		{
			xy0m = x0*m11 + y0*m12 + m13;
			yy0m = x0*m21 + y0*m22 + m23;
		
			for (i = w; i > 0; i--)
			{
				x2 = xy0m>>FRACT;
				y2 = yy0m>>FRACT;
				if (x2 < 0) x2 = 0;
				if (x2 > ww) x2 = ww;
				if (y2 < 0) y2 = 0;
				if (y2 > hh) y2 = hh;
				*d++ = src[x2 + y2*ws];
				xy0m += m11; // x0*m11
				yy0m += m21; // x0*m21
			}
			y0++;
			d += wbd;
		}
	}

	// color border
	else if (mode == DRAWING_COLOR)
	{
		// source image dimension
		u32 ww = ws;
		u32 hh = hs;

		for (; h > 0; h--)
		{
			xy0m = x0*m11 + y0*m12 + m13;
			yy0m = x0*m21 + y0*m22 + m23;
		
			for (i = w; i > 0; i--)
			{
				x2 = xy0m>>FRACT;
				y2 = yy0m>>FRACT;
				if (((u32)x2 < ww) && ((u32)y2 < hh))
					*d = src[x2 + y2*ws];
				else
					*d = color;
				d++;
				xy0m += m11; // x0*m11
				yy0m += m21; // x0*m21
			}
			y0++;
			d += wbd;
		}
	}

	// transparency
	else if (mode == DRAWIMG_TRANSP)
	{
		u32 ww = ws;
		u32 hh = hs;
		u8 c;

		for (; h > 0; h--)
		{
			xy0m = x0*m11 + y0*m12 + m13;
			yy0m = x0*m21 + y0*m22 + m23;
		
			for (i = w; i > 0; i--)
			{
				x2 = xy0m>>FRACT;
				y2 = yy0m>>FRACT;
				if (((u32)x2 < ww) && ((u32)y2 < hh))
				{
					c = src[x2 + y2*ws];
					if (c != color) *d = c;
				}
				d++;
				xy0m += m11; // x0*m11
				yy0m += m21; // x0*m21
			}
			y0++;
			d += wbd;
		}
	}

	// perspective
	else if (mode == DRAWIMG_PERSP)
	{
		// coordinate mask
		int xmask = ws - 1;
		int ymask = hs - 1;

		for (; h > 0; h--)
		{
			int dist = FRACTMUL*h0/(h0 + y0 + color + 1);
			int m11b = (m11*dist)>>FRACT;
			int m21b = (m21*dist)>>FRACT;
			int m12b = (m12*dist)>>FRACT;
			int m22b = (m22*dist)>>FRACT;

			xy0m = x0*m11b + y0*m12b + m13;
			yy0m = x0*m21b + y0*m22b + m23;
		
			for (i = w; i > 0; i--)
			{
				x2 = (xy0m>>FRACT) & xmask;
				xy0m += m11b; // x0*m11

				y2 = (yy0m>>FRACT) & ymask;
				yy0m += m21b; // x0*m21

				*d++ = src[x2 + y2*ws];
			}
			y0++;
			d += wbd;
		}
	}
}

// draw image line interpolated
//  src = source image
//  xd,yd = destination coordinates
//  wd = destination width
//  ws = source width
//  wbs = source line pitch in bytes
void DrawImgLine(const u8* src, int xd, int yd, int wd, int ws, int wbs)
{
	// some base checks
	if ((wd <= 0) || (ws <= 0) || (yd < 0) || (yd >= HEIGHT)) return;

	// pixel increment
	int dinc = FRACTMUL*ws/wd;

	// prepare buffers
	u8* d = &pDrawBuf[xd + yd*WIDTH]; // destination address
	int i, j;

	int dadd = 0;
	for (i = 0; i < wd; i++)
	{
		*d++ = *src;
		dadd += dinc;
		src += dadd >> FRACT;
		dadd &= (FRACTMUL-1);
	}
}

// rainbow gradient table
//	GP0 ... B0 ... VGA B0 blue
//	GP1 ... B1 ... VGA B1
//	GP2 ... B2 ... VGA G0 green
//	GP3 ... B3 ... VGA G1
//	GP4 ... B4 ... VGA G2
//	GP5 ... B5 ... VGA R0 red
//	GP6 ... B6 ... VGA R1
//	GP7 ... B7 ... VGA R2
u8 RainbowGrad[] = {
	B7,			// dark red
	B5+B6+B7,		// red
	B4+B6+B7,		// orange
	B2+B3+B4+B5+B6+B7,	// yellow
	B2+B3+B4+B7,		// yellow-green
	B2+B3+B4,		// green
	B0+B3+B4,		// green-cyan
	B0+B1+B2+B3+B4,		// cyan
	B0+B1+B4,		// cyan-blue
	B0+B1,			// blue
	B0+B1+B7,		// blue-magenta
	B0+B1+B5+B6+B7,		// magenta
	B1+B7,			// dark magenta
};

#define RAINBOW_NUM count_of(RainbowGrad)

// generate gradient
void GenGrad(u8* dst, int w)
{
	// sample - rainbow pattern
	int i;
	for (i = 0; i < w; i++)
	{
		*dst++ = RainbowGrad[i*RAINBOW_NUM/w];
	}
}

// decode unsigned number into ASCIIZ text buffer (returns number of digits)
//  sep = thousand separator, 0=none
int DecUNum(char* buf, u32 num, char sep /* = 0 */)
{
	// temporary buffer (max. 10 digits + 3 separators + 1 NUL)
	char b[14];

	// decode number
	char* d = &b[13];
	*d = 0;
	int n = 0;
	u32 k;
	int sepn = 0;
	do {
		// thousand separator
		if ((sepn == 3) && (sep != 0))
		{
			sepn = 0;
			d--;
			*d = sep;
			n++;
		}

		k = num / 10;
		num -= k*10;
		d--;
		*d = (char)(num + '0');
		num = k;
		n++;
		sepn++;
	} while (num != 0);

	// copy number to destination buffer
	memcpy(buf, d, n+1);
	return n;
}

// decode signed number into ASCIIZ text buffer (returns number of digits)
//  sep = thousand separator, 0=none
int DecNum(char* buf, s32 num, char sep /* = 0 */)
{
	if (num >= 0) return DecUNum(buf, num, sep);

	num = -num;
	*buf++ = '-';
	return DecUNum(buf, num, sep) + 1;
}

// decode hex number (dig = number of digits)
void DecHexNum(char* buf, u32 num, u8 dig)
{
	buf += dig;
	*buf = 0;
	u8 ch;

	for (; dig > 0; dig--)
	{
		buf--;
		ch = (num & 0x0f) + '0';
		if (ch > '9') ch += 'A' - ('9'+1);
		*buf = ch;
		num >>= 4;
	}
}

// decode 2 digits of number
void Dec2Dig(char* buf, u8 num)
{
	u8 n = num/10;
	*buf++ = (char)(n + '0');
	num -= n*10;
	*buf++ = (char)(num + '0');
	*buf = 0;
}

// decode 2 digits of number with space character
void Dec2DigSpc(char* buf, u8 num)
{
	u8 n = num/10;
	if (n == 0)
		*buf++ = ' ';
	else
		*buf++ = (char)(n + '0');
	num -= n*10;
	*buf++ = (char)(num + '0');
	*buf = 0;
}

// get ascii text length
int TextLen(const char* txt)
{
	const char* t = txt;
	while (*t != 0) t++;
	return t - txt;
}
