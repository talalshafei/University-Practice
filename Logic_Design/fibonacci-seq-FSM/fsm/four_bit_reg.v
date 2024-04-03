module four_bit_reg(D,Clk,Q);

parameter size = 4;

(* keep = 1 *) input [size - 1:0] D;
(* keep = 1 *) input Clk;

(* keep = 1 *) output [size-1:0]Q;

genvar i;

generate
	for(i=0;i<size;i=i+1)begin:top
		D_FF dff(D[i],Clk,Q[i]);
	end

endgenerate

endmodule	