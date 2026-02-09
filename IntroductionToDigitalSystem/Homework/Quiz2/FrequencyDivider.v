
`define TimeExpire 24'd12500000


module FrequencyDivider(clock, reset, clock_div);

	input clock, reset;
	output reg clock_div;
	
	reg [23:0] count;
	
	
	always @(posedge clock or negedge reset)
	begin
		
		if (!reset)  // reset == 0
		begin
			clock_div <= 1'b0;
			count <= 24'd0;  // reset to 0
		end
		
		else  // reset == 1
		begin
			
			if (count == `TimeExpire)  // count reach TimeExpire
			begin
				clock_div <= ~clock_div;  // flip output
				count <= 24'd0;  // reset to 0
			end
			
			else  // count does not reach TimeExpire
			begin
				count <= count + 24'd1;
			end
			
		end
		
	end

endmodule
