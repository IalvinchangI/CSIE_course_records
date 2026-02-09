
module SpecializedMultiplier(in, out);
	
	input [3:0] in;    // switch
	output [6:0] out;  // seven-segment display
	
	wire [3:0] calculate_result;
	
	
	calculator (in, calculate_result);
	translate  (calculate_result, out);
	
	
endmodule
