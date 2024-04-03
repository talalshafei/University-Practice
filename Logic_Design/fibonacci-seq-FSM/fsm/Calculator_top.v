module Calculator_top(START,RST,CLK,count,DONE,data_out);

parameter size = 4;

(* keep = 1 *) input START,RST,CLK;
(* keep = 1 *) input [size-1:0]count;
(* keep = 1 *) output DONE;
(* keep = 1 *) output [size-1:0]data_out;


(* keep = 1 *) wire ZERO_FLAG,Clk_out,wrt_en,load_data;
(* keep = 1 *) wire [1:0]wrt_addr,rd_addr1,rd_addr2;
(* keep = 1 *) wire [2:0]alu_opcode;

FSM F(START,ZERO_FLAG,RST,CLK,DONE,Clk_out,alu_opcode,rd_addr1,rd_addr2,wrt_addr,wrt_en,load_data);

FIBO_DATAPATH D (wrt_addr,wrt_en,Clk_out,load_data,rd_addr1,rd_addr2,alu_opcode,count,data_out,ZERO_FLAG);


endmodule