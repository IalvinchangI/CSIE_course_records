
module MooreMachine(clock, reset, In, out);
	
	input clock;    		// MAX10_CLK1_50
	input reset;    		// SW0
	input In;  				// SW1
	output [6:0] out;    // HEX06 ~ HEX00
	
	wire clock_div;
	wire [3:0] tmp;
	
	
	FrequencyDivider			divider(.clock(clock), .reset(reset), .clock_div(clock_div));
	Machine						counter(.clock_div(clock_div), .reset(reset), .In(In), .out(tmp));
	TranslateToSevenDisplay	display(.in(tmp), .out(out));


endmodule
