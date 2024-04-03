module ALU(alu_opcode,A,B,O,zero_flag);
parameter size = 4;

input [size-1:0]A;
input [size-1:0]B;
input [2:0]alu_opcode;

output reg [size-1:0]O;
output reg zero_flag;



always@(alu_opcode)begin

	zero_flag = 0;
	
	case(alu_opcode)
		3'b001: assign O = 1;     // set
		3'b010: assign O = A + 1; // increment
		3'b011: assign O = A - 1; // Decrement
		3'b101: assign O = A;     // Load
		3'b110: assign O = A + B; // add
		3'b111: assign O = B;     // copy
		default : O = 4'b1111;   // for 000 and 100 the AlU won't do
	                         // anything and we don't want to put it to 0 because 
									 // we don't want to activate the zero_flag
	endcase

	if (O == 0)
		zero_flag = 1;
		
end

endmodule
