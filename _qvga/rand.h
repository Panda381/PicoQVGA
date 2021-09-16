
// ****************************************************************************
//
//                             Random number generator
//
// ****************************************************************************

#ifndef _RAND_H
#define _RAND_H

// random generator seed
extern u32 RandSeed;

// shift random generator seed
void RandShift();

// randomize random generator from ROSC counter
Bool RandInitSeed();

// generate random number in full range
u8 RandU8();
u16 RandU16();
u32 RandU32();
inline s8 RandS8() { return (s8)RandU8(); }
inline s16 RandS16() { return (s16)RandU16(); }
inline s32 RandS32() { return (s32)RandU32(); }

// generate float random number in range 0 (including) to 1 (excluding)
float RandFloat();

// generate double random number in range 0 (including) to 1 (excluding)
double RandDouble();

// generate random number in range 0 to MAX (including)
u8 RandU8Max(u8 max);
u16 RandU16Max(u16 max);
u32 RandU32Max(u32 max);
s8 RandS8Max(s8 max);
s16 RandS16Max(s16 max);
s32 RandS32Max(s32 max);

// generate decimal random number in range 0 (including) to MAX (excluding)
float RandFloatMax(float max);
double RandDoubleMax(double max);

// generate random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval.
u8 RandU8MinMax(u8 min, u8 max);
u16 RandU16MinMax(u16 min, u16 max);
u32 RandU32MinMax(u32 min, u32 max);
s8 RandS8MinMax(s8 min, s8 max);
s16 RandS16MinMax(s16 min, s16 max);
s32 RandS32MinMax(s32 min, s32 max);
float RandFloatMinMax(float min, float max);
double RandDoubleMinMax(double min, double max);

#endif // _RAND_H
