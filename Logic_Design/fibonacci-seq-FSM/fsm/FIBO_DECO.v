module FIBO_DECO(opcode,operand1,operand2,alu_opcode,rd_addr1,rd_addr2,wrt_addr,wrt_en,load_data);

(* keep = 1 *) input [2:0]opcode;
(* keep = 1 *) input [1:0] operand1,operand2;

(* keep = 1 *) output [2:0]alu_opcode;
(* keep = 1 *) output [1:0] rd_addr1,rd_addr2,wrt_addr;
(* keep = 1 *) output wrt_en,load_data;

assign alu_opcode = opcode;
assign rd_addr1 = operand1;
assign rd_addr2  = operand2;

assign wrt_addr  = operand1;
assign wrt_en    = (opcode[2] || opcode[1] || opcode[0]) & ((~opcode[2]) || opcode[1] || (~opcode[0])); // if opcode  == 000 or 101
assign load_data = opcode[2] & (~opcode[1]) & (~opcode[0]); // if opcode == 100

endmodule