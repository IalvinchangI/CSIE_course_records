module Sudoku(
    input            clk,
    input            rst,
    
    // ROM Interface
    output           ROM_rd,        
    output     [6:0] ROM_A,         
    input      [7:0] ROM_Q,
    
    // RAM Interface
    output           RAM_ceb,       
    output           RAM_web,       
    output     [7:0] RAM_D,         
    output     [6:0] RAM_A,         
    input      [7:0] RAM_Q,
    
    // Finish Signal
    output           done           
);

    //=======================================================
    // Parameters & State
    //=======================================================
    localparam NULL   = 2'b00;
    localparam LOAD   = 2'b01;
    localparam SOLVE  = 2'b11;
    localparam OUTPUT = 2'b10;

    //=======================================================
    // Registers
    //=======================================================
    reg [3:0] grid [0:80];   
    reg [0:80] fixed;        
    
    // Constraint Registers (The Accelerator)
    // 9 rows/cols/blks, each has 9 bits (1-9) indicating used numbers
    reg [9:1] row_constraints [0:8];
    reg [9:1] col_constraints [0:8];
    reg [9:1] blk_constraints [0:8];

    reg [6:0] cnt;
    reg [1:0] state;

    //=======================================================
    // Wires 
    //=======================================================
    wire [3:0] next_num;
    wire backtrack_signal;
    wire [6:0] next_ptr;
    wire [6:0] prev_ptr;

    // Coordinate Calculation Signals
    wire [3:0] r_idx;
    wire [3:0] c_idx;
    wire [3:0] b_idx;
    
    // Helper to clear constraints on backtrack
    wire [3:0] val_to_clear;
    assign val_to_clear = grid[cnt];

    // I/O Assignments
    assign ROM_rd = (state == LOAD);
    assign ROM_A  = cnt;

    assign RAM_ceb = (state == OUTPUT && cnt <= 80); 
    assign RAM_web = (state == OUTPUT && cnt <= 80) ? 1'b0 : 1'b1; 
    assign RAM_A   = cnt;
    assign RAM_D   = {4'b0000, grid[cnt]}; 

    assign done = (state == OUTPUT && cnt == 81);

    //=======================================================
    // Coordinate Logic (Shared)
    //=======================================================
    coordinate_calculator u_coord (
        .addr(cnt),
        .row_idx(r_idx),
        .col_idx(c_idx),
        .blk_idx(b_idx)
    );

    //=======================================================
    // Module Instantiations
    //=======================================================
    
    // 1. Number Decider (Optimized: No longer reads grid directly)
    number_decide u_decider (
        .current_val(grid[cnt]),
        .row_mask(row_constraints[r_idx]),
        .col_mask(col_constraints[c_idx]),
        .blk_mask(blk_constraints[b_idx]),
        .next_num(next_num),
        .backtrack(backtrack_signal)
    );

    // 2. Smart Pointer
    pointer_logic u_pointer (
        .fixed_map(fixed),
        .current_ptr(cnt),
        .next_ptr(next_ptr),
        .prev_ptr(prev_ptr)
    );

    //=======================================================
    // FSM & Main Logic
    //=======================================================
    integer i;
    
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            state <= NULL;
            cnt   <= 0;
            
            // Clear Constraints
            for(i=0; i<9; i=i+1) begin
                row_constraints[i] <= 9'b0;
                col_constraints[i] <= 9'b0;
                blk_constraints[i] <= 9'b0;
            end
            
        end else begin
            case (state)
                LOAD: begin
                    cnt <= cnt + 1;
                    
                    if (cnt >= 0) begin
                        grid[cnt]  <= ROM_Q[3:0];
                        fixed[cnt] <= |ROM_Q;
                        
                        // Initialize Constraints
                        if (|ROM_Q) begin
                            row_constraints[r_idx][ROM_Q[3:0]] <= 1'b1;
                            col_constraints[c_idx][ROM_Q[3:0]] <= 1'b1;
                            blk_constraints[b_idx][ROM_Q[3:0]] <= 1'b1;
                        end
                    end
                    
                    if (cnt == 80) begin
                        state <= SOLVE;
                        cnt   <= 0;
                    end
                end

                SOLVE: begin
                    if (cnt == 81) begin
                        state <= OUTPUT;
                        cnt   <= 0;
                    end
                    else if (fixed[cnt]) begin
                        cnt <= next_ptr;
                    end
                    else begin
                        if (backtrack_signal) begin
                            // Backtrack: Clear current cell AND Clear Constraints
                            grid[cnt] <= 4'd0;
                            
                            cnt <= prev_ptr;
                        end else begin
                            // Forward: Fill cell AND Set Constraints
                            grid[cnt] <= next_num;
                            
                            // Set the bit for the new number
                            row_constraints[r_idx][next_num] <= 1'b1;
                            col_constraints[c_idx][next_num] <= 1'b1;
                            blk_constraints[b_idx][next_num] <= 1'b1;
                            
                            cnt <= next_ptr;
                        end

                        if (val_to_clear != 0) begin
                            row_constraints[r_idx][val_to_clear] <= 1'b0;
                            col_constraints[c_idx][val_to_clear] <= 1'b0;
                            blk_constraints[b_idx][val_to_clear] <= 1'b0;
                        end
                    end
                end

                OUTPUT: begin
                    if (cnt < 81) begin
                        cnt <= cnt + 1;
                    end
                end
                
                default: begin
                    state <= LOAD;
                end
            endcase
        end
    end

endmodule

//=======================================================
// Sub-module: Number Decider (Pure Logic, No Grid Fetch)
//=======================================================
module number_decide (
    input [3:0] current_val,
    input [9:1] row_mask,
    input [9:1] col_mask,
    input [9:1] blk_mask,
    output reg [3:0] next_num,
    output reg backtrack
);
    wire [9:1] total_mask;
    assign total_mask = row_mask | col_mask | blk_mask;

    reg found;
    integer k;

    always @(*) begin
        next_num = 0;
        backtrack = 1; 
        found = 0; 

        for (k = 1; k <= 9; k = k + 1) begin
            if (!found && k > current_val) begin 
                if (total_mask[k] == 1'b0) begin 
                    next_num = k[3:0];
                    backtrack = 0;
                    found = 1; 
                end
            end
        end
    end
endmodule

//=======================================================
// Sub-module: Smart Pointer
//=======================================================
module pointer_logic (
    input [0:80] fixed_map, 
    input [6:0] current_ptr,
    output reg [6:0] next_ptr,
    output reg [6:0] prev_ptr
);
    integer i;
    reg found_next;
    reg found_prev;

    always @(*) begin
        next_ptr = 7'd81; 
        found_next = 0;
        for (i = 0; i < 81; i = i + 1) begin
            if (!found_next && i > current_ptr) begin
                if (fixed_map[i] == 1'b0) begin
                    next_ptr = i[6:0];
                    found_next = 1;
                end
            end
        end
    end

    always @(*) begin
        prev_ptr = 7'd0; 
        found_prev = 0;
        for (i = 80; i >= 0; i = i - 1) begin
            if (!found_prev && i < current_ptr) begin
                if (fixed_map[i] == 1'b0) begin
                    prev_ptr = i[6:0];
                    found_prev = 1;
                end
            end
        end
    end
endmodule

//=======================================================
// Sub-module: Coordinate Calculator
//=======================================================
module coordinate_calculator (
    input  [6:0] addr,
    output reg [3:0] row_idx,
    output reg [3:0] col_idx,
    output reg [3:0] blk_idx
);
    always @(*) begin
        case(addr)
            // Row 0
            7'd0: begin row_idx=0; col_idx=0; blk_idx=0; end
            7'd1: begin row_idx=0; col_idx=1; blk_idx=0; end
            7'd2: begin row_idx=0; col_idx=2; blk_idx=0; end
            7'd3: begin row_idx=0; col_idx=3; blk_idx=1; end
            7'd4: begin row_idx=0; col_idx=4; blk_idx=1; end
            7'd5: begin row_idx=0; col_idx=5; blk_idx=1; end
            7'd6: begin row_idx=0; col_idx=6; blk_idx=2; end
            7'd7: begin row_idx=0; col_idx=7; blk_idx=2; end
            7'd8: begin row_idx=0; col_idx=8; blk_idx=2; end
            // Row 1
            7'd9: begin row_idx=1; col_idx=0; blk_idx=0; end
            7'd10: begin row_idx=1; col_idx=1; blk_idx=0; end
            7'd11: begin row_idx=1; col_idx=2; blk_idx=0; end
            7'd12: begin row_idx=1; col_idx=3; blk_idx=1; end
            7'd13: begin row_idx=1; col_idx=4; blk_idx=1; end
            7'd14: begin row_idx=1; col_idx=5; blk_idx=1; end
            7'd15: begin row_idx=1; col_idx=6; blk_idx=2; end
            7'd16: begin row_idx=1; col_idx=7; blk_idx=2; end
            7'd17: begin row_idx=1; col_idx=8; blk_idx=2; end
            // Row 2
            7'd18: begin row_idx=2; col_idx=0; blk_idx=0; end
            7'd19: begin row_idx=2; col_idx=1; blk_idx=0; end
            7'd20: begin row_idx=2; col_idx=2; blk_idx=0; end
            7'd21: begin row_idx=2; col_idx=3; blk_idx=1; end
            7'd22: begin row_idx=2; col_idx=4; blk_idx=1; end
            7'd23: begin row_idx=2; col_idx=5; blk_idx=1; end
            7'd24: begin row_idx=2; col_idx=6; blk_idx=2; end
            7'd25: begin row_idx=2; col_idx=7; blk_idx=2; end
            7'd26: begin row_idx=2; col_idx=8; blk_idx=2; end
            // Row 3
            7'd27: begin row_idx=3; col_idx=0; blk_idx=3; end
            7'd28: begin row_idx=3; col_idx=1; blk_idx=3; end
            7'd29: begin row_idx=3; col_idx=2; blk_idx=3; end
            7'd30: begin row_idx=3; col_idx=3; blk_idx=4; end
            7'd31: begin row_idx=3; col_idx=4; blk_idx=4; end
            7'd32: begin row_idx=3; col_idx=5; blk_idx=4; end
            7'd33: begin row_idx=3; col_idx=6; blk_idx=5; end
            7'd34: begin row_idx=3; col_idx=7; blk_idx=5; end
            7'd35: begin row_idx=3; col_idx=8; blk_idx=5; end
            // Row 4
            7'd36: begin row_idx=4; col_idx=0; blk_idx=3; end
            7'd37: begin row_idx=4; col_idx=1; blk_idx=3; end
            7'd38: begin row_idx=4; col_idx=2; blk_idx=3; end
            7'd39: begin row_idx=4; col_idx=3; blk_idx=4; end
            7'd40: begin row_idx=4; col_idx=4; blk_idx=4; end
            7'd41: begin row_idx=4; col_idx=5; blk_idx=4; end
            7'd42: begin row_idx=4; col_idx=6; blk_idx=5; end
            7'd43: begin row_idx=4; col_idx=7; blk_idx=5; end
            7'd44: begin row_idx=4; col_idx=8; blk_idx=5; end
            // Row 5
            7'd45: begin row_idx=5; col_idx=0; blk_idx=3; end
            7'd46: begin row_idx=5; col_idx=1; blk_idx=3; end
            7'd47: begin row_idx=5; col_idx=2; blk_idx=3; end
            7'd48: begin row_idx=5; col_idx=3; blk_idx=4; end
            7'd49: begin row_idx=5; col_idx=4; blk_idx=4; end
            7'd50: begin row_idx=5; col_idx=5; blk_idx=4; end
            7'd51: begin row_idx=5; col_idx=6; blk_idx=5; end
            7'd52: begin row_idx=5; col_idx=7; blk_idx=5; end
            7'd53: begin row_idx=5; col_idx=8; blk_idx=5; end
            // Row 6
            7'd54: begin row_idx=6; col_idx=0; blk_idx=6; end
            7'd55: begin row_idx=6; col_idx=1; blk_idx=6; end
            7'd56: begin row_idx=6; col_idx=2; blk_idx=6; end
            7'd57: begin row_idx=6; col_idx=3; blk_idx=7; end
            7'd58: begin row_idx=6; col_idx=4; blk_idx=7; end
            7'd59: begin row_idx=6; col_idx=5; blk_idx=7; end
            7'd60: begin row_idx=6; col_idx=6; blk_idx=8; end
            7'd61: begin row_idx=6; col_idx=7; blk_idx=8; end
            7'd62: begin row_idx=6; col_idx=8; blk_idx=8; end
            // Row 7
            7'd63: begin row_idx=7; col_idx=0; blk_idx=6; end
            7'd64: begin row_idx=7; col_idx=1; blk_idx=6; end
            7'd65: begin row_idx=7; col_idx=2; blk_idx=6; end
            7'd66: begin row_idx=7; col_idx=3; blk_idx=7; end
            7'd67: begin row_idx=7; col_idx=4; blk_idx=7; end
            7'd68: begin row_idx=7; col_idx=5; blk_idx=7; end
            7'd69: begin row_idx=7; col_idx=6; blk_idx=8; end
            7'd70: begin row_idx=7; col_idx=7; blk_idx=8; end
            7'd71: begin row_idx=7; col_idx=8; blk_idx=8; end
            // Row 8
            7'd72: begin row_idx=8; col_idx=0; blk_idx=6; end
            7'd73: begin row_idx=8; col_idx=1; blk_idx=6; end
            7'd74: begin row_idx=8; col_idx=2; blk_idx=6; end
            7'd75: begin row_idx=8; col_idx=3; blk_idx=7; end
            7'd76: begin row_idx=8; col_idx=4; blk_idx=7; end
            7'd77: begin row_idx=8; col_idx=5; blk_idx=7; end
            7'd78: begin row_idx=8; col_idx=6; blk_idx=8; end
            7'd79: begin row_idx=8; col_idx=7; blk_idx=8; end
            7'd80: begin row_idx=8; col_idx=8; blk_idx=8; end
            default: begin row_idx=0; col_idx=0; blk_idx=0; end
        endcase
    end
endmodule