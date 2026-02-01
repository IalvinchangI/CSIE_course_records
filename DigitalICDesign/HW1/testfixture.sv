
`timescale 1ns / 1ps

`include "./HA.v"
`include "./FA.v"
`include "./RCA.v"

module testfixture;
integer total_errors;
integer ha_errors;
integer fa_errors;
integer rca_errors;

// HA
reg ha_x, ha_y;
wire ha_s, ha_c;
reg ha_expected_s, ha_expected_c;
integer i_ha,num_ha;

// FA
reg fa_a, fa_b, fa_cin;
wire fa_sum, fa_cout;
integer i_fa,num_fa;
reg [2:0] fa_input_bits;
reg fa_expected_sum, fa_expected_cout;

// RCA
reg [3:0] rca_x, rca_y;
wire [3:0] rca_s;
reg rca_cin;
wire rca_cout;
reg [3:0] rca_result;
reg rca_c;
integer i_rca, j_rca, num_rca;

HA ha(.x(ha_x), .y(ha_y), .s(ha_s), .c(ha_c));
FA fa(.x(fa_a), .y(fa_b), .c_in(fa_cin), .s(fa_sum), .c_out(fa_cout));
RCA rca(.s(rca_s), .c_out(rca_cout), .x(rca_x), .y(rca_y), .c_in(rca_cin));

initial begin
total_errors = 0;
ha_errors = 0;
fa_errors = 0;
rca_errors = 0;
num_rca = 0;
num_fa = 0;
num_ha = 0;

$display("=== Testing Half Adder ===");
for (i_ha = 0; i_ha < 8; i_ha = i_ha + 1) begin
    {ha_x, ha_y} = i_ha[1:0];
    #1 ha_expected_s = ha_x ^ ha_y;
        ha_expected_c = ha_x & ha_y;
    #1;
    if ((ha_s === ha_expected_s) && (ha_c === ha_expected_c))
        $display("PASS: #%0d data is correct", num_ha);
    else begin
        $display("FAIL: #%0d x=%b y=%b => s=%b c=%b (expected s=%b c=%b)", num_ha, ha_x, ha_y, ha_s, ha_c, ha_expected_s, ha_expected_c);
        ha_errors = ha_errors + 1;
    end
    num_ha = num_ha + 1;
end
if (ha_errors == 0)
    $display(">>> [HA] TEST PASSED\n");
else begin
    $display(">>> [HA] TEST FAILED");
    $display("HA errors: %0d\n", ha_errors);
end
total_errors = total_errors + ha_errors;


$display("=== Testing Full Adder ===");
for (i_fa = 0; i_fa < 8; i_fa = i_fa + 1) begin
    fa_input_bits = i_fa[2:0];
    {fa_a, fa_b, fa_cin} = fa_input_bits;
    #1 {fa_expected_cout, fa_expected_sum} = fa_a + fa_b + fa_cin;
    #1;
    if ((fa_sum === fa_expected_sum) && (fa_cout === fa_expected_cout))
        $display("PASS: #%0d data is correct", num_fa);
    else begin
        $display("FAIL: #%0d a=%b b=%b c_in=%b => sum=%b c_out=%b (expected sum=%b c_out=%b)", 
                num_fa,fa_a, fa_b, fa_cin, fa_sum, fa_cout, fa_expected_sum, fa_expected_cout);
        fa_errors = fa_errors + 1;
    end
    num_fa = num_fa + 1;
end
if (fa_errors == 0)
    $display(">>> [FA] TEST PASSED\n");
else begin
    $display(">>> [FA] TEST FAILED");
    $display("FA errors: %0d\n", fa_errors);
end
total_errors = total_errors + fa_errors;

$display("=== Testing Ripple Carry Adder ===");
for (i_rca = 0; i_rca < 32; i_rca = i_rca + 1) begin
    for (j_rca = 0; j_rca < 16; j_rca = j_rca + 1) begin
        #1 rca_x = i_rca[3:0];
            rca_y = j_rca;
            rca_cin = i_rca[4];
        #1 {rca_c, rca_result} = rca_x + rca_y + rca_cin;
        #1;
        if ((rca_c == rca_cout) && (rca_result == rca_s)) begin
            if (num_rca < 10)
                $display("PASS: #%0d data is correct", num_rca);
        end else begin
            if (num_rca < 10)
                $display("FAIL: #%0d x=%b y=%b c_in=%b => s=%b c_out=%b (expected s=%b c_out=%b)", 
                            num_rca, rca_x, rca_y, rca_cin, rca_s, rca_cout, rca_result, rca_c);
            rca_errors = rca_errors + 1;
        end
        num_rca = num_rca + 1;
    end
end

if (rca_errors == 0)
    $display(">>> [RCA] TEST PASSED\n");
else begin
    $display(">>> [RCA] TEST FAILED");
    $display("RCA errors: %0d\n", rca_errors);
end
total_errors = total_errors + rca_errors;

if (total_errors == 0) begin
    $display("\n===================== ALL MODULE TESTS PASSED =====================");
end else begin
    $display("\n===================== TOTAL ERRORS: %0d =====================", total_errors);
end

#10 $finish;
end

endmodule
