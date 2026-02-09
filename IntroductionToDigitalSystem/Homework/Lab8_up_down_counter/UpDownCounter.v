
module UpDownCounter(clock, reset, Up_Down, out);
	
	input clock;    		// MAX10_CLK1_50
	input reset;    		// SW0
	input Up_Down;  		// SW1
	output [6:0] out;    // HEX06 ~ HEX00
	
	wire clock_div;
	wire [3:0] count;
	
	
	FrequencyDivider			divider(.clock(clock), .reset(reset), .clock_div(clock_div));
	Counter						counter(.clock_div(clock_div), .reset(reset), .Up_Down(Up_Down), .count(count));
	TranslateToSevenDisplay	display(.in(count), .out(out));

endmodule
