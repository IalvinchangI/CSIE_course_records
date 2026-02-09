
module VGA_Adjustable(clock, reset, keypadCol, keypadRow, VGA_R, VGA_G, VGA_B, VGA_HS, VGA_VS);
	
	input clock;				// MAX10_CLK1_50
	input reset;				// KEY0
	input [3:0] keypadCol;	// keypad col
	output [3:0] keypadRow;	// keypad row
	output [3:0] VGA_R;		// VGA red
	output [3:0] VGA_G;		// VGA green
	output [3:0] VGA_B;		// VGA blue
	output VGA_HS;				// VGA HSync
	output VGA_VS;				// VGA VSync
	
	wire pixel_clock;
	
	FrequencyDivider #(.TimeExpire(32'd1)) PixelClockFD (clock, pixel_clock);
	VGADisplay Display(pixel_clock, reset, VGA_R, VGA_G, VGA_B, VGA_HS, VGA_VS);
	
	assign keypadRow = 4'b1000;
	
	
endmodule
