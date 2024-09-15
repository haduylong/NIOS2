#include "gpio_drv.h"

/***************************************************************************
 * function: sseg_conv_hex
 * purpose: convert a hex digit to a 7-segment pattern
 * argument:
 * 		hex: hex digit (1-15)
 * return: 7-segment LED display pattern
 * note:
 * 		- blank pattern return if hex > 15
 * ************************************************************************/
alt_u8 sseg_conv_hex(int hex){
	/* active-low hex digit 7-sseg pattern (0-9, a-f); MSB ignored */
	static const alt_u8 SSEG_HEX_TABLE[16] = {
		0x40, 0x79, 0x24, 0x30, 0x19, 0x92, 0x02, 0x78, 0x00, 0x10, // 0-9
		0x88, 0x03, 0x46, 0x21, 0x06, 0x0E // a - f
	};

	alt_u8 ptn;

	if(hex < 15)
		ptn = SSEG_HEX_TABLE[hex];
	else
		ptn = 0xff;

	return (ptn);
}

/***************************************************************************
 * function: sseg_disp_ptn
 * purpose: display pattern in four 7-segment display
 * argument:
 * 		base: base address of 7-segment LED
 * 		*ptn: pointer to a 4-element pattern
 * return:
 * note:
 * example:
 * 		alt_u8 *msg
 * 		...
 * 		msg[0] = sseg_conv_hex(1);	// 1
 * 		msg[1] = sseg_conv_hex(2);	// 2
 * 		msg[2] = sseg_conv_hex(14);	// E
 * 		msg[3] = sseg_conv_hex(15);	// F
 *
 * 		sseg_disp_ptn(SSEG_BASE, msg);
 * ************************************************************************/
void sseg_disp_ptn(alt_u32 base, alt_u8 *ptn){
	alt_u32 sseg_data;
	int i;
	/* 32 bit data */
	for(i=0; i<4; i++){
		sseg_data = (sseg_data << 8) | *ptn;
		ptn++;
	}

	pio_write(base, sseg_data);
}
