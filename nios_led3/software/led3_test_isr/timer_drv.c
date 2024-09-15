/*
 * timer_drv.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Admin
 */

#include "timer_drv.h"

/**************************************************************************
 * funtion:
 * purpose:
 * argument:
 * return:
 * note:
 *************************************************************************/
void timer_write_prd(alt_u32 timer_base, alt_u32 prd){
	alt_u16 high, low;
	// unpack to high, low
	high = (alt_u16) (prd >> 16);
	low = (alt_u16) (prd & 0x0000ffff);
	// write to timer registor
	IOWR(timer_base, TIMER_PRDH_REG_OFT, high);
	IOWR(timer_base, TIMER_PRDL_REG_OFT, low);
	// configure timer to start, continuous mode, interrupt
	IOWR(timer_base, TIMER_CTRL_REG_OFT, 0x0007);
}

