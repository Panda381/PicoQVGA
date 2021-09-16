
// ****************************************************************************
//
//                             Random number generator
//
// ****************************************************************************

#include "include.h"

// random generator seed
u32 RandSeed;

// shift random generator seed
void RandShift()
{
	RandSeed = RandSeed*214013 + 2531011;
}

// randomize random generator from ROSC counter
Bool RandInitSeed()
{
	int i;
	for (i = 32; i > 0; i--) RandSeed = (RandSeed << 1) | (rosc_hw->randombit & 1);
	i = (RandU8() & 0x1f) + 5;
	for (; i > 0; i--) RandShift();
	return True;
}

// random auto init
Bool RandAutoInit = RandInitSeed();

// ============================================================================
//                  generate random number in full range
// ============================================================================

u8 RandU8()
{
	RandShift();
	return (u8)(RandSeed >> 24);
}

u16 RandU16()
{
	RandShift();
	return (u16)(RandSeed >> 16);
}

u32 RandU32()
{
	RandShift();
	u32 k = RandSeed;
	RandShift();
	return (k & 0xffff0000) | (RandSeed >> 16);
}

// ============================================================================
//     generate float random number in range 0 (including) to 1 (excluding)
// ============================================================================

float RandFloat()
{
	RandShift();
	return (float)(RandSeed >> (32-24))*(1.0f/0x1000000);
}

// ============================================================================
//    generate double random number in range 0 (including) to 1 (excluding)
// ============================================================================

double RandDouble()
{
	RandShift();
	return (double)RandSeed*(1.0/0x100000000ULL);
}

// ============================================================================
//         generate random number in range 0 to MAX (including)
// ============================================================================

u8 RandU8Max(u8 max)
{
	u8 res;
	u8 mask;

	// zero maximal value
	if (max == 0) return 0;

	// prepare mask
	res = 0xff;
	do {
		mask = res;
		res >>= 1;
	} while (res >= max);

	// generate random number
	do {
		res = RandU8() & mask;
	} while (res > max);

	return res;
}

u16 RandU16Max(u16 max)
{
	u16 res;
	u16 mask;

	// zero maximal value
	if (max == 0) return 0;

	// prepare mask
	res = 0xffff;
	do {
		mask = res;
		res >>= 1;
	} while (res >= max);

	// generate random number
	do {
		res = RandU16() & mask;
	} while (res > max);

	return res;
}

u32 RandU32Max(u32 max)
{
	u32 res;
	u32 mask;

	// zero maximal value
	if (max == 0) return 0;

	// prepare mask
	res = 0xffffffff;
	do {
		mask = res;
		res >>= 1;
	} while (res >= max);

	// generate random number
	do {
		res = RandU32() & mask;
	} while (res > max);

	return res;
}

s8 RandS8Max(s8 max)
{
	if (max >= 0)
		return (s8)RandU8Max((u8)max);
	else
		return -(s8)RandU8Max((u8)-max);
}

s16 RandS16Max(s16 max)
{
	if (max >= 0)
		return (s16)RandU16Max((u16)max);
	else
		return -(s16)RandU16Max((u16)-max);
}

s32 RandS32Max(s32 max)
{
	if (max >= 0)
		return (s32)RandU32Max((u32)max);
	else
		return -(s32)RandU32Max((u32)-max);
}

// ============================================================================
//   generate float random number in range 0 (including) to MAX (excluding)
// ============================================================================

float RandFloatMax(float max) { return RandFloat() * max; }

// ============================================================================
//   generate double random number in range 0 (including) to MAX (excluding)
// ============================================================================

double RandDoubleMax(double max) { return RandDouble() * max; }

// ============================================================================
//        generate random number in range MIN to MAX (including)
// ============================================================================
// If MIN > MAX, then number is generated out of interval.

u8 RandU8MinMax(u8 min, u8 max)
{
	return RandU8Max(max - min) + min;
}

u16 RandU16MinMax(u16 min, u16 max)
{
	return RandU16Max(max - min) + min;
}

u32 RandU32MinMax(u32 min, u32 max)
{
	return RandU32Max(max - min) + min;
}

s8 RandS8MinMax(s8 min, s8 max)
{
	return (s8)RandU8Max(max - min) + min;
}

s16 RandS16MinMax(s16 min, s16 max)
{
	return (s16)RandU16Max(max - min) + min;
}

s32 RandS32MinMax(s32 min, s32 max)
{
	return (s32)RandU32Max(max - min) + min;
}

float RandFloatMinMax(float min, float max)
{
	return RandFloatMax(max - min) + min;
}

double RandDoubleMinMax(double min, double max)
{
	return RandDoubleMax(max - min) + min;
}
