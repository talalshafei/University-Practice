module four_bit_reg(D,Clk,Q);

parameter size = 4;

input [size - 1:0] D;
input Clk;

output [size-1:0]Q;

genvar i;

generate
	for(i=0;i<size;i=i+1)begin:top
		DFF dff(D[i],Clk,Q[i]);
	end

endgenerate

endmodule	