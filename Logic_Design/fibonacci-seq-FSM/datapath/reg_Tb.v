module reg_Tb();

reg [3:0] D;
reg Clk;

wire [3:0]Q;

four_bit_reg DUT(D,Clk,Q);

always #100 Clk= ~Clk;

initial begin
Clk=0;
D =0;#400;
D=1;#400;
D=2;#400;
D=3;#400;
D=4;#400;
D=5;#400;
D=6;#400;
D=7;#400;
D=8;#400;
D=9;#400;
D=10;#400;
D=11;#400;
D=12;#400;
D=13;#400;
D=14;#400;
D=15;#400;


end
endmodule
