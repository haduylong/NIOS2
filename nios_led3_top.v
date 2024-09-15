module nios_led3_top
	(
	 input clk, // reset_n = 1'b1
	 input [9:0] sw,
	 input [1:0] btn,
	 output [1:0] ledr,
	 output [7:0] hex3, hex2, hex1, hex0
	);

	// signal declaration
	wire [31:0] sseg;
	// instance ip
	nios_led3 nios_led3_unit
	(
	 .clk_clk(clk),
	 .reset_reset_n(1'b1),
	 .i_btn_export(btn),
	 .i_switch_export(sw),
	 .o_ledr_export(ledr),
	 .o_sseg_export(sseg)
	);
	
	assign hex3 = sseg[30:24];
	assign hex2 = sseg[22:16];
	assign hex1 = sseg[14:8];
	assign hex0 = sseg[6:0];
endmodule