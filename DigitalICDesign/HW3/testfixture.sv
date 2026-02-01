`timescale 1ns/10ps
`define CYCLE      10          	  // Modify your clock period here
`define End_CYCLE  100000000      // Modify cycle times once your design need more cycle times!

`define RGB        "./RGB.dat"    
`define Gray       "./gray.dat"     
`define LBP        "./LBP.dat"     


module testfixture;

parameter N   = 16384; // 128 x 128 pixel


reg   [23:0]   RGB_mem   [0:N-1];
reg   [7:0]   gray_mem    [0:N-1];
reg   [7:0]   lbp_mem    [0:N-1];

wire [7:0] gray_data;
wire [7:0] lbp_data;
reg   clk = 0;
reg   reset = 0;
reg   result_compare = 0;

integer err1 = 0;
integer err = 0;
integer times = 0;
reg over = 0;
reg over1 = 0;
integer gray_num = 0;
integer lbp_num = 0;
wire [13:0] RGB_addr;
wire [13:0] gray_addr;
wire [13:0] lbp_addr;
reg [23:0] RGB_data;
reg RGB_ready = 0;
integer i;

LBP LBP( .clk(clk), .reset(reset), 
        .RGB_addr(RGB_addr), .RGB_req(RGB_req), .RGB_ready(RGB_ready), .RGB_data(RGB_data), 
        .gray_addr(gray_addr), .gray_valid(gray_valid), .gray_data(gray_data),
        .lbp_addr(lbp_addr), .lbp_valid(lbp_valid), .lbp_data(lbp_data), .finish(finish));
        
lbp_mem u_lbp_mem(.lbp_valid(lbp_valid), .lbp_data(lbp_data), .lbp_addr(lbp_addr), .clk(clk));
gray_mem u_gray_mem(.gray_addr(gray_addr), .gray_valid(gray_valid), .gray_data(gray_data), .clk(clk));

initial	$readmemh (`RGB, RGB_mem);
initial	$readmemh (`Gray, gray_mem);
initial	$readmemh (`LBP, lbp_mem);

always begin #(`CYCLE/2) clk = ~clk; end

initial begin 
   @(negedge clk)  reset = 1'b1; 
   #(`CYCLE*2);    reset = 1'b0; 
   @(negedge clk)  RGB_ready = 1'b1;
    while (finish == 0) begin             
      if( RGB_req ) begin
         RGB_data = RGB_mem[RGB_addr];  
      end 
      else begin
         RGB_data = 'hz;  
      end                    
      @(negedge clk); 
    end     
    RGB_ready = 0; RGB_data='hz;
	@(posedge clk) result_compare = 1; 
end

initial begin
    // -------- Gray Compare --------
    $display("=====================================================");
    $display("                START GRAY SIMULATION                ");
    $display("=====================================================");
    #(`CYCLE*3); 
    wait(finish);
    @(posedge clk); @(posedge clk);

    for (i = 0; i < N; i = i + 1) begin
        if (gray_mem[i] !== u_gray_mem.gray_M[i]) begin
            if (err <= 10)
                $display("GRAY pixel %d FAIL !! your answer = %d, golden = %d",
                          i, u_gray_mem.gray_M[i], gray_mem[i]);
            err = err + 1;
            if (err <= 10) $display("GRAY Output pixel %d are wrong!", i);
            if (err == 11)
                $display("GRAY wrong pixels >10, please check code!\n");
        end

        if (((i % 1000) == 0) || (i == N-1)) begin
            if (err == 0)
                $display("GRAY Output pixel: 0 ~ %d are correct!\n", i);
            else
                $display("GRAY Output pixel: 0 ~ %d have %d errors!\n", i, err);
        end
        gray_num = gray_num + 1;
    end
    over = 1;  // Gray 結束
end

initial begin
    wait(over);
	    // -------- LBP Compare --------
    $display("=====================================================");
    $display("                 START LBP SIMULATION                ");
    $display("=====================================================");
    #(`CYCLE*1);
    wait(finish);
    @(posedge clk); @(posedge clk);

    for (i = 0; i < N; i = i + 1) begin
        if (lbp_mem[i] !== u_lbp_mem.LBP_M[i]) begin
            if (err1 <= 10)
                $display("LBP pixel %d FAIL !! your answer = %d, golden = %d",
                          i, u_lbp_mem.LBP_M[i], lbp_mem[i]);
            err1 = err1 + 1;
            if (err1 <= 10) $display("LBP Output pixel %d are wrong!", i);
            if (err1 == 11)
                $display("LBP wrong pixels >10, please check code!\n");
        end

        if (((i % 1000) == 0) || (i == N-1)) begin
            if (err1 == 0)
                $display("LBP Output pixel: 0 ~ %d are correct!\n", i);
            else
                $display("LBP Output pixel: 0 ~ %d have %d errors!\n", i, err1);
        end
        lbp_num = lbp_num + 1;
    end
    over1 = 1;
end


initial begin
    wait(over && over1);

    $display("-----------------------------------------------------\n");

    if ((over) && (gray_num != 'd0)) begin
        if (err == 0) begin
            $display("Congratulations! GRAY data have been generated successfully!\n");
            $display("-------------------------PASS------------------------\n");
        end else begin
            $display("There are %d errors in GRAY!\n", err);
            $display("-----------------------------------------------------\n");
        end
    end

    if ((over1) && (lbp_num != 'd0)) begin
        if (err1 == 0) begin
            $display("Congratulations! LBP data have been generated successfully!\n");
            $display("-------------------------PASS------------------------\n");
        end else begin
            $display("There are %d errors in LBP!\n", err1);
            $display("-----------------------------------------------------\n");
        end
    end

    #(`CYCLE/2);
    $finish;
end

initial  begin
 #`End_CYCLE ;
 	$display("-----------------------------------------------------\n");
 	$display("Error!!! Somethings' wrong with your code ...!\n");
 	$display("-------------------------FAIL------------------------\n");
 	$display("-----------------------------------------------------\n");
 	$finish;
end
   
endmodule


module lbp_mem (lbp_valid, lbp_data, lbp_addr, clk);
input		lbp_valid;
input	[13:0] 	lbp_addr;
input	[7:0]	lbp_data;
input		clk;

reg [7:0] LBP_M [0:16383];
integer i;

initial begin
	for (i=0; i<=16383; i=i+1) LBP_M[i] = 0;
end

always@(negedge clk) 
	if (lbp_valid) LBP_M[ lbp_addr ] <= lbp_data;

endmodule

module gray_mem (gray_valid, gray_data, gray_addr, clk);
input		gray_valid;
input	[13:0] 	gray_addr;
input	[7:0]	gray_data;
input		clk;

reg [7:0] gray_M [0:16383];
integer i;

initial begin
	for (i=0; i<=16383; i=i+1) gray_M[i] = 0;
end

always@(negedge clk) 
	if (gray_valid) gray_M[ gray_addr ] <= gray_data;

endmodule




