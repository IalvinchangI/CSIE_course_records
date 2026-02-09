

module FullAdder(a, b, c_in, sum, c_out);

	input a, b, c_in; 
	output sum, c_out;
	
	wire w1, w2, w3;
	
	HalfAdder ha1(a, b, w1, w2);
	HalfAdder ha2(a, c_in, sum, w3);
	
	or o1(c_out, w2, w3);

endmodule
	