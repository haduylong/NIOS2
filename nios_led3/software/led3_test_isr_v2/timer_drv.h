/*
 * timer_drv.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Admin
 */

#ifndef _TIMER_DRV_H
#define _TIMER_DRV_H
/* file inclusion */
#include "alt_types.h"
#include "io.h"

/* registor map */
#define TIMER_STAT_REG_OFT	0	// stattus registor address offset
#define TIMER_CTRL_REG_OFT	1	// control registor address offset
#define TIMER_PRDL_REG_OFT	2	// period low registor address offset
#define TIMER_PRDH_REG_OFT	3	// period high registor address offset

/* check "to" field for ms tick (timeout) */
#define timer_read_tick(base) \
	(IORD(base, TIMER_STAT_REG_OFT) & 0x01)
/* clear "to" field */
#define timer_clear_tick(base) \
	(IOWR(base, TIMER_STAT_REG_OFT, 0))

/**********************************************************************
 * funtions prototype
 **********************************************************************/
void timer_write_prd(alt_u32 timer_base, alt_u32 prd);
#endif
