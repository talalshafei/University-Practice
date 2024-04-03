module DFF(input D, input Clk, output reg Q);

always @ (posedge Clk)
	Q<=D;



endmodule
