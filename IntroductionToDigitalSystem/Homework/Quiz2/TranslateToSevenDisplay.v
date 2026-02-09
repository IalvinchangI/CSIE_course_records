
module TranslateToSevenDisplay(in, out);
	
	input [2:0] in;
	output reg [6:0] out;
	
	always @(in)
	begin
		case(in)
			
			3'b000:  // 0 -> 0
			begin
				out = 7'b1000000;
			end
			
			3'b001:  // 1 -> 9
			begin
				out = 7'b0010000;
			end
			
			3'b010:  // 2 -> 2
			begin
				out = 7'b0100100;
			end
			
			3'b011:  // 3 -> 8
			begin
				out = 7'b0000000;
			end
			
			3'b100:  // 4 -> A
			begin
				out = 7'b0001000;
			end
			
			3'b101:  // 5 -> 1
			begin
				out = 7'b1111001;
			end
			
			3'b110:  // 6 -> 7
			begin
				out = 7'b1111000;
			end
			
			3'b111:  // 7 -> 6
			begin
				out = 7'b0000010;
			end
			
		endcase
	end
	
endmodule
