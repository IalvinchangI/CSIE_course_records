
module Counter(clock_div, reset, count);

	input clock_div, reset;
	output [2:0] count;
	
	reg [2:0] count;
	
	
	always @(posedge clock_div or negedge reset)
	begin
		
		if (!reset)  // reset == 0
		begin
			count <= 3'd0;	
		end
		
		else  // reset == 1
		begin
			count <= count + 3'd1;  // ++
		end
		
	end

endmodule
