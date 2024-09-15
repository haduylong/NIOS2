#include "stdio.h"
#include "unistd.h"
#include "sys/alt_alarm.h" // for alt_nticks() and alt_ticks_per_second()

#include "alt_types.h"
#include "system.h"

#include "gpio_drv.h"

typedef struct flash_cmd{
	int pause;
	int prd;
} cmd_type;

/*****************************************************************************
 * funtion: flashsys_init_v2
 * purpose: Init system, clear capture push button,  set 1-ms timeout period
 * argument:
 * 		pushbutton base address
 * return:
 * note:
 *****************************************************************************/
void flashsys_init_v2(alt_u32 btn_base){
	btn_clear(btn_base);				// clear edge capture button
}

/*****************************************************************************
 * funtion: sw_get_command_v1
 * purpose: get command from pushbutton and switch
 * arguments:
 * 		btn_base:
 * 		sw_base:
 * 		*cmd:
 * return:
 * note:
 *****************************************************************************/
void sw_get_command_v1(alt_u32 btn_base, alt_u32 sw_base, cmd_type *cmd){
	alt_u8 btn;

	btn = (alt_u8) btn_read(btn_base) & 0x03; 			// read 2 button
	if(btn != 0){ 										// a button is press
		if(btn == 0x01) 								// btn 0 is pressed
			cmd->pause = cmd->pause ^ 1; 				// toggle pause bit
		else if(btn == 0x02) 							// btn 1 is pressed
			cmd->prd = pio_read(sw_base) & 0x000003ff; 	// load new inteval
		btn_clear(btn_base); 							// clear edge capture reg
	}
}

/*********************************************************************
 * funtion: jtaguart_disp_msg_v2
 * purpose: display time interval when it is changed
 * argument:
 * 		command type
 * return:
 * note:
 ********************************************************************/
void jtaguart_disp_msg_v2(cmd_type cmd){
	static int old = 0;					// current interval

	if(cmd.prd != old){ 						// new interval detected
		printf("Interval: %03d ms\n", cmd.prd);
		old = cmd.prd;							// update current interval
	}
}

/*****************************************************************************
 * funtion: sseg_disp_msg_v1
 * purpose: display pause state and time interval on 4-digit 7 segment
 * arguments:
 * 		sseg_base:
 * 		cmd:
 * return:
 * note:
 * 		time interval > 999, display 999
 *****************************************************************************/
void sseg_disp_msg_v1(alt_u32 sseg_base, cmd_type cmd){
	int pd;
	alt_u8 hex, msg[4];

	if(cmd.prd > 999)
		pd = 999;		// display max is 999
	else
		pd= cmd.prd;

	hex = pd%10;
	msg[3] = sseg_conv_hex(hex);
	hex = (pd/10)%10;
	msg[2] = sseg_conv_hex(hex);
	hex = (pd/100)%10;
	msg[1] = sseg_conv_hex(hex);
	if(cmd.pause)
		msg[0] = 0x0c; // P pattern
	else
		msg[0] = 0xff; // blank

	sseg_disp_ptn(sseg_base, msg); // display whole pattern
}

/*****************************************************************************
 * funtion: led_flash_v2
 * purpose: toggle 2 led
 * arguments:
 * 		ledr_base:
 * 		cmd:
 * return:
 * note:
 *****************************************************************************/
void led_flash_v2(alt_u32 ledr_base, cmd_type cmd){
	static alt_u8 led_ptn = 0x01;

	if(cmd.pause)								// no toggle if pause is asserted
		return;

	led_ptn = led_ptn ^ 0x03;					// toggle two led;
	pio_write(ledr_base, led_ptn);				// write leds
	usleep(1000*cmd.prd);

}

void led_flash_v3(alt_u32 ledr_base, cmd_type cmd){
	static alt_u8 led_ptn = 0x01;
	static int last = 0;
	int now;

	if(cmd.pause)								// no toggle if pause is asserted
		return;
	/* *****************************************************************************
	 * thay thế cho hàm delay usleep(); khi chưa hết khoảng thời gian chương trình
	 * sẽ không phải đợi trong vòng lặp
	 * *****************************************************************************/
	now = (int) (alt_nticks() *  alt_ticks_per_second() / 1000);
	if((now - last) < cmd.prd)
		return;
	last = now;

	led_ptn = led_ptn ^ 0x03;					// toggle two led;
	pio_write(ledr_base, led_ptn);				// write leds
}

int main(){
	cmd_type cmd = {0, 100}; // Initial value; not pause; 100ms interval

	flashsys_init_v2(BTN_BASE);
	while(1){
		sw_get_command_v1(BTN_BASE, SWITCH_BASE, &cmd);
		jtaguart_disp_msg_v2(cmd);
		sseg_disp_msg_v1(SSEG_BASE, cmd);
		led_flash_v3(LEDR_BASE, cmd);
	}
}
