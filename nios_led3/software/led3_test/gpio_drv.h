/*
 * gpio_drv.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Admin
 *
 *  Routines to access switch and leds
 */
#ifndef _GPIO_DRV_H
#define _GPIO_DRV_H

/* file inclusion */
#include "alt_types.h"
#include "io.h"

/* registor map */
#define PIO_DATA_REG_OFT	0	// data registor address offset
#define PIO_DIRT_REG_OFT	1	// direction registor addres offset
#define PIO_INTM_REG_OFT	2	// intertupt mask registor address offset
#define PIO_EDGE_REG_OFT	3	// edge capture registor address offset

/***********************************************************************
 * macro definitions
 ***********************************************************************/
/* read/write PIO data registor */
#define pio_write(base, data)		IOWR(base, PIO_DATA_REG_OFT, data)
#define pio_read(base)				IORD(base, PIO_DATA_REG_OFT)
/* read/clear push button edge capture registor  */
/* must write 0xf if the write individual bit option is used in SOPC */
#define btn_read(base)				IORD(base, PIO_EDGE_REG_OFT)
#define btn_clear(base)				IOWR(base, PIO_EDGE_REG_OFT, 0xf)
#define btn_is_pressed(base)		(IORD(base, PIO_EDGE_REG_OFT) != 0)

/**********************************************************************
 * funtions prototype
 **********************************************************************/
alt_u8 sseg_conv_hex(int hex);
void sseg_disp_ptn(alt_u32 base, alt_u8 *ptn); // pattern

#endif
