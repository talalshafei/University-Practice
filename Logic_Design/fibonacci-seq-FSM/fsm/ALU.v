module ALU(alu_opcode,A,B,O,zero_flag);
parameter size = 4;

(* keep = 1 *) input [size-1:0]A;
(* keep = 1 *) input [size-1:0]B;
(* keep = 1 *) input [2:0]alu_opcode;

(* keep = 1 *) output reg [size-1:0]O;
(* keep = 1 *) output reg zero_flag;



always@(alu_opcode)begin

	zero_flag = 0;
	
	case(alu_opcode)
		3'b001:  O = 1;     // set
		3'b010:  O = A + 1; // increment
		3'b011:  O = A - 1; // Decrement
		3'b101:  O = A;     // Load
		3'b110:  O = A + B; // add
		3'b111:  O = B;     // copy
		default :  O = 4'b1111;   // for 000 and 100 the ALU won't do
	                         // anything and we don't want to put it to 0 because 
									 // we don't want to activate the zero_flag
	endcase

	if (A == 0)
		zero_flag = 1;
		
end

endmodule
