
module Quiz2(clock, reset, out);
	
	input clock;    		// MAX10_CLK1_50
	input reset;    		// SW0
	output [6:0] out;    // HEX06 ~ HEX00
	
	wire clock_div;
	wire [2:0] count;
	
	
	FrequencyDivider			divider(.clock(clock), .reset(reset), .clock_div(clock_div));
	Counter						counter(.clock_div(clock_div), .reset(reset), .count(count));
	TranslateToSevenDisplay	display(.in(count), .out(out));

endmodule
