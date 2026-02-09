
module SevenDisplay(in, out);
	
	input [3:0] in;
	output reg [6:0] out;
	
	always @(in)
	begin
		case(in)
			
			4'h0:  // 0
			begin
				out = 7'b1000000;
			end
			
			4'h1:  // 1
			begin
				out = 7'b1111001;
			end
			
			4'h2:  // 2
			begin
				out = 7'b0100100;
			end
			
			4'h3:  // 3
			begin
				out = 7'b0110000;
			end
			
			4'h4:  // 4
			begin
				out = 7'b0011001;
			end
			
			4'h5:  // 5
			begin
				out = 7'b0010010;
			end
			
			4'b0110:  // 6
//			4'h6:  // 6
			begin
				out = 7'b0000010;
			end
			
			4'h7:  // 7
			begin
				out = 7'b1111000;
			end
			
			4'h8:  // 8
			begin
				out = 7'b0000000;
			end
			
			4'h9:  // 9
			begin
				out = 7'b0010000;
			end
			
			4'hA:  // A  10
			begin
				out = 7'b0001000;
			end
			
			4'b1011:  // B  11
//			4'hB:  // B  11
			begin
				out = 7'b0000011;
			end
			
			4'hC:  // C  12
			begin
				out = 7'b1000110;
			end
			
			4'hD:  // D  13
			begin
				out = 7'b0100001;
			end
			
			4'hE:  // E  14
			begin
				out = 7'b0000110;
			end
			
			4'hF:  // F  15
			begin
				out = 7'b0001110;
			end
			
		endcase
	end
	
endmodule
