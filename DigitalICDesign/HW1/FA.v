`include "HA.v"

module FA(
	input 	   x,
	input 	   y,
	input 	c_in,
	output     s, 
	output  c_out
);

wire inner_s, inner1_c, inner2_c;

HA ha1(.x(x), .y(y), .s(inner_s), .c(inner1_c));
HA ha2(.x(c_in), .y(inner_s), .s(s), .c(inner2_c));
assign c_out = inner1_c | inner2_c;

endmodule

