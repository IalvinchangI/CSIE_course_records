module  BU(
    input              clk, 
    input              rst, 
    input       [22:0] X,
    input       [22:0] Y,
    input       [22:0] TF,
    output      [22:0] A,
    output      [22:0] B
);
parameter Q = 23'd8380417;


// stage 1
wire [45:0] xy;
assign xy = Y * TF;

// register
reg [45:0] xy_reg;
reg [22:0] x_reg;
always @(posedge clk or posedge rst) begin
    if (rst == 1) begin
        xy_reg <= 0;
        x_reg <= 0;
    end
    else begin
        xy_reg <= xy;
        x_reg <= X;
    end
end

// stage 2
wire [22:0] mod_xy;
wire [23:0] a_xy;
wire [23:0] temp_a_xy;
wire        temp_a_sign;
wire [22:0] b_xy;
wire        b_sign;
Mod #(.Q(Q)) mod(.U(xy_reg), .Z(mod_xy));

assign a_xy = x_reg + mod_xy;
assign {temp_a_sign, temp_a_xy} = a_xy - Q;
assign {b_sign, b_xy} = x_reg - mod_xy;

assign A = (temp_a_sign == 1) ? a_xy : temp_a_xy;
assign B = (b_sign == 1) ? b_xy + Q : b_xy;

endmodule





module  Mod (
    input  [45:0] U,
    output [22:0] Z
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

    assign Z = (inverse_sign == 1'd1) ? P[22:0] : R[22:0];
endmodule
