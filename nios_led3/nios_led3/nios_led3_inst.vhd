	component nios_led3 is
		port (
			clk_clk         : in  std_logic                     := 'X';             -- clk
			i_btn_export    : in  std_logic_vector(1 downto 0)  := (others => 'X'); -- export
			i_switch_export : in  std_logic_vector(9 downto 0)  := (others => 'X'); -- export
			o_ledr_export   : out std_logic_vector(1 downto 0);                     -- export
			o_sseg_export   : out std_logic_vector(31 downto 0);                    -- export
			reset_reset_n   : in  std_logic                     := 'X'              -- reset_n
		);
	end component nios_led3;

	u0 : component nios_led3
		port map (
			clk_clk         => CONNECTED_TO_clk_clk,         --      clk.clk
			i_btn_export    => CONNECTED_TO_i_btn_export,    --    i_btn.export
			i_switch_export => CONNECTED_TO_i_switch_export, -- i_switch.export
			o_ledr_export   => CONNECTED_TO_o_ledr_export,   --   o_ledr.export
			o_sseg_export   => CONNECTED_TO_o_sseg_export,   --   o_sseg.export
			reset_reset_n   => CONNECTED_TO_reset_reset_n    --    reset.reset_n
		);

