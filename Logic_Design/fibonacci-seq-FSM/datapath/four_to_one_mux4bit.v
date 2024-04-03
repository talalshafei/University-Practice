module four_to_one_mux4bit(A,B,C,D,S,O);

parameter size = 4;

input [size-1:0]A;
input [size-1:0]B;
input [size-1:0]C;
input [size-1:0]D;

input [1:0]S;

output reg [size-1:0]O;

always@(S)begin

	case(S)
		2'b00: assign O = A;
		2'b01: assign O = B;
		2'b10: assign O = C;
		2'b11: assign O = D;
		//default : assign O = 0;
	endcase
end

endmodule
