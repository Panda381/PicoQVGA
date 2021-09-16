
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

#define IMGH	160	// image height
#define WATERH	(HEIGHT-IMGH) // water height

// format: 8-bit pixel graphics
// image width: 320 pixels
// image height: 160 lines
// image pitch: 320 bytes
extern ALIGNED const u8 ForestImg[51200];

// format: 8-bit pixel graphics
// image width: 256 pixels
// image height: 29 lines
// image pitch: 256 bytes
extern ALIGNED const u8 DuckImg[7424];

// sound format: PCM mono 8-bit 22050Hz
extern const u8 RiverSnd[105000];

#endif // _MAIN_H
