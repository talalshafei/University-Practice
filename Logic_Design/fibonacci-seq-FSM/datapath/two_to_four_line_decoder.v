module two_to_four_line_decoder(Y,D);


input [1: 0] Y;
output reg[3:0] D;


always@(Y)begin
D=0;
D[Y]=1;

end

endmodule