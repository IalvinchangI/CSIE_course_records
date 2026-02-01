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
    // Use unpacked array for easier internal access
    reg [3:0] grid [0:80];   
    
    // Fixed: 81 bits. Use PACKED array to easily pass to submodules
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

    // Flatten grid to pass to submodule (Verilog-2001 workaround)
    wire [323:0] grid_flat;
    
    // Generate loop to flatten the 2D grid into 1D wire
    genvar g_i;
    generate
        for (g_i = 0; g_i < 81; g_i = g_i + 1) begin : flatten_grid
            assign grid_flat[g_i*4 +: 4] = grid[g_i];
        end
    endgenerate

    // I/O Assignments
    assign ROM_rd = (state == LOAD);
    assign ROM_A  = cnt;

    assign RAM_ceb = (state == OUTPUT && cnt <= 80); 
    assign RAM_web = (state == OUTPUT && cnt <= 80) ? 1'b0 : 1'b1; // Low for Write
    assign RAM_A   = cnt;
    // Pad with 0s for RAM output
    assign RAM_D   = {4'b0000, grid[cnt]}; 

    // Done Signal
    assign done = (state == OUTPUT && cnt == 81);

    //=======================================================
    // Module Instantiations
    //=======================================================
    
    // 1. Number Decider
    number_decide u_decider (
        .grid_flat(grid_flat), // Pass flattened wire
        .addr(cnt),
        .next_num(next_num),
        .backtrack(backtrack_signal)
    );

    // 2. Smart Pointer
    pointer_logic u_pointer (
        .fixed_map(fixed),     // Pass packed register
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
            // fixed and grid do not need reset
        end else begin
            case (state)
                LOAD: begin
                    // Pipeline behavior matches original design
                    cnt <= cnt + 1;
                    
                    if (cnt >= 0) begin
                        grid[cnt]  <= ROM_Q[3:0];
                        fixed[cnt] <= |ROM_Q;
                    end
                    
                    if (cnt == 80) begin
                        state <= SOLVE;
                        cnt   <= 0;
                    end
                end

                SOLVE: begin
                    if (cnt == 81) begin  // when next_ptr == 81, finish
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
// Sub-module: Number Decider
//=======================================================
module number_decide (
    input [323:0] grid_flat, // Flattened 81*4
    input [6:0] addr,
    output reg [3:0] next_num,
    output reg backtrack
);
    wire [8:0] temp_addr = {2'b0, addr};
    wire [6:0] row_start;
    wire [3:0] col_idx;
    wire [6:0] blk_start;
    
    coordinate_lut lut (
        .addr(addr), 
        .row_start(row_start), 
        .col_idx(col_idx), 
        .blk_start(blk_start)
    );

    // Flattened inputs for constraint units (9 * 4 = 36 bits)
    reg [35:0] row_vec;
    reg [35:0] col_vec;
    reg [35:0] blk_vec;
    
    wire [8:0] row_mask, col_mask, blk_mask, total_mask;
    
    integer i;

    // Data Fetching
    always @(*) begin
        // Row Fetch
        for(i=0; i<9; i=i+1) begin
            row_vec[(i << 2) +: 4] = grid_flat[((row_start + i) << 2) +: 4];
        end

        // Col Fetch
        for(i=0; i<9; i=i+1) begin
            col_vec[(i << 2) +: 4] = grid_flat[((col_idx + i*9) << 2) +: 4];
        end

        // Block Fetch (Direct indexing with shift)
        blk_vec[0 +: 4]  = grid_flat[(blk_start + 0)  << 2 +: 4];
        blk_vec[4 +: 4]  = grid_flat[(blk_start + 1)  << 2 +: 4];
        blk_vec[8 +: 4]  = grid_flat[(blk_start + 2)  << 2 +: 4];
        blk_vec[12 +: 4] = grid_flat[(blk_start + 9)  << 2 +: 4];
        blk_vec[16 +: 4] = grid_flat[(blk_start + 10) << 2 +: 4];
        blk_vec[20 +: 4] = grid_flat[(blk_start + 11) << 2 +: 4];
        blk_vec[24 +: 4] = grid_flat[(blk_start + 18) << 2 +: 4];
        blk_vec[28 +: 4] = grid_flat[(blk_start + 19) << 2 +: 4];
        blk_vec[32 +: 4] = grid_flat[(blk_start + 20) << 2 +: 4];
    end

    // Constraint Checking
    constraint_unit u_row (.in_nums(row_vec), .used_mask(row_mask));
    constraint_unit u_col (.in_nums(col_vec), .used_mask(col_mask));
    constraint_unit u_blk (.in_nums(blk_vec), .used_mask(blk_mask));

    // Decision Logic
    wire [3:0] current_val;

    assign current_val = grid_flat[(temp_addr << 2) +: 4];
    assign total_mask = (row_mask & col_mask & blk_mask) & (9'b111111111 << current_val);

    always @(*) begin
        next_num = 0;
        backtrack = 0;

        // Try candidates > current_val
        if (total_mask[3:0] != 0) begin
            if (total_mask[1:0] != 0) begin
                if (total_mask[0] != 0) begin
                    next_num = 1;
                end else begin  // total_mask[1]
                    next_num = 2;
                end
            end else begin  // total_mask[3:2]
                if (total_mask[2] != 0) begin
                    next_num = 3;
                end else begin  // total_mask[3]
                    next_num = 4;
                end
            end
        end else begin  // total_mask[8:4]
            if (total_mask[5:4] != 0) begin
                if (total_mask[4] != 0) begin
                    next_num = 5;
                end else begin  // total_mask[5]
                    next_num = 6;
                end
            end else begin  // total_mask[8:6]
                if (total_mask[7:6] != 0) begin
                    if (total_mask[6] != 0) begin
                        next_num = 7;
                    end else begin  // total_mask[7]
                        next_num = 8;
                    end
                end else begin  // total_mask[8] or None
                    if (total_mask[8] != 0) begin
                        next_num = 9;
                    end else begin  // None
                        backtrack = 1;
                    end
                end
            end
        end
    end
endmodule

//=======================================================
// Sub-module: Pointer Logic (Smart Jump)
//=======================================================
module pointer_logic (
    input [0:80] fixed_map, // Packed array input
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
// Sub-module: Constraint Unit
//=======================================================
module constraint_unit (
    input [35:0] in_nums, // Flattened 9*4
    output [9:1] used_mask
);
    integer i;
    reg [9:0] mask;
    reg [3:0] val;
    
    assign used_mask = mask[9:1];
    always @(*) begin
        mask[9:1] = 9'b111111111;
        for (i = 0; i < 9; i = i + 1) begin
            val = in_nums[(i << 2) +: 4];
            mask[val] = 1'b0;
        end
    end
endmodule

//=======================================================
// Sub-module: Coordinate Look Up Table
//=======================================================
module coordinate_lut (
    input  [6:0] addr,
    output reg [6:0] row_start,
    output reg [3:0] col_idx,
    output reg [6:0] blk_start
);
    always @(*) begin
        case(addr)
            // Row 0
            7'd0: begin row_start=7'd0; col_idx=4'd0; blk_start=7'd0; end
            7'd1: begin row_start=7'd0; col_idx=4'd1; blk_start=7'd0; end
            7'd2: begin row_start=7'd0; col_idx=4'd2; blk_start=7'd0; end
            7'd3: begin row_start=7'd0; col_idx=4'd3; blk_start=7'd3; end
            7'd4: begin row_start=7'd0; col_idx=4'd4; blk_start=7'd3; end
            7'd5: begin row_start=7'd0; col_idx=4'd5; blk_start=7'd3; end
            7'd6: begin row_start=7'd0; col_idx=4'd6; blk_start=7'd6; end
            7'd7: begin row_start=7'd0; col_idx=4'd7; blk_start=7'd6; end
            7'd8: begin row_start=7'd0; col_idx=4'd8; blk_start=7'd6; end
            // Row 1
            7'd9: begin row_start=7'd9; col_idx=4'd0; blk_start=7'd0; end
            7'd10: begin row_start=7'd9; col_idx=4'd1; blk_start=7'd0; end
            7'd11: begin row_start=7'd9; col_idx=4'd2; blk_start=7'd0; end
            7'd12: begin row_start=7'd9; col_idx=4'd3; blk_start=7'd3; end
            7'd13: begin row_start=7'd9; col_idx=4'd4; blk_start=7'd3; end
            7'd14: begin row_start=7'd9; col_idx=4'd5; blk_start=7'd3; end
            7'd15: begin row_start=7'd9; col_idx=4'd6; blk_start=7'd6; end
            7'd16: begin row_start=7'd9; col_idx=4'd7; blk_start=7'd6; end
            7'd17: begin row_start=7'd9; col_idx=4'd8; blk_start=7'd6; end
            // Row 2
            7'd18: begin row_start=7'd18; col_idx=4'd0; blk_start=7'd0; end
            7'd19: begin row_start=7'd18; col_idx=4'd1; blk_start=7'd0; end
            7'd20: begin row_start=7'd18; col_idx=4'd2; blk_start=7'd0; end
            7'd21: begin row_start=7'd18; col_idx=4'd3; blk_start=7'd3; end
            7'd22: begin row_start=7'd18; col_idx=4'd4; blk_start=7'd3; end
            7'd23: begin row_start=7'd18; col_idx=4'd5; blk_start=7'd3; end
            7'd24: begin row_start=7'd18; col_idx=4'd6; blk_start=7'd6; end
            7'd25: begin row_start=7'd18; col_idx=4'd7; blk_start=7'd6; end
            7'd26: begin row_start=7'd18; col_idx=4'd8; blk_start=7'd6; end
            // Row 3
            7'd27: begin row_start=7'd27; col_idx=4'd0; blk_start=7'd27; end
            7'd28: begin row_start=7'd27; col_idx=4'd1; blk_start=7'd27; end
            7'd29: begin row_start=7'd27; col_idx=4'd2; blk_start=7'd27; end
            7'd30: begin row_start=7'd27; col_idx=4'd3; blk_start=7'd30; end
            7'd31: begin row_start=7'd27; col_idx=4'd4; blk_start=7'd30; end
            7'd32: begin row_start=7'd27; col_idx=4'd5; blk_start=7'd30; end
            7'd33: begin row_start=7'd27; col_idx=4'd6; blk_start=7'd33; end
            7'd34: begin row_start=7'd27; col_idx=4'd7; blk_start=7'd33; end
            7'd35: begin row_start=7'd27; col_idx=4'd8; blk_start=7'd33; end
            // Row 4
            7'd36: begin row_start=7'd36; col_idx=4'd0; blk_start=7'd27; end
            7'd37: begin row_start=7'd36; col_idx=4'd1; blk_start=7'd27; end
            7'd38: begin row_start=7'd36; col_idx=4'd2; blk_start=7'd27; end
            7'd39: begin row_start=7'd36; col_idx=4'd3; blk_start=7'd30; end
            7'd40: begin row_start=7'd36; col_idx=4'd4; blk_start=7'd30; end
            7'd41: begin row_start=7'd36; col_idx=4'd5; blk_start=7'd30; end
            7'd42: begin row_start=7'd36; col_idx=4'd6; blk_start=7'd33; end
            7'd43: begin row_start=7'd36; col_idx=4'd7; blk_start=7'd33; end
            7'd44: begin row_start=7'd36; col_idx=4'd8; blk_start=7'd33; end
            // Row 5
            7'd45: begin row_start=7'd45; col_idx=4'd0; blk_start=7'd27; end
            7'd46: begin row_start=7'd45; col_idx=4'd1; blk_start=7'd27; end
            7'd47: begin row_start=7'd45; col_idx=4'd2; blk_start=7'd27; end
            7'd48: begin row_start=7'd45; col_idx=4'd3; blk_start=7'd30; end
            7'd49: begin row_start=7'd45; col_idx=4'd4; blk_start=7'd30; end
            7'd50: begin row_start=7'd45; col_idx=4'd5; blk_start=7'd30; end
            7'd51: begin row_start=7'd45; col_idx=4'd6; blk_start=7'd33; end
            7'd52: begin row_start=7'd45; col_idx=4'd7; blk_start=7'd33; end
            7'd53: begin row_start=7'd45; col_idx=4'd8; blk_start=7'd33; end
            // Row 6
            7'd54: begin row_start=7'd54; col_idx=4'd0; blk_start=7'd54; end
            7'd55: begin row_start=7'd54; col_idx=4'd1; blk_start=7'd54; end
            7'd56: begin row_start=7'd54; col_idx=4'd2; blk_start=7'd54; end
            7'd57: begin row_start=7'd54; col_idx=4'd3; blk_start=7'd57; end
            7'd58: begin row_start=7'd54; col_idx=4'd4; blk_start=7'd57; end
            7'd59: begin row_start=7'd54; col_idx=4'd5; blk_start=7'd57; end
            7'd60: begin row_start=7'd54; col_idx=4'd6; blk_start=7'd60; end
            7'd61: begin row_start=7'd54; col_idx=4'd7; blk_start=7'd60; end
            7'd62: begin row_start=7'd54; col_idx=4'd8; blk_start=7'd60; end
            // Row 7
            7'd63: begin row_start=7'd63; col_idx=4'd0; blk_start=7'd54; end
            7'd64: begin row_start=7'd63; col_idx=4'd1; blk_start=7'd54; end
            7'd65: begin row_start=7'd63; col_idx=4'd2; blk_start=7'd54; end
            7'd66: begin row_start=7'd63; col_idx=4'd3; blk_start=7'd57; end
            7'd67: begin row_start=7'd63; col_idx=4'd4; blk_start=7'd57; end
            7'd68: begin row_start=7'd63; col_idx=4'd5; blk_start=7'd57; end
            7'd69: begin row_start=7'd63; col_idx=4'd6; blk_start=7'd60; end
            7'd70: begin row_start=7'd63; col_idx=4'd7; blk_start=7'd60; end
            7'd71: begin row_start=7'd63; col_idx=4'd8; blk_start=7'd60; end
            // Row 8
            7'd72: begin row_start=7'd72; col_idx=4'd0; blk_start=7'd54; end
            7'd73: begin row_start=7'd72; col_idx=4'd1; blk_start=7'd54; end
            7'd74: begin row_start=7'd72; col_idx=4'd2; blk_start=7'd54; end
            7'd75: begin row_start=7'd72; col_idx=4'd3; blk_start=7'd57; end
            7'd76: begin row_start=7'd72; col_idx=4'd4; blk_start=7'd57; end
            7'd77: begin row_start=7'd72; col_idx=4'd5; blk_start=7'd57; end
            7'd78: begin row_start=7'd72; col_idx=4'd6; blk_start=7'd60; end
            7'd79: begin row_start=7'd72; col_idx=4'd7; blk_start=7'd60; end
            7'd80: begin row_start=7'd72; col_idx=4'd8; blk_start=7'd60; end
            default: begin row_start=7'd0; col_idx=4'd0; blk_start=7'd0; end
        endcase
    end
endmodule