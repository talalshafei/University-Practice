module data_path_TB();

reg wrt_en, Clk, load_data;
reg [1:0]wrt_adder, rd_addr1,rd_addr2;
reg [3:0] count;
reg [2:0]alu_opcode;

wire zero_flag;
wire [3:0]data_out;

FIBO_DATAPATH DUT(wrt_adder,wrt_en,Clk,load_data,rd_addr1,rd_addr2,alu_opcode,count,data_out,zero_flag);

always #100 Clk = ~Clk;
// we are going to evaluate F(7) = 13 where count =5 (because the machine starts calculating after index 2)to test the data_path

integer i;
initial begin
Clk = 1;
count = 4;
// load count to R3 (R4 in manual)
		wrt_adder =3 ; wrt_en =1 ; load_data =1 ; rd_addr1 =0 ; rd_addr2 =0 ; alu_opcode= 3'b000;#500;

		// set R0 to num1 (R1 in the manual)
		wrt_adder =0 ; wrt_en =1 ; load_data =0 ; rd_addr1 =0 ; rd_addr2 =0 ; alu_opcode= 3'b001;#500;

		// set R1 to num1 (R2 in the manual)
		wrt_adder =1 ; wrt_en =1 ; load_data =0 ; rd_addr1 =1 ; rd_addr2 =0 ; alu_opcode= 3'b001;#500;
		
		
			
			
			for(i=0;i<4;i=i+1)begin
				// assign R2 = R0 (R2 is R3 manual)
				wrt_adder =2 ; wrt_en =1 ; load_data =0 ; rd_addr1 =0 ; rd_addr2 =0 ; alu_opcode= 3'b111;#300;wrt_en=0;#200;

				// assign R0 = R0 + R1
				wrt_adder =0 ; wrt_en =1 ; load_data =0 ; rd_addr1 =0 ; rd_addr2 =1 ; alu_opcode= 3'b110;#300;wrt_en=0;#200;

				// assign R1 = R2 
				wrt_adder =1 ; wrt_en =1 ; load_data =0 ; rd_addr1 =1 ; rd_addr2 =2 ; alu_opcode= 3'b111;#300;wrt_en=0;#200;

				// decreament count that was stored in R3
				wrt_adder =3 ; wrt_en =1 ; load_data =0 ; rd_addr1 =3 ; rd_addr2 =0 ; alu_opcode= 3'b011; #300;wrt_en=0;#200;
			end
			
					
		
		
end

endmodule