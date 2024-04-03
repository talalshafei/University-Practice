module FIBO_DATAPATH(wrt_adder,wrt_en,Clk,load_data,rd_addr1,rd_addr2,alu_opcode,count,data_out,zero_flag);


parameter size = 4;

input wrt_en, Clk, load_data;
input [1:0]wrt_adder, rd_addr1,rd_addr2;
input [size-1:0] count;
input [2:0]alu_opcode;

output zero_flag;
output [size-1:0]data_out;


wire [size-1:0]decod;
wire [size-1:0]a;
wire [size-1:0]d0,d1,d2,d3,d4;
wire [size-1:0]Q0,Q1,Q2,Q3;
wire [size-1:0]w,A,B;



two_to_four_line_decoder D(wrt_adder,decod);

and an0(a[0],decod[0],wrt_en);
and an1(a[1],decod[1],wrt_en);
and an2(a[2],decod[2],wrt_en);
and an3(a[3],decod[3],wrt_en);

two_to_one_mux4bit mux20(Q0,w,a[0],d0);
two_to_one_mux4bit mux21(Q1,w,a[1],d1);
two_to_one_mux4bit mux22(Q2,w,a[2],d2);
two_to_one_mux4bit mux23(Q3,w,a[3],d3);


four_bit_reg REG0(d0,Clk,Q0);
four_bit_reg REG1(d1,Clk,Q1);
four_bit_reg REG2(d2,Clk,Q2);
four_bit_reg REG3(d3,Clk,Q3);


four_to_one_mux4bit mux40(Q0,Q1,Q2,Q3,rd_addr1,A);
four_to_one_mux4bit mux41(Q0,Q1,Q2,Q3,rd_addr2,B);

ALU A_unit(alu_opcode,A,B,d4,zero_flag);

wire negClk;
not n(negClk,Clk);
four_bit_reg REG4(d4,negClk,data_out);
//four_bit_reg REG4(d4,Clk,data_out);

two_to_one_mux4bit mux24(data_out,count,load_data,w);




endmodule
