
// ****************************************************************************
//
//                                  QVGA
//
// ****************************************************************************
// VGA resolution:
// - 640x480 pixels
// - vertical frequency 60 Hz
// - horizontal frequency 31.4685 kHz
// - pixel clock 25.175 MHz
//
// QVGA resolution:
// - 320x240 pixels
// - vertical double image scanlines
// - vertical frequency 60 Hz
// - horizontal frequency 31.4685 kHz
// - pixel clock 12.5875 MHz
//
// VGA vertical timings:
// - 525 scanlines total
// - line 1,2: (2) vertical sync
// - line 3..35: (33) dark
// - line 36..515: (480) image lines 0..479
// - line 516..525: (10) dark
//
// VGA horizontal timings:
// - 31.77781 total scanline in [us] (800 pixels, QVGA 400 pixels)
// - 0.63556 H front porch (after image, before HSYNC) in [us] (16 pixels, QVGA 8 pixels)
// - 3.81334 H sync pulse in [us] (96 pixels, QVGA 48 pixels)
// - 1.90667 H back porch (after HSYNC, before image) in [us] (48 pixels, QVGA 24 pixels)
// - 25.42224 H full visible in [us] (640 pixels, QVGA 320 pixels)
// - 0.0397222625 us per pixel at VGA, 0.079444525 us per pixel at QVGA
//
// We want reach 25.175 pixel clock (at 640x480). Default system clock is 125 MHz, which is
// approx. 5x pixel clock. We need 25.175*5 = 125.875 MHz. We use nearest frequency 126 MHz.
//	126000, 1512000, 126, 6, 2,     // 126.00MHz, VC0=1512MHz, FBDIV=126, PD1=6, PD2=2
//	126000, 504000, 42, 4, 1,       // 126.00MHz, VC0=504MHz, FBDIV=42, PD1=4, PD2=1
//	sysclk=126.000000 MHz, vco=504 MHz, fbdiv=42, pd1=4, pd2=1
//	sysclk=126.000000 MHz, vco=504 MHz, fbdiv=42, pd1=2, pd2=2
//	sysclk=126.000000 MHz, vco=756 MHz, fbdiv=63, pd1=6, pd2=1
//	sysclk=126.000000 MHz, vco=756 MHz, fbdiv=63, pd1=3, pd2=2
//	sysclk=126.000000 MHz, vco=1008 MHz, fbdiv=84, pd1=4, pd2=2 !!!!!
//	sysclk=126.000000 MHz, vco=1260 MHz, fbdiv=105, pd1=5, pd2=2
//	sysclk=126.000000 MHz, vco=1512 MHz, fbdiv=126, pd1=6, pd2=2
//	sysclk=126.000000 MHz, vco=1512 MHz, fbdiv=126, pd1=4, pd2=3
// Pixel clock is now:
//      5 system clock ticks per pixel at VGA ... pixel clock = 25.2 MHz, 0.039683 us per pixel
//     10 system clock ticks per pixel at QVGA ... pixel clock = 12.6 MHz, 0.079365 us per pixel
//
// - active image is 640*5=3200 clock ticks = 25.3968 us (QVGA: 1600 clock ticks)
// - total scanline is 126*31.77781=4004 clock ticks (QVGA: 2002 clock ticks)
// - H front porch = 82 clock ticks (QVGA: 41 clock ticks)
// - H sync pulse = 480 clock ticks (QVGA: 240 clock ticks)
// - H back porch = 242 clock ticks (QVGA: 121 clock ticks)

// in case of system clock = 125 MHz
// - PIO clock = system clock / 2
// - 5 PIO clocks per pixel = 10 system clocks per pixel
// - PIO clock = 62.5 MHz
// - pixel clock = 12.5 MHz
// - active image (320 pixels): 320*5 = 1600 PIO clocks = 3200 system ticks = 25.6 us (2.2 pixels stays invisible)
// - total scanline: 125*31.77781 = 3972 system clocks = 1986 PIO clocks
// - H front porch = 34 PIO clk
// - H sync = 238 PIO clk
// - H back = 114 PIO clk

#include "include.h"

// PIO command (jmp=program address, num=loop counter)
#define QVGACMD(jmp, num) ( ((u32)((jmp)+QVGAOff)<<27) | (u32)(num))

// display frame buffer
ALIGNED u8 FrameBuf[FRAMESIZE];

// pointer to current frame buffer
u8* pFrameBuf = FrameBuf;

// QVGA PIO
uint QVGAOff;	// offset of QVGA PIO program

// Scanline data buffers (commands sent to PIO)
u32 ScanLineImg[3];	// image: HSYNC ... back porch ... image command
u32 ScanLineFp;		// front porch
u32 ScanLineDark[2];	// dark: HSYNC ... back porch + dark + front porch
u32 ScanLineSync[2];	// vertical sync: VHSYNC ... VSYNC(back porch + dark + front porch)

// Scanline control buffers
#define CB_MAX 8	// size of one scanline control buffer (1 link to data buffer requires 2x u32)
u32 ScanLineCB[2*CB_MAX]; // 2 control buffers
int QVgaBufInx;		// current running control buffer
u32* ScanLineCBNext;	// next control buffer

// handler variables
volatile int QVgaScanLine; // current processed scan line 0... (next displayed scan line)
volatile u32 QVgaFrame;	// frame counter

// saved integer divider state
hw_divider_state_t SaveDividerState;

// VGA DMA handler - called on end of every scanline
void __not_in_flash_func(QVgaLine)()
{
	// Clear the interrupt request for DMA control channel
	dma_hw->ints0 = (1u << QVGA_DMA_PIO);

	// update DMA control channel and run it
	dma_channel_set_read_addr(QVGA_DMA_CB, ScanLineCBNext, true);

	// save integer divider state
	hw_divider_save_state(&SaveDividerState);

	// switch current buffer index (bufinx = current preparing buffer, MiniVgaBufInx = current running buffer)
	int bufinx = QVgaBufInx;
	QVgaBufInx = bufinx ^ 1;

	// prepare control buffer to be processed
	u32* cb = &ScanLineCB[bufinx*CB_MAX];
	ScanLineCBNext = cb;

	// increment scanline (1..)
	int line = QVgaScanLine; // current scanline
	line++; 		// new current scanline
	if (line >= QVGA_VTOT) // last scanline?
	{
		QVgaFrame++;	// increment frame counter
		line = 0; 	// restart scanline
	}
	QVgaScanLine = line;	// store new scanline

	// check scanline
	line -= QVGA_VSYNC;
	if (line < 0)
	{
		// VSYNC
		*cb++ = 2;
		*cb++ = (u32)&ScanLineSync[0];
	}
	else
	{
		// front porch and back porch
		line -= QVGA_VBACK;
		if ((line < 0) || (line >= QVGA_VACT))
		{
			// dark line
			*cb++ = 2;
			*cb++ = (u32)&ScanLineDark[0];
		}

		// image scanlines
		else
		{
			// prepare image line
			line >>= 1;

			// HSYNC ... back porch ... image command
			*cb++ = 3;
			*cb++ = (u32)&ScanLineImg[0];

			// image data
			*cb++ = WIDTH/4;
			*cb++ = (u32)&pFrameBuf[line*WIDTH];

			// front porch
			*cb++ = 1;
			*cb++ = (u32)&ScanLineFp;
		}
	}

	// end mark
	*cb++ = 0;
	*cb = 0;

	// restore integer divider state
	hw_divider_restore_state(&SaveDividerState);
}

// initialize QVGA PIO
void QVgaPioInit()
{
	int i;

	// load PIO program
	QVGAOff = pio_add_program(QVGA_PIO, &qvga_program);

	// configure GPIOs for use by PIO
	for (i = QVGA_GPIO_FIRST; i <= QVGA_GPIO_LAST; i++) pio_gpio_init(QVGA_PIO, i);
	pio_gpio_init(QVGA_PIO, QVGA_GPIO_HSYNC);
	pio_gpio_init(QVGA_PIO, QVGA_GPIO_VSYNC);

	// set pin direction to output
	pio_sm_set_consecutive_pindirs(QVGA_PIO, QVGA_SM, QVGA_GPIO_FIRST, QVGA_GPIO_NUM, true);
	pio_sm_set_consecutive_pindirs(QVGA_PIO, QVGA_SM, QVGA_GPIO_HSYNC, 2, true);

	// negate HSYNC and VSYNC output
	gpio_set_outover(QVGA_GPIO_HSYNC, GPIO_OVERRIDE_INVERT);
	gpio_set_outover(QVGA_GPIO_VSYNC, GPIO_OVERRIDE_INVERT);

	// prepare default PIO program config
	pio_sm_config cfg = qvga_program_get_default_config(QVGAOff);

	// map state machine's OUT and MOV pins	
	sm_config_set_out_pins(&cfg, QVGA_GPIO_FIRST, QVGA_GPIO_NUM);

	// set sideset pins (HSYNC and VSYNC)
	sm_config_set_sideset_pins(&cfg, QVGA_GPIO_HSYNC);

	// join FIFO to send only
	sm_config_set_fifo_join(&cfg, PIO_FIFO_JOIN_TX);

	// PIO clock divider
	sm_config_set_clkdiv(&cfg, QVGA_CLKDIV);

	// shift right, autopull, pull threshold
	sm_config_set_out_shift(&cfg, true, true, 32);

	// initialize state machine
	pio_sm_init(QVGA_PIO, QVGA_SM, QVGAOff+qvga_offset_entry, &cfg);
}

// initialize scanline buffers
void QVgaBufInit()
{
	// image scanline data buffer: HSYNC ... back porch ... image command
	ScanLineImg[0] = QVGACMD(qvga_offset_hsync, QVGA_HSYNC-3); // HSYNC
	ScanLineImg[1] = QVGACMD(qvga_offset_dark, QVGA_BP-4); // back porch
	ScanLineImg[2] = QVGACMD(qvga_offset_output, WIDTH-2); // image

	// front porch
	ScanLineFp = QVGACMD(qvga_offset_dark, QVGA_FP-4); // front porch

	// dark scanline: HSYNC ... back porch + dark + front porch
	ScanLineDark[0] = QVGACMD(qvga_offset_hsync, QVGA_HSYNC-3); // HSYNC
	ScanLineDark[1] = QVGACMD(qvga_offset_dark, QVGA_TOTAL-QVGA_HSYNC-4); // back porch + dark + front porch

	// vertical sync: VHSYNC ... VSYNC(back porch + dark + front porch)
	ScanLineSync[0] = QVGACMD(qvga_offset_vhsync, QVGA_HSYNC-3); // VHSYNC
	ScanLineSync[1] = QVGACMD(qvga_offset_vsync, QVGA_TOTAL-QVGA_HSYNC-3); // VSYNC(back porch + dark + front porch)

	// control buffer 1 - initialize to VSYNC
	ScanLineCB[0] = 2; // send 2x u32 (send ScanLineSync)
	ScanLineCB[1] = (u32)&ScanLineSync[0]; // VSYNC data buffer
	ScanLineCB[2] = 0; // stop mark
	ScanLineCB[3] = 0; // stop mark

	// control buffer 1 - initialize to VSYNC
	ScanLineCB[CB_MAX+0] = 2; // send 2x u32 (send ScanLineSync)
	ScanLineCB[CB_MAX+1] = (u32)&ScanLineSync[0]; // VSYNC data buffer
	ScanLineCB[CB_MAX+2] = 0; // stop mark
	ScanLineCB[CB_MAX+3] = 0; // stop mark
}

// initialize QVGA DMA
//   control blocks aliases:
//                  +0x0        +0x4          +0x8          +0xC (Trigger)
// 0x00 (alias 0):  READ_ADDR   WRITE_ADDR    TRANS_COUNT   CTRL_TRIG
// 0x10 (alias 1):  CTRL        READ_ADDR     WRITE_ADDR    TRANS_COUNT_TRIG
// 0x20 (alias 2):  CTRL        TRANS_COUNT   READ_ADDR     WRITE_ADDR_TRIG
// 0x30 (alias 3):  CTRL        WRITE_ADDR    TRANS_COUNT   READ_ADDR_TRIG ... we use this!
void QVgaDmaInit()
{

// ==== prepare DMA control channel

	// prepare DMA default config
	dma_channel_config cfg = dma_channel_get_default_config(QVGA_DMA_CB);

	// increment address on read from memory
	channel_config_set_read_increment(&cfg, true);

	// increment address on write to DMA port
	channel_config_set_write_increment(&cfg, true);

	// each DMA transfered entry is 32-bits
	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_32);

	// write ring - wrap to 8-byte boundary (TRANS_COUNT and READ_ADDR_TRIG of data DMA)
	channel_config_set_ring(&cfg, true, 3);

	// DMA configure
	dma_channel_configure(
		QVGA_DMA_CB,		// channel
		&cfg,			// configuration
		&dma_hw->ch[QVGA_DMA_PIO].al3_transfer_count, // write address
		&ScanLineCB[0],		// read address - as first, control buffer 1 will be sent out
		2,			// number of transfers in u32 (number of transfers per one request from data DMA)
		false			// do not start yet
	);

// ==== prepare DMA data channel

	// prepare DMA default config
	cfg = dma_channel_get_default_config(QVGA_DMA_PIO);

	// increment address on read from memory
	channel_config_set_read_increment(&cfg, true);

	// do not increment address on write to PIO
	channel_config_set_write_increment(&cfg, false);

	// each DMA transfered entry is 32-bits
	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_32);

	// DMA data request for sending data to PIO
	channel_config_set_dreq(&cfg, pio_get_dreq(QVGA_PIO, QVGA_SM, true));

	// chain channel to DMA control block
	channel_config_set_chain_to(&cfg, QVGA_DMA_CB);

	// raise the IRQ flag when 0 is written to a trigger register (end of chain)
	channel_config_set_irq_quiet(&cfg, true);

	// set high priority
	cfg.ctrl |= DMA_CH0_CTRL_TRIG_HIGH_PRIORITY_BITS;

	// DMA configure
	dma_channel_configure(
		QVGA_DMA_PIO,		// channel
		&cfg,			// configuration
		&QVGA_PIO->txf[QVGA_SM], // write address
		NULL,			// read address
		0,			// number of transfers in u32
		false			// do not start immediately
	);

// ==== initialize IRQ0, raised from DMA data channel

	// enable DMA channel IRQ0
	dma_channel_set_irq0_enabled(QVGA_DMA_PIO, true);

	// set DMA IRQ handler
	irq_set_exclusive_handler(DMA_IRQ_0, QVgaLine);

	// set highest IRQ priority
	irq_set_priority(DMA_IRQ_0, 0);
}

// initialize QVGA (can change system clock)
void QVgaInit()
{
	// initialize system clock
	set_sys_clock_pll(QVGA_VCO, QVGA_PD1, QVGA_PD2);

	// initialize PIO
	QVgaPioInit();

	// initialize scanline buffers
	QVgaBufInit();

	// initialize DMA
	QVgaDmaInit();

	// initialize parameters
	QVgaScanLine = 0; // currently processed scanline
	QVgaBufInx = 0; // at first, control buffer 1 will be sent out
	QVgaFrame = 0; // current frame
	ScanLineCBNext = &ScanLineCB[CB_MAX]; // send control buffer 2 next

	// enable DMA IRQ
	irq_set_enabled(DMA_IRQ_0, true);

	// start DMA
	dma_channel_start(QVGA_DMA_CB);

	// run state machine
	pio_sm_set_enabled(QVGA_PIO, QVGA_SM, true);
}

void (* volatile Core1Fnc)() = NULL; // core 1 remote function

// QVGA core
void QVgaCore()
{
	void (*fnc)();

	// initialize QVGA
	QVgaInit();

	// infinite loop
	while (true)
	{
		// data memory barrier
		__dmb();

		// execute remote function
		fnc = Core1Fnc;
		if (fnc != NULL)
		{
			fnc();
			__dmb();
			Core1Fnc = NULL;
		}
	}
}

// execute core 1 remote function
void Core1Exec(void (*fnc)())
{
	__dmb();
	Core1Fnc = fnc;
	__dmb();
}

// check if core 1 is busy (executing remote function)
Bool Core1Busy()
{
	__dmb();
	return Core1Fnc != NULL;
}

// wait if core 1 is busy (executing remote function)
void Core1Wait()
{
	while (Core1Busy()) {}
}

// check VSYNC
Bool IsVSync()
{
	return ((QVgaScanLine < QVGA_VSYNC+QVGA_VBACK) ||
		(QVgaScanLine >= QVGA_VTOT-QVGA_VFRONT));
}

// wait for VSync scanline
void WaitVSync()
{
	// wait for end of VSync
	while (IsVSync()) { __dmb(); }

	// wait for start of VSync
	while (!IsVSync()) { __dmb(); }
}

// auto initialization
Bool QVgaAutoInit()
{
	multicore_launch_core1(QVgaCore);
	return True;
}

Bool QVgaAutoInitOK = QVgaAutoInit();
