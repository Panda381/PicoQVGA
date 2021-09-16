
// ****************************************************************************
//
//                                  QVGA
//
// ****************************************************************************

#ifndef _QVGA_H
#define _QVGA_H

#include "config.h"

// QVGA Colors
//	B0 ... VGA B0 blue
//	B1 ... VGA B1
//	B2 ... VGA G0 green
//	B3 ... VGA G1
//	B4 ... VGA G2
//	B5 ... VGA R0 red
//	B6 ... VGA R1
//	B7 ... VGA R2

#define COL_BLACK	0

#define COL_DARKBLUE	B0
#define COL_SEMIBLUE	B1
#define COL_BLUE	(B0+B1)
#define COL_MOREBLUE	(COL_BLUE+B3+B6)
#define COL_LIGHTBLUE	(COL_BLUE+B4+B7)

#define COL_DARKGREEN	B3
#define COL_SEMIGREEN	B4
#define COL_GREEN	(B2+B3+B4)
#define COL_MOREGREEN	(COL_GREEN+B0+B6)
#define COL_LIGHTGREEN	(COL_GREEN+B1+B7)

#define COL_DARKRED	B6
#define COL_SEMIRED	B7
#define COL_RED		(B5+B6+B7)
#define COL_MORERED	(COL_RED+B0+B3)
#define COL_LIGHTRED	(COL_RED+B1+B4)

#define COL_DARKCYAN	(B0+B3)
#define COL_SEMICYAN	(B1+B4)
#define COL_CYAN	(B0+B1+B2+B3+B4)

#define COL_DARKMAGENTA (B0+B6)
#define COL_SEMIMAGENTA (B1+B7)
#define COL_MAGENTA	(B0+B1+B5+B6+B7)

#define COL_DARKYELLOW	(B3+B6)
#define COL_SEMIYELLOW	(B4+B7)
#define COL_YELLOW	(B2+B3+B4+B5+B6+B7)

#define COL_GRAY0	0
#define COL_GRAY1	(B2+B5)
#define COL_GRAY2	(B0+B3+B6)
#define COL_GRAY3	(B0+B2+B3+B5+B6)
#define COL_GRAY4	(B1+B4+B7)
#define COL_GRAY5	(B1+B2+B4+B5+B7)
#define COL_GRAY6	(B0+B1+B3+B4+B6+B7)
#define COL_GRAY7	(B0+B1+B2+B3+B4+B5+B6+B7)

#define COL_WHITE	COL_GRAY7

// compose color from RGB
#define COLRGB(r,g,b) ((u8)(((r)&0xe0)|(((g)&0xe0)>>3)|((b)>>6)))

// display frame buffers
extern ALIGNED u8 FrameBuf[FRAMESIZE];

// pointer to current frame buffer
extern u8* pFrameBuf;

// initialize QVGA (can change system clock)
void QVgaInit();

// QVGA core
void QVgaCore();

// execute core 1 remote function
void Core1Exec(void (*fnc)());

// check if core 1 is busy (executing remote function)
Bool Core1Busy();

// wait if core 1 is busy (executing remote function)
void Core1Wait();

// check VSYNC
Bool IsVSync();

// wait for VSync scanline
void WaitVSync();

#endif // _QVGA_H
