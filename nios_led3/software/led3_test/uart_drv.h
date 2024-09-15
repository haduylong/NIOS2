/*
 * uart_drv.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Admin
 *
 *  Demo JATG UART driver header
 */

#ifndef _UART_DRV_H
#define _UART_DRV_H
/* file inclusion */
#include "alt_types.h"
#include "io.h"

/* registor map */
#define JUART_DATA_REG_OFT	0	// data registor address offset
#define JUART_CTRL_REG_OFT	1	// control registor address offset

/* check slot available in FIFO buffer */
#define jtag_rd_wspace(base) \
	((IORD(base, JUART_CTRL_REG_OFT) & 0xffff0000) >> 16)
/* write an 8-bit char */
#define jtag_wr_chr(base, data) \
	(IOWR(base, JUART_DATA_REG_OFT, data & 0x000000ff))

/**********************************************************************
 * funtions prototype
 **********************************************************************/
void jtag_wr_str(alt_u32 jatg_base, char *msg);
#endif
