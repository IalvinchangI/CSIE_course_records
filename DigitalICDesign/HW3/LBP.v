
`timescale 1ns/10ps
module LBP ( 
    input             clk       , 
    input             reset     , 
    output reg [13:0] RGB_addr  , 
    output reg        RGB_req   ,  // p.s. should not be flip-flop
    input             RGB_ready , 
    input      [23:0] RGB_data  ,
    output reg [13:0] lbp_addr  , 
    output reg        lbp_valid ,  // p.s. should not be flip-flop
    output reg [7:0]  lbp_data  , 
    output reg [13:0] gray_addr , 
    output reg        gray_valid,  // p.s. should not be flip-flop
    output reg [7:0]  gray_data , 
    output reg        finish
);

// block state
parameter CENTER_BLOCK = 0;
parameter NEXT_CENTER_BLOCK = 1;
parameter TOP_BLOCK = 2;
parameter BOTTOM_BLOCK = 3;
reg [1:0] block_state;

// small (step) state
parameter STEP_NULL = 2'b11;
parameter READ = 2'b00;
parameter OUTPUT_GRAY = 2'b01;
parameter OUTPUT_LBP = 2'b10;
reg [1:0] step_state;
// use step to control valid signal
always @(step_state) begin
    case (step_state)
        STEP_NULL: begin
            RGB_req <= 0;
            gray_valid <= 0;
            lbp_valid <= 0;
        end
        READ: begin
            RGB_req <= 1;
            gray_valid <= 0;
            lbp_valid <= 0;
        end
        OUTPUT_GRAY: begin
            RGB_req <= 0;
            gray_valid <= 1;
            lbp_valid <= 0;
        end
        OUTPUT_LBP: begin
            RGB_req <= 0;
            gray_valid <= 0;
            lbp_valid <= 1;
        end
    endcase
end


// temporarily store previous center & current center & next center
reg [7:0] center_register [2:0];
// temp LBP output  (lbp_data <= temp_lbp_data[1] when OUTPUT_LBP)
reg [7:0] temp_lbp_data [2:1];

// current pos
reg [6:0] x;
reg [6:0] y;



always @(posedge clk or posedge reset) begin
    if (reset) begin
        RGB_addr <= 0;
        gray_addr <= 0;
        gray_data <= 0;
        lbp_addr <= 0;
        lbp_data <= 0;
        temp_lbp_data[1] <= 0;
        temp_lbp_data[2] <= 0;
        center_register[0] <= 0;
        center_register[1] <= 0;
        center_register[2] <= 0;
        x <= 0;
        y <= 1;
        finish <= 0;

        block_state <= CENTER_BLOCK;
        step_state <= STEP_NULL;
    end
    else begin
        if (RGB_ready == 1 && finish == 0) begin
            // operation
            case (step_state)
                READ: begin  // calculate gray
                    gray_data <= (RGB_data[7:0] + RGB_data[15:8] + RGB_data[23:16]) / 3;
                end
                OUTPUT_GRAY: begin  // calculate LBP
                    case (block_state)  // bit operation on temp_lbp_data or push gray into center_register
                        CENTER_BLOCK: begin
                            center_register[1] <= gray_data;
                        end
                        NEXT_CENTER_BLOCK: begin
                            center_register[2] <= gray_data;
                            // temp2
                            temp_lbp_data[2][3] <= (center_register[1] >= gray_data);
                            // temp1
                            temp_lbp_data[1][4] <= (gray_data >= center_register[1]);
                            // lbp_data
                            // NOP
                        end
                        TOP_BLOCK: begin
                            // temp2
                            temp_lbp_data[2][0] <= (gray_data >= center_register[2]);
                            // temp1
                            temp_lbp_data[1][1] <= (gray_data >= center_register[1]);
                            // lbp_data
                            lbp_data[2] <= (gray_data >= center_register[0]);
                        end
                        BOTTOM_BLOCK: begin
                            if (x == 0 || x == 1) begin
                                // temp2
                                temp_lbp_data[1][5] <= (gray_data >= center_register[2]);
                                // temp1
                                lbp_data[6] <= (gray_data >= center_register[1]);
                                // lbp_data
                                // lbp_data[7] <= (gray_data >= center_register[0]);
                                
                                // shift LGB register: lbp_data <= temp_lbp_data; temp_lbp_data <= 0;
                                temp_lbp_data[2] <= 0;
                                temp_lbp_data[1][7:6] <= temp_lbp_data[2][7:6];
                                temp_lbp_data[1][4:0] <= temp_lbp_data[2][4:0];
                                lbp_data[5:0] <= temp_lbp_data[1][5:0];
                                lbp_data[7] <= temp_lbp_data[1][7];
                            end
                            else begin
                                // temp2
                                temp_lbp_data[2][5] <= (gray_data >= center_register[2]);
                                // temp1
                                temp_lbp_data[1][6] <= (gray_data >= center_register[1]);
                                // lbp_data
                                lbp_data[7] <= (gray_data >= center_register[0]);
                            end

                            // shift center_register
                            center_register[1] <= center_register[2];
                            center_register[0] <= center_register[1];
                        end
                    endcase
                end
                OUTPUT_LBP: begin
                    // shift LGB register: lbp_data <= temp_lbp_data; temp_lbp_data <= 0;
                    temp_lbp_data[2] <= 0;
                    temp_lbp_data[1] <= temp_lbp_data[2];
                    lbp_data <= temp_lbp_data[1];
                end
                default: begin end
            endcase
            
            // update anything (output & state & pos)
            case (step_state)
                READ: begin
                    step_state <= OUTPUT_GRAY;
                    gray_addr <= RGB_addr;
                end
                OUTPUT_GRAY: begin
                    case (block_state)  // update block_state & RGB_addr & pos
                        CENTER_BLOCK: begin
                            step_state <= READ;
                            block_state <= NEXT_CENTER_BLOCK;  // prepare NEXT_CENTER_BLOCK
                            RGB_addr[13:7] <= y;
                            RGB_addr[ 6:0] <= x + 1;
                        end
                        NEXT_CENTER_BLOCK: begin
                            step_state <= READ;
                            block_state <= TOP_BLOCK;  // prepare TOP_BLOCK
                            RGB_addr[13:7] <= y - 1;
                            RGB_addr[ 6:0] <= x;
                        end
                        TOP_BLOCK: begin
                            step_state <= READ;
                            block_state <= BOTTOM_BLOCK;  // prepare BOTTOM_BLOCK
                            RGB_addr[13:7] <= y + 1;
                            RGB_addr[ 6:0] <= x;
                        end
                        BOTTOM_BLOCK: begin
                            case (x)
                                0, 1: begin  // next block
                                    step_state <= READ;
                                    block_state <= NEXT_CENTER_BLOCK;  // prepare NEXT_CENTER_BLOCK
                                    RGB_addr[13:7] <= y;
                                    RGB_addr[ 6:0] <= x + 2;
                                    x <= x + 1;
                                end
                                // output LBP
                                127: begin  // next horizontal line or finish
                                    step_state <= OUTPUT_LBP;
                                    lbp_addr[13:7] <= y;
                                    lbp_addr[ 6:0] <= x - 1;
                                    block_state <= CENTER_BLOCK;  // prepare CENTER_BLOCK
                                    RGB_addr[13:7] <= y + 1;
                                    RGB_addr[ 6:0] <= 0;
                                    // next horizontal line
                                    x <= 0;
                                    y <= y + 1;
                                end
                                126: begin  // x + 1 == 127  // will reach the end of a horizontal line
                                    step_state <= OUTPUT_LBP;
                                    lbp_addr[13:7] <= y;
                                    lbp_addr[ 6:0] <= x - 1;
                                    block_state <= TOP_BLOCK;  // prepare TOP_BLOCK
                                    RGB_addr[13:7] <= y - 1;
                                    RGB_addr[ 6:0] <= x + 1;
                                    x <= x + 1;
                                end
                                default: begin  // middle of a horizontal line
                                    step_state <= OUTPUT_LBP;
                                    lbp_addr[13:7] <= y;
                                    lbp_addr[ 6:0] <= x - 1;
                                    block_state <= NEXT_CENTER_BLOCK;  // prepare NEXT_CENTER_BLOCK
                                    RGB_addr[13:7] <= y;
                                    RGB_addr[ 6:0] <= x + 2;
                                    x <= x + 1;
                                end
                            endcase
                        end
                    endcase
                end
                OUTPUT_LBP: begin
                    if (y == 127 && x == 0) begin  // finish !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                        step_state <= STEP_NULL;
                        finish <= 1;
                    end
                    else begin
                        step_state <= READ;
                    end
                end
                default: begin  // start process
                    step_state <= READ;
                    block_state <= CENTER_BLOCK;
                    RGB_addr <= {y, x};  // prepare CENTER_BLOCK
                end
            endcase
        end
    end
end
endmodule
