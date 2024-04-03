;
; module_2.asm
;
; Created: 5/1/2023 6:25:38 PM
; Author : Talal Shafei and Noor Ul Zain
;


RJMP start
.INCLUDE "M128DEF.INC"

.EQU ZEROS = 0X00
.EQU ONES = 0XFF
.EQU POLYG = 0b11010100
.EQU IMEM_START = 0x121; to start after the stack
.EQU XMEM_END = 0x18FF ; 0x10FF + 800(2KB) :
; 0x7FF 0111 1111 1111  the first 8 bits will be sent through 
; porta and the other 3 bits will be sent through portc[0:2]

.EQU PACKET_IN = PINB
.EQU PACKET_OUT = PORTD
.EQU READY_OUT = PORTE

.DEF TOS = R2 ; since TOS will only contain one packet at any time
.DEF IS_TOS_EMPTY = R30 ; a flag to see if TOS is empty (1: empty, 0:full)
; it will give warning since ZL is R30 but it in not a problem since we are not using Z reg

.DEF FAIL_PASS = R25 ;(0: failed, 1: passed)

.DEF CAPTURED = R5


.MACRO PUSH_TOS
	CLR IS_TOS_EMPTY
	MOV TOS, @0
.ENDMACRO

.MACRO POP_TOS
	LDI IS_TOS_EMPTY, 0X01
	MOV @0 , TOS
.ENDMACRO


;CODE
.CSEG
.ORG 0X0050
start:

	; Memory partition
	
	; Initialize Stack pointer so we can use subroutines with no problem
	; Note 0x120 for the stack to have at least 20 bytes
	LDI R16, LOW(0x120)
	OUT SPL, R16
	LDI R16, HIGH(0x120)
	OUT SPH, R16

	; Initialize X as a pointer to the position in the Log file
	
	LDI XL, LOW(IMEM_START)
	LDI XH, HIGH(IMEM_START)

	; Initialize for XMEM
	LDI R16, (1<<SRE) ; activate XMEM
	OUT MCUCR, R16

	LDI R16, (1<<XMM2)|(1<<XMM0) ; so we can release PC7 - PC3
	STS XMCRB, R16
	
	; Initialize C
	; C[3] is input for Start/Stop
	; C[4] is input for Memory Dump
	; C[5] is input for Last Entry
	; C[6] is input for Recieve flag push down button
	; C[7] is output for Ready (LED)
	; 0b1000 0xxx -> 0x80
	LDI R16, 0x80
	OUT DDRC, R16 
	
	LDI R16, ZEROS
	OUT DDRB, R16 ; PIN B is input for PACKET_IN

	; Initialize outputs
	LDI R16, ONES 
	OUT DDRD, R16 ; Port D is output for PACKET_OUT
	OUT DDRE, R16 ; Port E is output for READY_OUT


	

initialize:
	CBI PORTC, 7 ; make sure the led is off 
	; initialize
	CALL INIT

main:
	;CALL DELAY
	;CALL DELAY
	;CALL DELAY
	;CALL DELAY ; to see the difference on proteus
	
	CBI PORTC, 7; turning off the led

	;CALL DELAY 
	;CALL DELAY
	;CALL DELAY
	;CALL DELAY

	CALL SERVICE_OUT

	; check start/stop
	SBIS PINC,3
	RJMP main ; if it stop go back to main

	SBI PORTC, 7 ; turn on Ready led

	SBIS PINC, 6 ; receive push down button
	RJMP main
wait_to_let_go_of_the_push_down_button:
	SBIC PINC, 6
	RJMP wait_to_let_go_of_the_push_down_button

	CBI PORTC, 7 ; turn off Ready led because now capturing Packet_in

	IN CAPTURED, PACKET_IN ; captture packet_in

	SBRS CAPTURED , 7 ; if the packet is data skip
	RJMP command_packet_in

	; is stack (TOS) empty?
	SBRS IS_TOS_EMPTY, 0; if yes skip the popping
	POP_TOS R16; R16 temp to discard what came out of the TOS

	PUSH_TOS CAPTURED ; push the data packet to TOS
	JMP main


command_packet_in:
	; TOS has data packet?
	SBRC TOS, 7 ; if it is set then it means it is a data packet
	JMP tos_has_data_packet

	; since there is no data packet in TOS we will do crc3_check
	CALL CRC3_CHECK

	SBRS FAIL_PASS,0
	JMP fail
	; since there is a data packet we need to do check11
	; default behavior is pass
	MOV R19, CAPTURED
	ANDI R19,  0x60 ; mask the command input (0b0110 0000)
	CPI R19, 0x40; check if acknowledge (0b0100 0000)
	BRNE check_if_repeat

	; it is acknowledge, then empty the stack and go back to main
	; is stack (TOS) empty?
	SBRS IS_TOS_EMPTY,0; if yes skip the popping
	POP_TOS R16; R16 temp to discard what came out of the TOS
	JMP main

check_if_repeat:
	CPI R19, 0x60 ; check if repeat (0b0110 0000)
	BREQ it_is_repeat
	JMP main ; if it not repeat go back to main

it_is_repeat:
	; is stack (TOS) empty?
	SBRC IS_TOS_EMPTY,0; if is not empty skip jumping directly to main
	JMP main
	; not empty then pop into R17 and transmit
	POP_TOS R17
	CALL TRANSMIT
	JMP main


tos_has_data_packet:
	CALL CRC11_CHECK
	
	CPI FAIL_PASS, 0x01
	BREQ passed
	POP_TOS R16; R16 temperoray to discard TOS
	JMP fail

passed:
	; check if it is a log request
	MOV R19, CAPTURED
	ANDI R19,  0x60 ; mask the command input (0b0110 0000)
	CPI R19, 0x20; check if is log request (0b0010 0000)
	BREQ it_is_log
	JMP main

it_is_log:
	; it is log, then log the data that was in TOS
	ST X+, TOS
	CALL CHECK_MEMORY

	POP_TOS R16; to discard the value in the TOS

	LDI R17, 0x40 ; laod acknowledge
	CALL CRC3 ; generate the crc for the acknowledge
	PUSH_TOS R17 ; keep it in stack incase sensor asked to resent it
	CALL TRANSMIT
	
	JMP main



fail:
	CALL REPEAT_REQUEST
	JMP main





TRANSMIT:
	OUT PACKET_OUT, R17
	RET

INIT:
	LDI R17, 0X00 ; command reset request
	CALL CRC3 ; generate crc in R17
	CALL TRANSMIT ; send R17 to packet_out, made as a macro to have more flexibility when calling and it is one line anyway
	PUSH_TOS R17 ; push the value to TOS
	RET


CHECK_MEMORY:
	; if they are equal check the lower byte else return where you left off
	LDI R16, HIGH(XMEM_END)
	CP XH, R16
	BREQ maybe_full
	RET
maybe_full:
	; if XL > MEM_END Lower byte reset it else return
	LDI R16, LOW(XMEM_END)
	SUBI R16, 20 ; because last 20 bytes are for stack
	CP R16, XL
	BRMI reset
	RET
reset:
	; reset to the beginning of internal sram again in round-robin fashion
	LDI XL, LOW(IMEM_START)
	LDI XH, HIGH(IMEM_START)
	RET


SERVICE_OUT:
	; memory dump check
	SBIC PINC, 4 ; if memory_dump is not active skip jumping to it
	RJMP memory_dump

	; last entry check
	SBIS PINC, 5 ; if we reach here that means memory dump is not active
	; if last entry FLAG is set then skip and dont return now
	RET

	; read last entry in the log file
	MOVW Y,X ; so we dont affect the memory pointer
	LD R16, -Y
	OUT READY_OUT, R16
	RET

	
memory_dump:
; so we dont discard all the bytes in the memory when we dump them
	MOVW Y, X ; maybe it should be MOVW R28, R26
loop_dump:
	
	CPI YL, LOW(IMEM_START)
	BRNE not_finished
	CPI YH, HIGH(IMEM_START)
	BREQ finished

not_finished:
	LD R16, -Y
	OUT READY_OUT, R16
	CALL DELAY
	CALL DELAY ; for proteus
	CALL DELAY
	CALL DELAY
	RJMP loop_dump

finished:
	RET



REPEAT_REQUEST:
	LDI R17, 0x60 ; Repeat Request
	CALL CRC3
	CALL TRANSMIT
	RET

DELAY:
	LDI R16, 0xFF
delay_loop:
	DEC R16
	NOP
	BRNE delay_loop
	RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CRC3:
	; R17 is the param
	MOV R21, R17     ; copying the data input into R21 
     
    ANDI R21, 0b11100000  ; bit masking as we are calculating the CRC3 	    
    ANDI R17, 0b11100000  ; bit masking as we are calculating the CRC3 


    LDI R16, POLYG   ; polynomial G with big endian ->even if little endian, we can just shift it, does not matter
	LDI R22, 0  ; will be used as shift counter

div:
	SBRC R21, 7 ; if the first bit is cleared, skip xor and shift 
	EOR R21, R16  
	SBRS R21, 7  ;if the xor result's MSB is not set, we shift 
	JMP shift     
     
shift:
	LSL R21
	INC R22      ; keep counting the shifts
	CPI R22, 3   ; 3 because we are creating the CRC code for the first 3 bits
	BREQ exit    ; if shifted 3 times, message is over; exit
	SBRS R21, 7  ; check MSB again after shift and if not set, loop
	BRNE shift
	
	
	BRNE div     ; Loop back to div 

    JMP exit

exit: 
	ROR R21         ;
	ROR R21         ; since the first 5 bits are CRC and I want to add it to R17 (original)  
	ROR R21         ;        
	Add R17, R21    ; append CRC to input
	RET
  


CRC3_CHECK:
	LDI FAIL_PASS, 0x00 ; set the falg to fail
	MOV R17, CAPTURED
	MOV R20, CAPTURED

	CALL CRC3 
	 
	CP R20, R17   ; R17 is used by CRC3 and should have the appended CRC to input
	BREQ not_corrupted       
	RET

not_corrupted: 
	LDI FAIL_PASS, 0x01 ; if it not corrupted set the flag to pass
	RET

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CRC11:

	;R17 has the Higher byte and R18 the Lower byte -> R17:R18
	
	MOV R21, R17		; copying the data input into R21 -> in case original data is needed again

	LDI R16, POLYG ; polynomial G with big endian (even if little endian, we can shift)
	

	LDI R22, 0  ; will be used as shift counter for lower byte
	LDI R23, 0  ; will always stay 0
	LDI R24, 0  ; will be used a shift counter for total shifts 
	


div11:
	SBRC R21, 7 ; if the first bit is cleared, skip xor and shift (v imp)
	EOR R21, R16  
	SBRS R21, 7  ;if the xor result's MSB is not set, we shift 
	JMP shift11     
     
shift11: 
	LSL R21

	
	CPI R22, 3    ;as we only want 3 bits we want from lower byte
	BREQ go_here
	LSL R18
	ADC R21, R23  ;add the shifted lower byte's carry to R21
	INC R22 
	

go_here:
	INC R24 ; keep counting the shifts
	CPI R24, 11  ;  11 because we are creating the CRC code for the first 11 bits
	BREQ exit11    ;  if shifted 11 times, message is over; exit
	SBRS R21, 7  ;  check MSB again after shift and if not set, keep shifting
	BRNE shift11
	
	BRNE div11    ;Loop back to div 

	JMP exit11

exit11:
	ROR R21  ;
	ROR R21  ;  Rotating three times to get the CRC to be last 5 bits (back to little endian)
	ROR R21  ;  Note that cannot use swap here 00111(CRC)000 swapped would be 10000011->incorrect			
				
	RET



CRC11_CHECK:
	LDI FAIL_PASS, 0x00 ; set falg as failed
	MOV R17, TOS   ; the data input (highbyte)
	MOV R18, CAPTURED		; the data input (lowbyte)

	CALL CRC11

	MOV R20, CAPTURED
	ANDI R20, 0b00011111	; mask the lower byte to extract the last 5 bits only

	CP R20, R21				; check CRC11 for both these registers

	BREQ not_corrupted       
	RET 
not_corrupted11: 
	LDI FAIL_PASS, 0x01 ; set flag as pased

	RET
