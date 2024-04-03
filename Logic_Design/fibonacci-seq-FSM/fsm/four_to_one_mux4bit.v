module four_to_one_mux4bit(A,B,C,D,S,O);

parameter size = 4;

(* keep = 1 *) input [size-1:0]A;
(* keep = 1 *) input [size-1:0]B;
(* keep = 1 *) input [size-1:0]C;
(* keep = 1 *) input [size-1:0]D;

(* keep = 1 *) input [1:0]S;

(* keep = 1 *) output reg [size-1:0]O;

always@(S)begin

	case(S)
		2'b00: O = A;
		2'b01: O = B;
		2'b10: O = C;
		2'b11: O = D;
		//default :  O = 0;
	endcase
end

endmodule
