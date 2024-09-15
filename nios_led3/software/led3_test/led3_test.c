#include "system.h"
#include "gpio_drv.h"
#include "uart_drv.h"
#include "timer_drv.h"

typedef struct flash_cmd{
	int pause;
	int prd;
} cmd_type;

/*****************************************************************************
 * funtion: flashsys_init_v1
 * purpose: Init system, clear capture push button,  set 1-ms timeout period
 * argument:
 * 		pushbutton base address
 * 		timer base address
 * return:
 * note:
 *****************************************************************************/
void flashsys_init_v1(alt_u32 btn_base, alt_32 timer_base){
	btn_clear(btn_base);				// clear edge capture button
	timer_write_prd(timer_base, 50000); // set 1-ms timeout period
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
 * funtion: jtaguart_disp_msg_v1
 * purpose: display time interval when it is changed
 * argument:
 * 		JTAG_UART base address
 * 		command type
 * return:
 * note:
 ********************************************************************/
void jtaguart_disp_msg_v1(alt_u32 uart_base, cmd_type cmd){
	static int current = 0;					// current interval
	char msg[] = "Interval: 0000ms\n";

	if(cmd.prd != current){ 						// new interval detected
		msg[13] = (cmd.prd % 10) + '0';
		msg[12] = (cmd.prd/10) % 10 + '0';
		msg[11] = (cmd.prd/100) % 10 + '0';
		msg[10] = (cmd.prd/1000) % 10 + '0';
		jtag_wr_str(uart_base, msg);				// send message to console
		current = cmd.prd;							// update current interval
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
 * funtion: led_flash_v1
 * purpose: toggle 2 led
 * arguments:
 * 		ledr_base:
 * 		timer_base:
 * 		cmd:
 * return:
 * note:
 *****************************************************************************/
void led_flash_v1(alt_u32 ledr_base, alt_u32 timer_base,cmd_type cmd){
	static alt_u8 led_ptn = 0x01;
	int tick = 0;

	if(cmd.pause)								// no toggle if pause is asserted
		return;

	led_ptn = led_ptn ^ 0x03;					// toggle two led;
	pio_write(ledr_base, led_ptn);				// write leds
	while(tick < cmd.prd){ 						// loop prd times
		if(timer_read_tick(timer_base) == 1){ 	// check count up to 1 ms
			timer_clear_tick(timer_base);		// clear timer
			tick++;								// incease tick 1
		}
	}
}

int main(){
	cmd_type cmd = {0, 100}; // inital value; not pause; 100ms interval

	flashsys_init_v1(BTN_BASE, USR_TIMER_BASE);
	while(1){
		sw_get_command_v1(BTN_BASE, SWITCH_BASE, &cmd);
		jtaguart_disp_msg_v1(JTAG_UART_BASE, cmd);
		sseg_disp_msg_v1(SSEG_BASE, cmd);
		led_flash_v1(LEDR_BASE, USR_TIMER_BASE, cmd);
	}
}
