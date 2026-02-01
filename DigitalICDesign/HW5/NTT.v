// `include "BU.v"

module  NTT(
    input             clk, 
    input             rst, 
    output reg        input_ready,
    input             input_valid,
    input      [22:0] input_data,
    output reg [7:0]  tf_addr,
    input      [22:0] tf_data,
    output reg        output_valid,
    output reg [22:0] output_data
);

// memory
reg [22:0] memory [255:0];


// counter
reg [10:0] next_count;
reg [10:0] current_count;
always @(posedge clk or posedge rst) begin
    if (rst == 1) begin
        current_count <= 0;
    end
    else begin
        current_count <= next_count;
    end
end


// main control
reg [3:0] stage;  // 1, 2, ... , 8, 0(idle)
reg [6:0] NTT_count;
reg       input_work;
reg [7:0] input_count;
reg       output_work;
reg [7:0] output_count;
always @(*) begin
    if (input_valid == 0 && current_count == 0) begin  // reset
        stage = 0;
        NTT_count = 0;
        input_work = 0;
        input_count = 0;
        output_work = 0;
        output_count = 0;
        next_count = 0;
    end
    else begin
        next_count   = current_count + 1;
        NTT_count    = current_count[6:0];
        input_count  = current_count[7:0];
        output_count = current_count[7:0] - 8'd3;
        
        stage        = 0;
        input_work   = 0;
        output_work  = 0;
        
        if (current_count <= 1023) begin
            case (current_count[9:7]) 
                3'd0: begin // 0 ~ 127
                    stage       = 0;
                    input_work  = 1;
                    output_work = 0;
                end
                3'd1: begin // 128 ~ 255
                    stage       = 1;
                    input_work  = 1;
                    output_work = 0;
                end
                3'd2: stage = 2;  // 256 ~ 383
                3'd3: stage = 3;  // 384 ~ 511
                3'd4: stage = 4;  // 512 ~ 639
                3'd5: stage = 5;  // 640 ~ 767
                3'd6: stage = 6;  // 768 ~ 895
                3'd7: stage = 7;  // 896 ~ 1023
                default: begin    // else
                    stage = 0; 
                end
            endcase
        end
        else begin
            if (current_count == 1024) begin // idle (1023 + 1)
                stage       = 0;
                input_work  = 0;
                output_work = 0;
            end
            else if (current_count <= 1026) begin // NTT 8 (1023 + 1 + 2)
                stage       = 8;
                NTT_count   = current_count[6:0] - 1'b1; 
                input_work  = 0;
                output_work = 0;
            end
            else if (current_count <= 1152) begin // NTT 8 + output
                stage       = 8;
                NTT_count   = current_count[6:0] - 1'b1;
                input_work  = 0;
                output_work = 1;
            end
            else if (current_count <= 1282) begin // output
                stage       = 0;
                input_work  = 0;
                output_work = 1;
            end
            else begin // Reset / Finish
                next_count  = 0;
                stage       = 0;
                input_work  = 0;
                output_work = 0;
            end
        end
    end
end


// NTT control
reg NTT_operate;
reg [7:0] a_addr;
reg [7:0] b_addr;
reg       use_input;
// m = tf_addr
always @(*) begin
    case (stage)
        4'd1: begin
            NTT_operate = 1;
            a_addr = {1'b0, NTT_count};
            b_addr = {1'b1, NTT_count};
            use_input = 1;
            tf_addr = 8'd1;
        end
        4'd2: begin
            NTT_operate = 1;
            a_addr = {NTT_count[6], 1'b0, NTT_count[5:0]};
            b_addr = {NTT_count[6], 1'b1, NTT_count[5:0]};
            use_input = 0;
            tf_addr = {7'd1, NTT_count[6]};
        end
        4'd3: begin
            NTT_operate = 1;
            a_addr = {NTT_count[6:5], 1'b0, NTT_count[4:0]};
            b_addr = {NTT_count[6:5], 1'b1, NTT_count[4:0]};
            use_input = 0;
            tf_addr = {6'd1, NTT_count[6:5]};
        end
        4'd4: begin
            NTT_operate = 1;
            a_addr = {NTT_count[6:4], 1'b0, NTT_count[3:0]};
            b_addr = {NTT_count[6:4], 1'b1, NTT_count[3:0]};
            use_input = 0;
            tf_addr = {5'd1, NTT_count[6:4]};
        end
        4'd5: begin
            NTT_operate = 1;
            a_addr = {NTT_count[6:3], 1'b0, NTT_count[2:0]};
            b_addr = {NTT_count[6:3], 1'b1, NTT_count[2:0]};
            use_input = 0;
            tf_addr = {4'd1, NTT_count[6:3]};
        end
        4'd6: begin
            NTT_operate = 1;
            a_addr = {NTT_count[6:2], 1'b0, NTT_count[1:0]};
            b_addr = {NTT_count[6:2], 1'b1, NTT_count[1:0]};
            use_input = 0;
            tf_addr = {3'd1, NTT_count[6:2]};
        end
        4'd7: begin
            NTT_operate = 1;
            a_addr = {NTT_count[6:1], 1'b0, NTT_count[0]};
            b_addr = {NTT_count[6:1], 1'b1, NTT_count[0]};
            use_input = 0;
            tf_addr = {2'd1, NTT_count[6:1]};
        end
        4'd8: begin
            NTT_operate = 1;
            a_addr = {NTT_count, 1'b0};
            b_addr = {NTT_count, 1'b1};
            use_input = 0;
            tf_addr = {1'd1, NTT_count};
        end
        default begin  // idle
            NTT_operate = 0;
            a_addr = 0;
            b_addr = 0;
            use_input = 0;
            tf_addr = 0;
        end
    endcase
end

// Input control
reg [7:0] input_addr;
always @(*) begin
    if (input_work == 0) begin  // idle
        input_ready = 0;
        input_addr = 0;
    end
    else begin  // write memory
        input_ready = 1;
        input_addr = input_count;  // TODO use sequential?
    end
end

// Output control
reg [7:0] output_addr;
reg temp_output_valid;
always @(*) begin
    if (output_work == 0) begin  // idle
        temp_output_valid = 0;
        output_addr = 0;
    end
    else begin  // read memory
        temp_output_valid = 1;
        output_addr = output_count;  // TODO use sequential?
    end
end



// ******************************* DATAPATH ************************************ //

// NTT memory read
wire [22:0] butter_a;
wire [22:0] butter_b;
assign butter_a = memory[a_addr];
assign butter_b = (use_input == 1) ? input_data : memory[b_addr];

// access Twiddle Factor ROM
// implement at NTT control

// Butterfly Unit
wire [22:0] NTTa;
wire [22:0] NTTb;
BU bu(
    .clk(clk), .rst(rst), 
    .X(butter_a), .Y(butter_b), .TF(tf_data),
    .A(NTTa), .B(NTTb)
);

// memory write temp register
reg [7:0] a_addr_reg;
reg [7:0] b_addr_reg;
reg       NTT_operate_reg;
always @(posedge clk or posedge rst) begin
    if (rst == 1) begin
        a_addr_reg <= 0;
        b_addr_reg <= 0;
        NTT_operate_reg <= 0;
    end
    else begin
        a_addr_reg <= a_addr;
        b_addr_reg <= b_addr;
        NTT_operate_reg <= NTT_operate;
    end
end





// memory write
always @(posedge clk) begin
    if (NTT_operate_reg == 1) begin  // NTT memory write
        memory[a_addr_reg] <= NTTa;
        memory[b_addr_reg] <= NTTb;
    end
    else if (input_work == 1) begin  // input memory write
        memory[input_addr] <= input_data;
    end
end

// output memory read
always @(posedge clk) begin
    output_valid <= temp_output_valid;
    output_data <= memory[output_addr];
end

endmodule
