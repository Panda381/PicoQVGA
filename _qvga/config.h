
// ****************************************************************************
//
//                                    QVGA configuration
//
// ****************************************************************************
// port pins
//	GP0 ... Audio: PWM mono, 22050 Hz, 8 bits unsigned
//	GP2 ... VGA B0 blue
//	GP3 ... VGA B1
//	GP4 ... VGA G0 green
//	GP5 ... VGA G1
//	GP6 ... VGA G2
//	GP7 ... VGA R0 red
//	GP8 ... VGA R1
//	GP9 ... VGA R2
//	GP10 ... VGA HSYNC/CSYNC synchronization (inverted: negative SYNC=LOW=0x80, BLACK=HIGH=0x00)
//	GP11 ... VSYNC

// QVGA port pins
#define QVGA_GPIO_FIRST	2	// first QVGA GPIO
#define QVGA_GPIO_NUM	8	// number of QVGA color GPIOs, without HSYNC and VSYNC
#define QVGA_GPIO_LAST	(QVGA_GPIO_FIRST+QVGA_GPIO_NUM-1) // last QVGA GPIO
#define QVGA_GPIO_HSYNC	10	// QVGA HSYNC/CSYNC GPIO
#define QVGA_GPIO_VSYNC	(QVGA_GPIO_HSYNC+1) // QVGA VSYNC GPIO

// QVGA PIO and state machines
#define QVGA_PIO	pio0	// QVGA PIO
#define QVGA_SM		0	// QVGA state machine

// QVGA DMA channel
#define QVGA_DMA_CB	0	// DMA control block of base layer
#define QVGA_DMA_PIO	1	// DMA copy data to PIO (raises IRQ0 on quiet)

// QVGA display resolution
#define WIDTH	320		// display width
#define HEIGHT	240		// display height
#define WIDTHBYTE WIDTH		// scanline length (byte offset between image rows)
#define FRAMESIZE (WIDTH*HEIGHT) // display frame size in bytes (=76800)

// QVGA horizonal timings
#define QVGA_CLKDIV	2	// SM divide clock ticks
#define QVGA_CPP	5	// state machine clock ticks per pixel
#define QVGA_SYSCPP	(QVGA_CLKDIV*QVGA_SMCPP) // system clock ticks per pixel (=10)

// 126 MHz timings
#define QVGA_VCO	(1008*1000*1000) // PLL VCO frequency in Hz
#define QVGA_PD1	4	// PLL PD1
#define QVGA_PD2	2	// PLL PD2
#define QVGA_TOTAL	2002	// total clock ticks (= QVGA_HSYNC + QVGA_BP + WIDTH*QVGA_CPP[1600] + QVGA_FP)
#define QVGA_HSYNC	240	// horizontal sync clock ticks
#define QVGA_BP	121	// back porch clock ticks
#define QVGA_FP	41	// front porch clock ticks

// 125 MHz timings (first and last pixel can be hidden)
/*
#define QVGA_VCO	(1500*1000*1000) // PLL VCO frequency in Hz
#define QVGA_PD1	6	// PLL PD1
#define QVGA_PD2	2	// PLL PD2
#define QVGA_TOTAL	1986	// total clock ticks (= QVGA_HSYNC + QVGA_BP + WIDTH*QVGA_CPP[1600] + QVGA_FP)
#define QVGA_HSYNC	238	// horizontal sync clock ticks
#define QVGA_BP	114	// back porch clock ticks
#define QVGA_FP	34	// front porch clock ticks
*/

// QVGA vertical timings
#define QVGA_VTOT	525	// total scanlines (= QVGA_VSYNC + QVGA_VBACK + QVGA_VACT + QVGA_VFRONT)
#define QVGA_VSYNC	2	// length of V sync (number of scanlines)
#define QVGA_VBACK	33	// V back porch
#define QVGA_VACT	480	// V active scanlines (= 2*HEIGHT)
#define QVGA_VFRONT	10	// V front porch

// === PWM sound configuration
#define PWMSND_GPIO	0	// PWM output GPIO pin (0..29)
#define PWMSND_SLICE	((PWMSND_GPIO>>1)&7) // PWM slice index
#define PWMSND_CHAN	(PWMSND_GPIO&1) // PWM channel index
