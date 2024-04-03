module FIBO_FSM(START,ZERO_FLAG,RST,CLK,DONE,opcode,operand1,operand2,Clk_out);

(* keep = 1 *) input START,ZERO_FLAG,RST,CLK;

(* keep = 1 *) output reg[2:0]opcode;
(* keep = 1 *) output reg[1:0]operand1,operand2;
(* keep = 1 *) output reg DONE;
(* keep = 1 *) output Clk_out;

parameter S0 = 0, S1 = 1, S2 = 2, S3 = 3, S4 = 4, S5 = 5, S6 = 6, S7 = 7,
			 S8 = 8, S9 = 9, S10 = 10, S11 = 11, S12 = 12, S13 = 13, S14 = 14,
			 S15 = 15, S16 = 16;

(* keep = 1 *) reg [5:0]state,nextState; // size of the register 5 bits because of the 16 states

initial begin // initiallizing
	state = S0;
	nextState = S0;
end

always@(posedge CLK or posedge RST)begin // choose whether reset or proceed
	if(RST)//asychronous reset
		state <= S0;
	
	else
		state <= nextState;
	
end

always@(state)begin
	case(state)
		S0:  {DONE,opcode,operand1,operand2} = 0;
		S1:  {DONE,opcode,operand1,operand2} = 8'b01001100;
		S2:  {DONE,opcode,operand1,operand2} = 8'b00010000;
		S3:  {DONE,opcode,operand1,operand2} = 8'b00010100;
		S4:  {DONE,opcode,operand1,operand2} = 8'b01111111;
		S5:  {DONE,opcode,operand1,operand2} = 8'b01011111;
		S6:  {DONE,opcode,operand1,operand2} = 8'b01111000;
		S7:  {DONE,opcode,operand1,operand2} = 8'b01011000;
		S8:  {DONE,opcode,operand1,operand2} = 8'b01100001;
		S9:  {DONE,opcode,operand1,operand2} = 8'b01010001;
		S10: {DONE,opcode,operand1,operand2} = 8'b01110110;
		S11: {DONE,opcode,operand1,operand2} = 8'b01010100;
		S12: {DONE,opcode,operand1,operand2} = 8'b00111100;
		S13: {DONE,opcode,operand1,operand2} = 8'b01011100;
		S14: {DONE,opcode,operand1,operand2} = 8'b01111111;
		S15: {DONE,opcode,operand1,operand2} = 8'b01011100;
		S16: {DONE,opcode,operand1,operand2} = 8'b10001010;

	endcase

end

assign Clk_out = CLK; // to send the clock signals to the Datapath

always@(state or START or ZERO_FLAG)begin // computing next state
	nextState = S0;
	
	if(START == 0)//sychronous reset
		nextState <= S0;

	else begin
		case(state)
			S0: nextState = S1;
			S1: nextState = S2;
			S2: nextState = S3;
			S3: nextState = S4;
			S4:begin 
					if(!ZERO_FLAG)
						nextState = S5;
					else
						nextState = S1;
				end
			S5: nextState = S6;
			S6: nextState = S7;
			S7: nextState = S8;
		   S8: nextState = S9;
			S9: nextState = S10;
			S10: nextState = S11;
			S11: nextState = S12;
			S12: nextState = S13;
			S13: nextState = S14;
			S14:begin 
					if(ZERO_FLAG)
						nextState = S15;
					else
						nextState = S6;
				end
			S15: nextState = S16;
			S16: nextState = S16;	

		endcase
	end
end


endmodule