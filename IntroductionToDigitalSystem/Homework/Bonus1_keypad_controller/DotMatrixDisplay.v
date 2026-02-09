
module DotMatrixDisplay(clock_div, control, dot_row, dot_col);
	
	input clock_div;
	input [4:0] control;
	output reg [7:0] dot_row;
	output reg [7:0] dot_col;
	
	always @(posedge clock_div) begin
		
		if (control[4] == 1) begin  // 1
		
			case(control[1:0])  // row
				2'd0: dot_row <= 8'b00111111;
				2'd1: dot_row <= 8'b11001111;
				2'd2: dot_row <= 8'b11110011;
				2'd3: dot_row <= 8'b11111100;
			endcase

			case(control[3:2])  // col
				2'd0: dot_col <= 8'b11000000;
				2'd1: dot_col <= 8'b00110000;
				2'd2: dot_col <= 8'b00001100;
				2'd3: dot_col <= 8'b00000011;
			endcase
		
		end
		else begin
			dot_row <= 8'b11111111;
			dot_col <= 8'b00000000;
		end
		
	end
	
endmodule
