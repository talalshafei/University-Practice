module ALU_TB();

reg [3:0]A;
reg [3:0]B;
reg [2:0]alu_opcode;

wire [3:0]O;
wire zero_flag;

ALU DUT(alu_opcode,A,B,O,zero_flag);


always begin
alu_opcode = 3'b000; A= 4'b0001;B=4'b0000; #200;
alu_opcode = 3'b001; A= 4'b0010;B=4'b0001; #200;
alu_opcode = 3'b010; A= 4'b0011;B=4'b0010; #200;
alu_opcode = 3'b011; A= 4'b0100;B=4'b0011; #200;
alu_opcode = 3'b100; A= 4'b0101;B=4'b0100; #200;
alu_opcode = 3'b101; A= 4'b0110;B=4'b0101; #200;
alu_opcode = 3'b110; A= 4'b0111;B=4'b0110; #200;
alu_opcode = 3'b111; A= 4'b0000;B=4'b0111; #200;



end

endmodule
