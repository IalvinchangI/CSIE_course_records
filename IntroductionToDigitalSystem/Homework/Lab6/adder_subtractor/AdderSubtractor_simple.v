
module AdderSubtractor_simple (a,b,c,s,sel);

input [3:0] a;
input [3:0] b;
input sel;

output reg [3:0] s;
output reg c;

always@(*) begin
    if(sel==1'd1)begin
        {c,s}=a-b;
    end
    else begin
        {c,s}=a+b;
    end
end

endmodule
