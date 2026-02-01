`include "FIFO_sync.v"

`timescale 1ns / 10ps
module FourBankFIFO(
    input               clk         ,
    input               rst         ,
    input               wr_en_M0    ,
    input      [7:0]    data_in_M0  ,
    input               rd_en_M0    ,
    input      [1:0]    rd_id_M0    ,
    input               wr_en_M1    ,
    input      [7:0]    data_in_M1  ,
    input               rd_en_M1    ,
    input      [1:0]    rd_id_M1    ,
    output reg [7:0]    data_out_M0 ,
    output reg [7:0]    data_out_M1 ,
    output reg          valid_M0    ,
    output reg          valid_M1
);

// FIFO (wire)
wire [3:0] bank_full;
wire [3:0] bank_empty;
wire [7:0] bank_data_out [3:0];
wire [7:0] read_bus;
reg [7:0] write_bus;

// control signal (wire)
wire rd_selector;
wire wr_selector;
wire rd_en;
wire [1:0] rd_id;
wire wr_en;
wire next_output_TF;
wire [1:0] next_output_bank;
wire [3:0] bank_rd_en;
wire [3:0] bank_wr_en;

// state
reg output_TF;
reg [1:0] output_bank;


/***************************** Control Unit *****************************/

MasterArbiter master_arbiter(
    .clk(clk), .rst(rst), 
    .wr_en_M0(wr_en_M0), .rd_en_M0(rd_en_M0), .rd_id_M0(rd_id_M0), 
    .wr_en_M1(wr_en_M1), .rd_en_M1(rd_en_M1), .rd_id_M1(rd_id_M1), 
    .rd_master(rd_selector), .wr_master(wr_selector), .rd_en(rd_en), .rd_id(rd_id), .wr_en(wr_en)
);

BankArbiter bank_arbiter (
    .clk(clk), .rst(rst), 
    .rd_en(rd_en), .rd_id(rd_id), .wr_en(wr_en), .bank_empty(bank_empty), .bank_full(bank_full), 
    .out_TF(next_output_TF), .out_bank(next_output_bank), .bank_rd_en(bank_rd_en), .bank_wr_en(bank_wr_en)
);

// state
always @(posedge clk or posedge rst) begin
    if (rst == 1) begin
        output_TF <= 0;
        output_bank <= 0;
    end
    else begin
        output_TF <= next_output_TF;
        output_bank <= next_output_bank;
    end
end



/***************************** Datapath *****************************/

// FIFO output
assign read_bus = bank_data_out[output_bank];

// read selector
always @(*) begin
    if (output_TF == 1) begin
        if (rd_selector == 0) begin
            valid_M0 = 1;
            data_out_M0 = read_bus;
            valid_M1 = 0;
            data_out_M1 = 0;
        end
        else begin
            valid_M0 = 0;
            data_out_M0 = 0;
            valid_M1 = 1;
            data_out_M1 = read_bus;
        end
    end
    else begin
        valid_M0 = 0;
        data_out_M0 = 0;
        valid_M1 = 0;
        data_out_M1 = 0;
    end
end

// write selector
always @(*) begin
    if (wr_selector == 0) begin
        write_bus = data_in_M0;
    end
    else begin
        write_bus = data_in_M1;
    end
end

// FIFO
FIFO_sync bank0(
    .clk(clk), .rst(rst), 
    .wr_en(bank_wr_en[0]), .rd_en(bank_rd_en[0]), .data_in(write_bus), 
    .full(bank_full[0]), .empty(bank_empty[0]), .data_out(bank_data_out[0])
);
FIFO_sync bank1(
    .clk(clk), .rst(rst), 
    .wr_en(bank_wr_en[1]), .rd_en(bank_rd_en[1]), .data_in(write_bus), 
    .full(bank_full[1]), .empty(bank_empty[1]), .data_out(bank_data_out[1])
);
FIFO_sync bank2(
    .clk(clk), .rst(rst), 
    .wr_en(bank_wr_en[2]), .rd_en(bank_rd_en[2]), .data_in(write_bus), 
    .full(bank_full[2]), .empty(bank_empty[2]), .data_out(bank_data_out[2])
);
FIFO_sync bank3(
    .clk(clk), .rst(rst), 
    .wr_en(bank_wr_en[3]), .rd_en(bank_rd_en[3]), .data_in(write_bus), 
    .full(bank_full[3]), .empty(bank_empty[3]), .data_out(bank_data_out[3])
);

endmodule



module MasterArbiter (
    input            clk      ,
    input            rst      ,
    input            wr_en_M0 ,
    input            rd_en_M0 ,
    input  [1:0]     rd_id_M0 ,
    input            wr_en_M1 ,
    input            rd_en_M1 ,
    input  [1:0]     rd_id_M1 ,
    output reg       rd_master,
    output reg       wr_master,
    output reg       rd_en    ,
    output reg [1:0] rd_id    ,
    output reg       wr_en
);
    // state
    reg previous_master;

    /***************************** Control Unit *****************************/

    // next state & control signal
    reg next_previous_master;
    always @(*) begin
        casex ({previous_master, (rd_en_M1 | wr_en_M1), (rd_en_M0 | wr_en_M0)})
            3'bx01: begin  // master 0 operate
                next_previous_master = 0;
            end
            3'bx10: begin  // master 1 operate
                next_previous_master = 1;
            end
            3'b011: begin  // master 0 & 1 operate, but only master 1 can operate
                next_previous_master = 1;
            end
            3'b111: begin  // master 0 & 1 operate, but only master 0 can operate
                next_previous_master = 0;
            end
            default: begin  // case x00: nothing
                next_previous_master = previous_master;
            end
        endcase
    end

    // state
    always @(posedge clk or posedge rst) begin
        if (rst == 1) begin 
            previous_master <= 1;
        end
        else begin
            previous_master <= next_previous_master;
        end
    end

    // control signal
    always @(*) begin
        rd_master = previous_master;
        // next_previous_master = current
        if (next_previous_master == 0) begin
            rd_en = rd_en_M0;
            rd_id = rd_id_M0;
            wr_en = wr_en_M0;
            wr_master = 0;
        end
        else begin
            rd_en = rd_en_M1;
            rd_id = rd_id_M1;
            wr_en = wr_en_M1;
            wr_master = 1;
        end
    end
endmodule


module BankArbiter (
    input            clk       ,
    input            rst       ,
    input            rd_en     ,
    input  [1:0]     rd_id     ,
    input            wr_en     ,
    input  [3:0]     bank_empty,
    input  [3:0]     bank_full ,
    output reg [3:0] bank_rd_en,
    output reg [3:0] bank_wr_en,
    output reg       out_TF    ,
    output reg [1:0] out_bank
);
    // state
    reg [1:0] in_bank;  // point to next bank

    /***************************** Control Unit *****************************/
    // next state
    reg [1:0] next_in_bank;
    // out_TF  // indicate whether it has to output
    // out_bank  // which bank should be output
    always @(*) begin
        // read
        if (rd_en == 1 && bank_empty[rd_id] != 1) begin
            out_TF = 1;
            out_bank = rd_id;
        end
        else begin
            out_TF = 0;
            out_bank = 0;
        end

        // write
        if (wr_en == 1 && bank_full[in_bank] != 1) begin
            next_in_bank = in_bank + 1;
        end
        else begin
            // if input bank is full, it should not update in_bank
            next_in_bank = in_bank;
        end
    end

    // state
    always @(posedge clk or posedge rst) begin
        if (rst == 1) begin
            in_bank <= 0;
        end
        else begin
            in_bank <= next_in_bank;
        end
    end

    // control signal
    always @(*) begin
        // read
        if (out_TF == 1) begin
            case (out_bank)
                2'd1:    bank_rd_en = 4'b0010;
                2'd2:    bank_rd_en = 4'b0100;
                2'd3:    bank_rd_en = 4'b1000;
                default: bank_rd_en = 4'b0001;
            endcase
        end
        else begin
            bank_rd_en = 4'b0000;
        end
        // write
        if (wr_en == 1) begin
            case (in_bank)
                2'd1:    bank_wr_en = 4'b0010;
                2'd2:    bank_wr_en = 4'b0100;
                2'd3:    bank_wr_en = 4'b1000;
                default: bank_wr_en = 4'b0001;
            endcase
        end
        else begin
            bank_wr_en = 4'b0000;
        end
    end
endmodule
