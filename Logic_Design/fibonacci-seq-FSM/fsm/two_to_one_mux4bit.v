module two_to_one_mux4bit(A,B,S,O);

parameter size = 4;

(* keep = 1 *) input [size-1:0] A;
(* keep = 1 *) input [size-1:0]B;
(* keep = 1 *) input S;
(* keep = 1 *) output [size-1:0] O;


assign O = S ? B : A ;

endmodule
