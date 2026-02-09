

module FullAdder_DataFlow(a, b, c_in, sum, c_out);

	input a, b, c_in; 
	output sum, c_out;
	
	assign sum = ((a ^ b) ^ c_in);
	assign c_out = ((a & b) | ((a ^ b) & c_in));

endmodule