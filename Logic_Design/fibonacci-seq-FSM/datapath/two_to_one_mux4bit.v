module two_to_one_mux4bit(A,B,S,O);

parameter size = 4;

input [size-1:0] A;
input [size-1:0]B;
input S;
output [size-1:0] O;


assign O = S ? B : A ;

endmodule
