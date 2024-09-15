
module nios_led3 (
	clk_clk,
	i_btn_export,
	i_switch_export,
	o_ledr_export,
	o_sseg_export,
	reset_reset_n);	

	input		clk_clk;
	input	[1:0]	i_btn_export;
	input	[9:0]	i_switch_export;
	output	[1:0]	o_ledr_export;
	output	[31:0]	o_sseg_export;
	input		reset_reset_n;
endmodule
