
module Machine(clock_div, reset, In, out);

	input clock_div, reset, In;
	output [3:0] out;
	
	reg [3:0] out;
	reg [2:0] state;
	
	
	always @(posedge clock_div or negedge reset)
	begin
		
		if (!reset)  // reset == 0
		begin
			out <= 4'd0;
			state <= 3'd0;
		end
		
		else  // reset == 1
		begin
			
			out <= state;
			
			if (In == 1'd0)  // current += 1
			begin
			
				case (state)
					
					3'd0:
					begin
						state = 3'd1;
					end
					
					3'd1:
					begin
						state = 3'd2;
					end
					
					3'd2:
					begin
						state = 3'd3;
					end
					
					3'd3:
					begin
						state = 3'd4;
					end
					
					3'd4:
					begin
						state = 3'd5;
					end
					
					default:  // 3'd5
					begin
						state = 3'd0;
					end
				
				endcase
				
			end
			else  // random
			begin
				
				case (state)
					
					3'd0:
					begin
						state = 3'd3;
					end
					
					3'd1:
					begin
						state = 3'd5;
					end
					
					3'd2:
					begin
						state = 3'd0;
					end
					
					3'd3:
					begin
						state = 3'd1;
					end
					
					3'd4:
					begin
						state = 3'd2;
					end
					
					default:  // 3'd5
					begin
						state = 3'd4;
					end
				
				endcase
				
			end
			
		end
		
	end

endmodule
