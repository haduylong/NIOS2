/* inclusion section */
/* General C library */
#include "stdio.h"
/* Altera specific library */
#include "sys/alt_irq.h"
#include "alt_types.h"
#include "system.h"
/* Module specific library */
#include "gpio_drv.h"
#include "timer_drv.h"

/****************************************************************
 * data type definitions
 ****************************************************************/
typedef struct flash_cmd{
	int pause;
	int prd;
} cmd_type;

typedef struct ctxts {
	cmd_type *cmd_ptr;
	alt_u32 led_base;
	alt_u32 timer_base;

} ctext1_type;
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
 * funtion: led_flash_isr
 * purpose: toggle 2 led; each isr check if tick >= prd, toggle led
 * arguments:
 * 		ctxt: isr for flash led
 * 		id:
 * return:
 * note:
 * 		cmd passed within context
 *
 *****************************************************************************/
static void led_flash_isr(void* ctxt, alt_u32 id){
	ctext1_type* ctxt1;
	cmd_type *cmd;
	static int tick = 0;
	static alt_u8 led_ptn = 0x01;

	/* type casting */
	ctxt1 = (ctext1_type *) ctxt;
	cmd = ctxt1->cmd_ptr;

	if(cmd->pause)
		return;
	/* clear "to" field; also enable future interrupt */
	timer_clear_tick(ctxt1->timer_base);
	if(tick < cmd->prd){
		tick++;
	}else{
		tick=0;
		led_ptn = led_ptn ^ 0x03;	// invert two led
		pio_write(ctxt1->led_base, led_ptn);
	}
}

int main(){
	cmd_type cmd = {0, 100}; // not pause; 100ms interval
	ctext1_type ctxt1;

	ctxt1.cmd_ptr  = &cmd;
	ctxt1.led_base = LEDR_BASE;
	ctxt1.timer_base = USR_TIMER_BASE;
	flashsys_init_v1(BTN_BASE, USR_TIMER_BASE);

	// alt_irq_register(USR_TIMER_IRQ, (void *) &ctxt1, led_flash_isr); // legacy
	alt_ic_isr_register(USR_TIMER_IRQ_INTERRUPT_CONTROLLER_ID,
						USR_TIMER_IRQ, (void *) led_flash_isr,
						(void *) &ctxt1, 0x0); // enhanced
	while(1){
		sw_get_command_v1(BTN_BASE, SWITCH_BASE, &cmd);
		jtaguart_disp_msg_v2(cmd);
		sseg_disp_msg_v1(SSEG_BASE, cmd);
	}
}
