
module DotMatrixDisplay(clock, reset, dot_row, dot_col, test);
	
	input clock;    		// MAX10_CLK1_50
	input reset;    		// SW0
	output [7:0] dot_row;// DotMatrix row
	output [7:0] dot_col;// DotMatrix col
	output reg test;		// LED0
	
	wire clock_div;
	reg [9:0] count;
	
	FrequencyDivider FD(clock, reset, clock_div);
	Display Dp(clock_div, reset, dot_row, dot_col);
	
	always @(posedge clock_div) begin		
		count <= count + 1;
		test <= count[9];
	end
	
endmodule
