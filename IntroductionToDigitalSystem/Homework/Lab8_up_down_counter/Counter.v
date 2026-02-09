
module Counter(clock_div, reset, Up_Down, count);

	input clock_div, reset, Up_Down;
	output [3:0] count;
	
	reg [3:0] count;
	
	
	always @(posedge clock_div or negedge reset)
	begin
		
		if (!reset)  // reset == 0
		begin
			count <= 4'd0;	
		end
		
		else  // reset == 1
		begin
			
			if (Up_Down == 1'd1)  // ++
			begin
				count <= count + 4'd1;
			end
			else  // --
			begin
				count <= count - 4'd1;
			end
			
		end
		
	end

endmodule
