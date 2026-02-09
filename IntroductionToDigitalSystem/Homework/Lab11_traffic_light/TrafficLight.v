
module TrafficLight(clock, Reset, dot_row, dot_col, out);
	
	input clock;				// MAX10_CLK1_50
	input Reset;				// KEY0
	output [7:0] dot_row;	// DotMatrix row
	output [7:0] dot_col;	// DotMatrix col
	output [6:0] out;			// HEX06 ~ HEX00
	
	
	// Display Frequency
	wire display_frequency;
	FrequencyDivider FD(clock, display_frequency);
	
	// Count Frequency
	wire count_frequency;
	FrequencyDivider #(.TimeExpire(32'd25000000)) FD_LOW(clock, count_frequency);
	
	
	// Control
	wire [1:0] light;
	wire [3:0] count;
	Controller CTRL(count_frequency, Reset, light, count);
	
	
	// Display
	SevenDisplay SD(count, out);
	DotMatrixDisplay DMD(display_frequency, light, dot_row, dot_col);

endmodule
