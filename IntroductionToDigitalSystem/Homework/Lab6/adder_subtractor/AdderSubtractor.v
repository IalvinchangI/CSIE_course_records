
module AdderSubtractor	(a, b, select, sum, carry, overflow);
   // ripple-carry adder
	
	input [3:0] a;
	input [3:0] b;
	input select;
	
	output [3:0] sum;
	output carry;
	output overflow;
	
	
	wire [3:0] complement;
	wire [4:0] temp;
	
	
	// ripple
	assign temp[0] = select;
	assign complement[0] = select ^ b[0];
	FullAdder FA0(a[0], complement[0], temp[0], sum[0], temp[1]);
	
	assign complement[1] = select ^ b[1];
	FullAdder FA1(a[1], complement[1], temp[1], sum[1], temp[2]);
	
	assign complement[2] = select ^ b[2];
	FullAdder FA2(a[2], complement[2], temp[2], sum[2], temp[3]);
	
	assign complement[3] = select ^ b[3];
	FullAdder FA3(a[3], complement[3], temp[3], sum[3], temp[4]);
	
	
	// out
	assign carry = temp[4];
	assign overflow = temp[4] ^ temp[3];
	
endmodule
