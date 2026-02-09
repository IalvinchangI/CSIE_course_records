
module KeyPadDetector(clock_div, reset, keypad_col, keypad_row, press);
	
	input clock_div;
	input reset;
	input [3:0] keypad_col;
	output reg [3:0] keypad_row;
	output reg [4:0] press;
	
	
	always @(posedge clock_div) begin
		
		if (~reset) press <= 5'd0;
		else begin
		
			case(keypad_row)
				4'b0111: keypad_row <= 4'b1011;
				4'b1011: keypad_row <= 4'b1101;
				4'b1101: keypad_row <= 4'b1110;
				4'b1110: keypad_row <= 4'b0111;
				default: keypad_row <= 4'b0111;  // impossible
			endcase
			
			case({keypad_col, keypad_row})
				8'b0111_0111: press <= 5'b10000;
				8'b0111_1011: press <= 5'b10001;
				8'b0111_1101: press <= 5'b10010;
				8'b0111_1110: press <= 5'b10011;
				
				8'b1011_0111: press <= 5'b10100;
				8'b1011_1011: press <= 5'b10101;
				8'b1011_1101: press <= 5'b10110;
				8'b1011_1110: press <= 5'b10111;
				
				8'b1101_0111: press <= 5'b11000;
				8'b1101_1011: press <= 5'b11001;
				8'b1101_1101: press <= 5'b11010;
				8'b1101_1110: press <= 5'b11011;
				
				8'b1110_0111: press <= 5'b11100;
				8'b1110_1011: press <= 5'b11101;
				8'b1110_1101: press <= 5'b11110;
				8'b1110_1110: press <= 5'b11111;
				
				default: press <= press;
			endcase
		
		end
		
	end

endmodule
