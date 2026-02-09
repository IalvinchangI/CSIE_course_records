
module calculator(in, out);
	
	input [3:0] in;
	output reg [3:0] out;
	
	always @(in)
	begin
		case(in)
			
			4'b0000, 4'b0001, 4'b0010:  // 0, 1, 2
			begin
				out = in;
			end
			
			4'b0011, 4'b0100, 4'b0101:  // 3, 4, 5
			begin
				out = (in << 1) + 1;
			end
			
			4'b0110 ,4'b0111, 4'b1000:  // 6, 7, 8
			begin
				out = (in << 1) - 1;
			end
			
			default:
			begin
				out = 0;
			end
			
		endcase
	end
	
endmodule
