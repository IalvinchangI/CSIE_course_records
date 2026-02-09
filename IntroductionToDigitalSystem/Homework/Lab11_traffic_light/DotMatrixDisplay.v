
module DotMatrixDisplay(clock_div, light, dot_row, dot_col);
	
	input clock_div;
	input [1:0] light;
	output reg [7:0] dot_row;
	output reg [7:0] dot_col;
	
	reg [2:0] state;
	
	
	always @(posedge clock_div) begin
	
		case(state)  // row
			3'd0: dot_row <= 8'b01111111;
			3'd1: dot_row <= 8'b10111111;
			3'd2: dot_row <= 8'b11011111;
			3'd3: dot_row <= 8'b11101111;
			3'd4: dot_row <= 8'b11110111;
			3'd5: dot_row <= 8'b11111011;
			3'd6: dot_row <= 8'b11111101;
			3'd7: dot_row <= 8'b11111110;
		endcase
		
		case(light)  // col
			2'b01: begin
			case(state)  // G
				3'd0: dot_col <= 8'b00001100;
				3'd1: dot_col <= 8'b00001100;
				3'd2: dot_col <= 8'b00011001;
				3'd3: dot_col <= 8'b01111110;
				3'd4: dot_col <= 8'b10011000;
				3'd5: dot_col <= 8'b00011000;
				3'd6: dot_col <= 8'b00101000;
				3'd7: dot_col <= 8'b01001000;
			endcase
			end
			
			2'b10: begin
			case(state)  // Y
				3'd0: dot_col <= 8'b00000000;
				3'd1: dot_col <= 8'b00100100;
				3'd2: dot_col <= 8'b00111100;
				3'd3: dot_col <= 8'b10111101;
				3'd4: dot_col <= 8'b11111111;
				3'd5: dot_col <= 8'b00111100;
				3'd6: dot_col <= 8'b00111100;
				3'd7: dot_col <= 8'b00000000;
			endcase
			end
			
			2'b11: begin
			case(state)  // R
				3'd0: dot_col <= 8'b00011000;
				3'd1: dot_col <= 8'b00011000;
				3'd2: dot_col <= 8'b00111100;
				3'd3: dot_col <= 8'b00111100;
				3'd4: dot_col <= 8'b01011010;
				3'd5: dot_col <= 8'b00011000;
				3'd6: dot_col <= 8'b00011000;
				3'd7: dot_col <= 8'b00100100;
			endcase
			end
			
			default: begin
				dot_row <= 8'd0;
				dot_col <= 8'd0;
			end
		endcase
	
		state <= state + 1;
		
	end
	
endmodule
