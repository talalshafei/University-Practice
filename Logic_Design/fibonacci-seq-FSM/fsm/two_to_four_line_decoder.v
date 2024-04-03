module two_to_four_line_decoder(Y,D);


(* keep = 1 *) input [1: 0] Y;
(* keep = 1 *) output reg[3:0] D;


always@(Y)begin
D=0;
D[Y]=1;

end

endmodule