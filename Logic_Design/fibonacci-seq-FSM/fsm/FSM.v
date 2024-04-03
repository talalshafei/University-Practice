module FSM(START,ZERO_FLAG,RST,CLK,DONE,Clk_out,alu_opcode,rd_addr1,rd_addr2,wrt_addr,wrt_en,load_data);

(* keep = 1 *) input START,ZERO_FLAG,RST,CLK;

(* keep = 1 *) output DONE,Clk_out;

(* keep = 1 *) output [2:0]alu_opcode;
(* keep = 1 *) output [1:0] rd_addr1,rd_addr2,wrt_addr;
(* keep = 1 *) output wrt_en,load_data;

(* keep = 1 *) wire [2:0]opcode;
(* keep = 1 *) wire [1:0]operand1,operand2;


FIBO_FSM f(START,ZERO_FLAG,RST,CLK,DONE,opcode,operand1,operand2,Clk_out);

FIBO_DECO d(opcode,operand1,operand2,alu_opcode,rd_addr1,rd_addr2,wrt_addr,wrt_en,load_data);

endmodule