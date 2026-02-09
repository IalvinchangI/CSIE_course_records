

module FullAdder_Behavioral(a, b, c_in, sum, c_out);
	
	input a, b, c_in; 
	output reg sum, c_out;
	
	always @(a, b, c_in)
	begin
		sum = ((a ^ b) ^ c_in);
		c_out = ((a & b) | ((a ^ b) & c_in));
	end
	
endmodule
