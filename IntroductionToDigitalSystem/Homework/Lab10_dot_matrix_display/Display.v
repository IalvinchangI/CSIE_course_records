
module Display(clock_div, reset, dot_row, dot_col);
	
	input clock_div, reset;
	output reg [7:0] dot_row;
	output reg [7:0] dot_col;
	
	reg [2:0] state;
	
	
	always @(posedge clock_div or negedge reset) begin
		
		if (~reset) begin
			dot_row <= 8'd0;
			dot_col <= 8'd0;
		end
		else begin
			
			case(state)
				3'd0: begin
					dot_row <= 8'b01111111;
					dot_col <= 8'b00011000;
				end
		
				3'd1: begin
					dot_row <= 8'b10111111;
					dot_col <= 8'b00100100;
				end
		
				3'd2: begin
					dot_row <= 8'b11011111;
					dot_col <= 8'b01000010;
				end
		
				3'd3: begin
					dot_row <= 8'b11101111;
					dot_col <= 8'b11000011;
				end
		
				3'd4: begin
//					dot_row <= 8'b11110111;
					dot_row <= 8'b11110001;
					dot_col <= 8'b01000010;
				end
		
				3'd5: begin
//					dot_row <= 8'b11111011;
					dot_row <= 8'b11110001;
					dot_col <= 8'b01000010;
				end
		
				3'd6: begin
//					dot_row <= 8'b11111101;
					dot_row <= 8'b11110001;
					dot_col <= 8'b01000010;
				end
		
				3'd7: begin
					dot_row <= 8'b11111110;
					dot_col <= 8'b01111110;
				end
			endcase
		
			state <= state + 1;
		
		end
		
	end
	
endmodule
