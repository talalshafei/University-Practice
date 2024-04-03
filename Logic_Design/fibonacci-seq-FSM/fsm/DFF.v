module D_FF( D, Clk, Q);

(* keep = 1 *) input D,Clk;
(* keep = 1 *) output reg Q=0;

always @ (posedge Clk)
	Q<=D;



endmodule
