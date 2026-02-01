module Mul_Mod (
    input  [22:0] A,
    input  [22:0] B,
    output [23:0] Z
);
parameter Q = 24'd8380417;


// mul part //
wire [46:0] U;
wire [28:0] U_top;
wire [40:0] U_bottom;

assign U_bottom = A * B[16:0];
assign U_top = A * B[22:17];
RCA #(.N(46)) mul_part_adder (
	.x({5'd0, U_bottom}), .y({U_top, 17'd0}), .c_in(1'd0), 
	.s(U[45:0]), .c_out(U[46])
);

Mod mod(.U(U[45:0]), .Z(Z));


endmodule



module  Mod (
    input  [45:0] U,
    output [23:0] Z
);
parameter Q = 23'd8380417;

// middle before add //
wire [23:0] V;
wire [24:0] V_before_concat;
wire [34:0] V_after_concat;
wire [26:0] V_between_add [1:0];
wire [35:0] V_after_add;
wire [23:0] W;

assign V = U[45:22];

// concat
assign {V_before_concat[24], V_before_concat[23:0]} = V + {10'd0, V[23:10]};
// RCA #(.N(24)) concat_part_adder(
// 	.x(V), .y({10'd0, V[23:10]}), .c_in(1'd0), 
// 	.s(V_before_concat[23:0]), .c_out(V_before_concat[24])
// );
assign V_after_concat = {V_before_concat, V[9:0]};

// 3 input adder (3Iadder)
assign {V_between_add[0][25], V_between_add[0][24:0]} = {V, 1'd0} + {1'd0, V[23:0]};
// RCA #(.N(25)) first_part_3Iadder(
// 	.x({V, 1'd0}), .y({1'd0, V[23:0]}), .c_in(1'd0), 
// 	.s(V_between_add[0][24:0]), .c_out(V_between_add[0][25])
// );
assign V_between_add[1][25:0] = V_between_add[0][25:0] + {3'd0, V[23:1]};
// RCA #(.N(26)) second_part_3Iadder(
// 	.x(V_between_add[0][25:0]), .y({3'd0, V[23:1]}), .c_in(1'd0), 
// 	.s(V_between_add[1][25:0]), .c_out(V_between_add[1][26])
// );

// middle adder
assign V_after_add[34:0] = {21'd0, V_between_add[1][25:12]} + V_after_concat;
// RCA #(.N(35)) middle_part_adder(
// 	.x({21'd0, V_between_add[1][25:12]}), .y(V_after_concat), .c_in(1'd0), 
// 	.s(V_after_add[34:0]), .c_out(V_after_add[35])
// );
assign W = V_after_add[34:11];


// middle after add //
wire [11:0] W_after_sub;
wire [23:0] X;
assign W_after_sub[10:0] = W[23:13] - W[10:0];
// RCA #(.N(11)) middle_part_subtractor(
// 	.x(W[23:13]), .y(~(W[10:0])), .c_in(1'd1), 
// 	.s(W_after_sub[10:0]), .c_out(W_after_sub[11])
// );
assign X = {W[0] ^ W_after_sub[10], W_after_sub[9:0], W[12:0]};


// final part //
wire [24:0] P;
wire [23:0] R;
wire inverse_sign;
assign P[23:0] = U[23:0] - X;
// RCA #(.N(24)) final_part_subtractor(
// 	.x(U[23:0]), .y(~X), .c_in(1'd1), 
// 	.s(P[23:0]), .c_out(P[24])
// );
assign {inverse_sign, R} = P[23:0] - Q;
// RCA #(.N(24)) mod_part_subtractor(
// 	.x(P[23:0]), .y(~Q), .c_in(1'd1), 
// 	.s(R), .c_out(inverse_sign)
// );

assign Z[22:0] = (inverse_sign == 1'd1) ? P[22:0] : R[22:0];
assign Z[23] = 0;

endmodule


module RCA #(parameter N = 4) (
	input  [N-1:0]   x,
	input  [N-1:0]   y,
	input 			 c_in,
	output [N-1:0]   s,
	output     	 	 c_out
);

wire [N-1:0] input_c;
wire [N-1:0] output_c;

assign input_c[0] = c_in;
assign input_c[N-1:1] = output_c[N-2:0];
assign c_out = output_c[N-1];

generate
	genvar i;
	for (i = 0; i < N; i = i + 1) begin: generate_N_full_adder
		FA fa(.x(x[i]), .y(y[i]), .c_in(input_c[i]), .s(s[i]), .c_out(output_c[i]));
	end
endgenerate

endmodule

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

module HA(
	input  x,
	input  y,
	output s, 
	output c
);

assign s = x ^ y;
assign c = x & y;

endmodule

