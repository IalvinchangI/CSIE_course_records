module FIFO_sync(
    input             clk     ,
    input             rst     ,
    input             wr_en   ,
    input             rd_en   ,
    input       [7:0] data_in ,
    output            full    ,
    output            empty   ,
    output reg  [7:0] data_out  // output buffer (for stable output)
);

// FIFO memory
reg [7:0] FIFO_memory [32:1];

// FIFO counter (0 ~ 32)
reg [5:0] FIFO_counter;



/***************************** Control Unit *****************************/

////////// control signal //////////
reg counter_enable;
reg counter_updown;
reg shift_right_enable;
reg output_enable;

parameter COUNTER_UP       = 1'b0;
parameter COUNTER_DOWN     = 1'b1;
parameter COUNTER_DONTCARE = 1'bx;

always @(*) begin
    case ({(wr_en & ~full), (rd_en & ~empty)})
        2'b01: begin  // only read
            counter_enable = 1'b1;
            counter_updown = COUNTER_DOWN;
            shift_right_enable = 1'b0;
            output_enable = 1'b1;
        end
        2'b10: begin  // only write
            counter_enable = 1'b1;
            counter_updown = COUNTER_UP;
            shift_right_enable = 1'b1;
            output_enable = 1'b0;
        end
        2'b11: begin  // read & write
            counter_enable = 1'b0;
            counter_updown = COUNTER_DONTCARE;
            shift_right_enable = 1'b1;
            output_enable = 1'b1;
        end
        default: begin  // case 00: nothing
            counter_enable = 1'b0;
            counter_updown = COUNTER_DONTCARE;
            shift_right_enable = 1'b0;
            output_enable = 1'b0;
        end
    endcase
end



/***************************** Datapath *****************************/

////////// empty & full //////////
assign empty = ~|FIFO_counter;  // empty = (FIFO_counter == 0)
assign full = FIFO_counter[5];  // full = (FIFO_counter == 32)


////////// empty & full //////////
reg [5:0] next_FIFO_counter;

always @(*) begin
    if (counter_enable == 1'b0) begin
        next_FIFO_counter = FIFO_counter;
    end
    else begin
        next_FIFO_counter = FIFO_counter + (
            ({6{counter_updown}} ^ 6'd1) + counter_updown  // adder / sudtracter
        );
    end
end


////////// counter //////////
always @(posedge clk or posedge rst) begin
    if (rst == 1) begin
        FIFO_counter <= 0;
    end
    else begin
        FIFO_counter <= next_FIFO_counter;
    end
end


////////// memory //////////
integer i;  // for loop

always @(posedge clk or posedge rst) begin
    if (rst == 1) begin
        for (i = 1; i <= 32; i = i + 1) FIFO_memory[i] <= 0;
        data_out <= 0;
    end
    else begin
        // shift right
        if (shift_right_enable == 1) begin
            FIFO_memory[1] <= data_in;
            for (i = 1; i < 32; i = i + 1) FIFO_memory[i + 1] <= FIFO_memory[i];
        end
        else begin
            // nothing
        end

        // output
        if (output_enable == 1) begin
            data_out <= FIFO_memory[FIFO_counter];
        end
        else begin
            data_out <= 0;
        end
    end
end

endmodule