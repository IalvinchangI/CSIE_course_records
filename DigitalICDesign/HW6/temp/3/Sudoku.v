module Sudoku(
    input            clk,
    input            rst,
    
    // ROM Interface
    output           ROM_rd,        // wire by default
    output     [6:0] ROM_A,         // wire
    input      [7:0] ROM_Q,
    
    // RAM Interface
    output           RAM_ceb,       // wire
    output           RAM_web,       // wire
    output     [7:0] RAM_D,         // wire
    output     [6:0] RAM_A,         // wire
    input      [7:0] RAM_Q,
    
    // Finish Signal
    output           done           // wire
);

    //=======================================================
    // Parameters & State Definition
    //=======================================================
    localparam NULL   = 2'b00;
    localparam LOAD   = 2'b01;
    localparam SOLVE  = 2'b11;
    localparam OUTPUT = 2'b10;

    //=======================================================
    // Registers
    //=======================================================
    // Grid: 81 cells * 4 bits = 324 bits
    reg [3:0] grid [0:80];   
    
    // Fixed: 81 bits
    reg [0:80] fixed;        
    
    reg [6:0] cnt;
    reg [1:0] state;

    //=======================================================
    // Wires & Combinational Logic
    //=======================================================
    
    // Solver Signals
    wire [3:0] next_num;
    wire backtrack_signal;
    wire [6:0] next_ptr;
    wire [6:0] prev_ptr;

    // 1. Grid Bus Wiring (Optimization: Avoids massive MUX from flat grid)
    wire [35:0] row_buses [0:8];
    wire [35:0] col_buses [0:8];
    wire [35:0] blk_buses [0:8];

    genvar g_i, g_j;
    generate
        // Wire Rows
        for (g_i = 0; g_i < 9; g_i = g_i + 1) begin : gen_row
            for (g_j = 0; g_j < 9; g_j = g_j + 1) begin : gen_r
                assign row_buses[g_i][g_j*4 +: 4] = grid[g_i*9 + g_j];
            end
        end
        // Wire Cols
        for (g_i = 0; g_i < 9; g_i = g_i + 1) begin : gen_col
            for (g_j = 0; g_j < 9; g_j = g_j + 1) begin : gen_c
                assign col_buses[g_i][g_j*4 +: 4] = grid[g_j*9 + g_i];
            end
        end
        // Wire Blocks (0..8)
        for (g_i = 0; g_i < 9; g_i = g_i + 1) begin : gen_blk
            for (g_j = 0; g_j < 9; g_j = g_j + 1) begin : gen_b
                // Mapping block index g_i and cell index g_j to global grid index
                assign blk_buses[g_i][g_j*4 +: 4] = grid[ ((g_i/3)*3 + (g_j/3))*9 + ((g_i%3)*3 + (g_j%3)) ];
            end
        end
    endgenerate

    // 2. Coordinate LUT (Optimization: Returns indices 0-8 directly)
    wire [3:0] cur_row_idx;
    wire [3:0] cur_col_idx;
    wire [3:0] cur_blk_idx;

    coordinate_lut u_lut (
        .addr(cnt),
        .row_idx(cur_row_idx),
        .col_idx(cur_col_idx),
        .blk_idx(cur_blk_idx)
    );

    // 3. Data Fetch MUX (Selects the correct vectors based on LUT)
    reg [35:0] cur_row_vec;
    reg [35:0] cur_col_vec;
    reg [35:0] cur_blk_vec;

    always @(*) begin
        cur_row_vec = row_buses[cur_row_idx];
        cur_col_vec = col_buses[cur_col_idx];
        cur_blk_vec = blk_buses[cur_blk_idx];
    end

    // I/O Assignments
    assign ROM_rd = (state == LOAD);
    assign ROM_A  = cnt;

    assign RAM_ceb = (state == OUTPUT && cnt <= 80); 
    assign RAM_web = (state == OUTPUT && cnt <= 80) ? 1'b0 : 1'b1; // Low for Write
    assign RAM_A   = cnt;
    assign RAM_D   = {4'b0000, grid[cnt]}; 

    assign done = (state == OUTPUT && cnt == 81);

    //=======================================================
    // Module Instantiations
    //=======================================================
    
    // 1. Number Decider (Modified to take vectors instead of grid_flat)
    number_decide u_decider (
        .current_val(grid[cnt]),
        .row_vec(cur_row_vec),
        .col_vec(cur_col_vec),
        .blk_vec(cur_blk_vec),
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
    // Finite State Machine (FSM)
    //=======================================================
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            state <= NULL;
            cnt   <= 0;
        end else begin
            case (state)
                LOAD: begin
                    cnt <= cnt + 1;
                    
                    if (cnt >= 0) begin // cnt is unsigned, always true, but keeps structure
                        grid[cnt]  <= ROM_Q[3:0];
                        fixed[cnt] <= |ROM_Q;
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
                    // Smart Jump over fixed cells
                    else if (fixed[cnt]) begin
                        cnt <= next_ptr;
                    end
                    else begin
                        if (backtrack_signal) begin
                            grid[cnt] <= 4'd0;
                            cnt       <= prev_ptr;
                        end else begin
                            grid[cnt] <= next_num;
                            cnt       <= next_ptr;
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
// Sub-module: Number Decider (Binary Tree Optimized)
//=======================================================
module number_decide (
    input [3:0]  current_val,
    input [35:0] row_vec,
    input [35:0] col_vec,
    input [35:0] blk_vec,
    output reg [3:0] next_num,
    output reg backtrack
);
    integer i;
    reg [9:1] used_mask;
    reg [8:0] total_mask;
    reg [9:1] greater_mask;

    // 1. Generate Used Mask (OR Tree)
    always @(*) begin
        used_mask = 9'b0;
        for (i=0; i<9; i=i+1) begin
            if (row_vec[i*4 +: 4] != 0) used_mask[row_vec[i*4 +: 4]] = 1'b1;
            if (col_vec[i*4 +: 4] != 0) used_mask[col_vec[i*4 +: 4]] = 1'b1;
            if (blk_vec[i*4 +: 4] != 0) used_mask[blk_vec[i*4 +: 4]] = 1'b1;
        end
    end

    // 2. Pre-computed mask for "Candidates > current_val"
    always @(*) begin
        case(current_val)
            4'd0: greater_mask = 9'b111111111;
            4'd1: greater_mask = 9'b111111110;
            4'd2: greater_mask = 9'b111111100;
            4'd3: greater_mask = 9'b111111000;
            4'd4: greater_mask = 9'b111110000;
            4'd5: greater_mask = 9'b111100000;
            4'd6: greater_mask = 9'b111000000;
            4'd7: greater_mask = 9'b110000000;
            4'd8: greater_mask = 9'b100000000;
            default: greater_mask = 9'b0;
        endcase
    end

    // 3. Binary Tree Priority Encoder
    always @(*) begin
        next_num = 0;
        backtrack = 1; // Default to 1 (Reduces logic complexity)

        // Combine Used Mask and Greater Mask
        // total_mask bits: 1 means VALID CANDIDATE
        total_mask = (~used_mask) & greater_mask;

        if (total_mask[3:0] != 0) begin
            if (total_mask[1:0] != 0) begin
                if (total_mask[0] != 0) begin
                    next_num = 1; backtrack = 0;
                end else begin  // total_mask[1]
                    next_num = 2; backtrack = 0;
                end
            end else begin  // total_mask[3:2]
                if (total_mask[2] != 0) begin
                    next_num = 3; backtrack = 0;
                end else begin  // total_mask[3]
                    next_num = 4; backtrack = 0;
                end
            end
        end else begin  // total_mask[8:4]
            if (total_mask[5:4] != 0) begin
                if (total_mask[4] != 0) begin
                    next_num = 5; backtrack = 0;
                end else begin  // total_mask[5]
                    next_num = 6; backtrack = 0;
                end
            end else begin  // total_mask[8:6]
                if (total_mask[7:6] != 0) begin
                    if (total_mask[6] != 0) begin
                        next_num = 7; backtrack = 0;
                    end else begin  // total_mask[7]
                        next_num = 8; backtrack = 0;
                    end
                end else begin  // total_mask[8] or None
                    if (total_mask[8] != 0) begin
                        next_num = 9; backtrack = 0;
                    end 
                    // else: backtrack remains 1
                end
            end
        end
    end
endmodule

//=======================================================
// Sub-module: Pointer Logic
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

    // Find Next Empty
    always @(*) begin
        next_ptr = 7'd81; // Default
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

    // Find Prev Empty
    always @(*) begin
        prev_ptr = 7'd0; // Default
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
// Sub-module: Coordinate Look Up Table
// Updated to return 0-8 indices for direct MUX mapping
//=======================================================
module coordinate_lut (
    input  [6:0] addr,
    output reg [3:0] row_idx,
    output reg [3:0] col_idx,
    output reg [3:0] blk_idx
);
    always @(*) begin
        case(addr)
            // Row 0
            7'd0:  begin row_idx=0; col_idx=0; blk_idx=0; end
            7'd1:  begin row_idx=0; col_idx=1; blk_idx=0; end
            7'd2:  begin row_idx=0; col_idx=2; blk_idx=0; end
            7'd3:  begin row_idx=0; col_idx=3; blk_idx=1; end
            7'd4:  begin row_idx=0; col_idx=4; blk_idx=1; end
            7'd5:  begin row_idx=0; col_idx=5; blk_idx=1; end
            7'd6:  begin row_idx=0; col_idx=6; blk_idx=2; end
            7'd7:  begin row_idx=0; col_idx=7; blk_idx=2; end
            7'd8:  begin row_idx=0; col_idx=8; blk_idx=2; end
            // Row 1
            7'd9:  begin row_idx=1; col_idx=0; blk_idx=0; end
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