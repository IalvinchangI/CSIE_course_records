
module KeyPadDetector(clock_div, keypad_col, keypad_row, press);
	
	input clock_div;
	input [3:0] keypad_col;
	output [3:0] keypad_row;
	output reg [1:0] press;  // 0: nothing; 1: R; 2: G; B: 3;
	
	
	assign keypad_row = 4'b1110;
	
	always @(posedge clock_div) begin
	
		case(keypad_col)
			4'b0111: press <= 2'd1;
			4'b1011: press <= 2'd2;
			4'b1101: press <= 2'd3;
			default: press <= 2'd0;
		endcase
		
	end

endmodule
