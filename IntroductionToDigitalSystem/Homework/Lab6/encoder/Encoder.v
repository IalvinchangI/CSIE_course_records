
module Encoder(in, valid, out);
	input [7:0] in;
	output reg valid;
	output reg [2:0] out;
	
	always @(in)
	begin
		case (in)
		
			8'b00000001:  // 0
			begin
				out = 3'b000;
				valid = 1'b1;
			end
			
			8'b00000010:  // 1
			begin
				out = 3'b001;
				valid = 1'b1;
			end
			
			8'b00000100:  // 2
			begin
				out = 3'b010;
				valid = 1'b1;
			end
			
			8'b00001000:  // 3
			begin
				out = 3'b011;
				valid = 1'b1;
			end
			
			8'b00010000:  // 4
			begin
				out = 3'b100;
				valid = 1'b1;
			end
			
			8'b00100000:  // 5
			begin
				out = 3'b101;
				valid = 1'b1;
			end
			
			8'b01000000:  // 6
			begin
				out = 3'b110;
				valid = 1'b1;
			end
			
			8'b10000000:  // 7
			begin
				out = 3'b111;
				valid = 1'b1;
			end
			
			
			default:  // else
			begin
				out = 3'b000;
				valid = 1'b0;
			end
		
		endcase
	end
endmodule

