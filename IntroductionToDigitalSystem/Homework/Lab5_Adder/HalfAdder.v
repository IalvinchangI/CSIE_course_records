

module HalfAdder(
	input a, 
	input b, 
	output sum, 
	output carry
);
	
	xor xo1(sum, a, b);
	and an1(carry, a, b);
endmodule
	
	
	