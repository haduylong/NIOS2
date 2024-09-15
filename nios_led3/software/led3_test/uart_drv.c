/*
 * uart_drv.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Admin
 *
 *  Demo JATG UART driver
 */
#include "uart_drv.h"

void jtag_wr_str(alt_u32 jatg_base, char *msg){
	alt_u32 data32;

	while(*msg){
		data32 = (alt_u32) *msg;
		if(jtag_rd_wspace(jatg_base) != 0){ // buffer space available
			jtag_wr_chr(jatg_base, data32); // send a char
			msg++;
		}
	}
}
