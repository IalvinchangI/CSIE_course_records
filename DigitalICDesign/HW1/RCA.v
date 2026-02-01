`include "FA.v"

module RCA(
	input  [3:0]   x,
	input  [3:0]   y,
	input 		c_in,
	output [3:0]   s,
	output     c_out
);

wire [2:0] inner_c;

FA fa0(.x(x[0]), .y(y[0]), .c_in(c_in), .s(s[0]), .c_out(inner_c[0]));
FA fa1(.x(x[1]), .y(y[1]), .c_in(inner_c[0]), .s(s[1]), .c_out(inner_c[1]));
FA fa2(.x(x[2]), .y(y[2]), .c_in(inner_c[1]), .s(s[2]), .c_out(inner_c[2]));
FA fa3(.x(x[3]), .y(y[3]), .c_in(inner_c[2]), .s(s[3]), .c_out(c_out));

endmodule
