
module Controller(count_frequency, reset, light, count);
	
	input count_frequency, reset;
	output reg [1:0] light;
	output reg [3:0] count;
	
	always @(posedge count_frequency or negedge reset) begin
		
		if (~reset) begin
			count <= 4'd10;
			light <= 2'b01;
		end
		else begin
			
			if (count == 4'd0) begin
				case(light)
					
					2'b01: begin  // G to Y
						count <= 4'd3;
						light <= 2'b10;
					end
					2'b10: begin  // Y to R
						count <= 4'd15;
						light <= 2'b11;
					end
					2'b11: begin  // R to G
						count <= 4'd10;
						light <= 2'b01;
					end
					default: begin
						count <= 4'd10;
						light <= 2'b01;
					end
					
				endcase
			end
			else count <= count - 1;
			
		end
		
	end

endmodule
