
module FullAdder(a, b, carry_in, sum, carry_out);
	
	input a, b, carry_in;
	output sum, carry_out;
	
	wire [2:0] temp;
	
	HalfAdder HA1(a, b, temp[0], temp[1]);
	HalfAdder HA2(carry_in, temp[0], sum, temp[2]);
	assign carry_out = temp[1] | temp[2];
	
endmodule
