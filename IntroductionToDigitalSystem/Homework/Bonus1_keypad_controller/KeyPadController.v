
module KeyPadController(clock, reset, keypadCol, keypadRow, dot_row, dot_col);
	
	input clock;				// MAX10_CLK1_50
	input reset;				// KEY0
	input [3:0] keypadCol;	// keypad col
	output [3:0] keypadRow;	// keypad row
	output [7:0] dot_row;	// DotMatrix row
	output [7:0] dot_col;	// DotMatrix col
	
	
	// Display Frequency
	wire display_frequency;
	FrequencyDivider FD(clock, display_frequency);
	
	// Detect Frequency
	wire detect_frequency;
	FrequencyDivider #(.TimeExpire(32'd250000)) FD_LOW(clock, detect_frequency);
	
	
	// Detect
	wire [4:0] control_signal;
	KeyPadDetector KPD(detect_frequency, reset, keypadCol, keypadRow, control_signal);
	
	// Display
	DotMatrixDisplay DMD(display_frequency, control_signal, dot_row, dot_col);
	
endmodule
