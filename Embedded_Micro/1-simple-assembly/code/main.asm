RJMP start
.INCLUDE "M128DEF.INC"

.EQU ZEROS = 0X00
.EQU ONES = 0XFF

; limits
; note for upper limits here they are greater than the pdf values by 1
; because we are useing BRLO in the code below 
; less than x+1 is the same as less than or equal x
.EQU T_LO_LMT = 0x0D
.EQU T_HT_LMT = 0xE5
.EQU M_LO_LMT = 0x0A
.EQU M_HT_LMT = 0xB5
.EQU W_LO_LMT = 0x07
.EQU W_HT_LMT = 0xF5

.EQU MEM_START = 0x100
.EQU MEM_END = 0x105

;MY CODE
.CSEG
.ORG 0X0050
start:
	
	; Initialize flags
	; G[0] is input for request flag
	; G[1] is output for Acknowledged flag
	LDI R16, 0x02
	STS DDRG, R16 
	
	; Initialize inputs
	LDI R16, ZEROS
	OUT DDRA, R16 ; Port A is input for Temperature
	OUT DDRB, R16 ; Port B is input for Moisture
	OUT DDRC, R16 ; Port C is input for Water level

	; Initialize outputs
	LDI R16, ONES 
	OUT DDRD, R16 ; Port D is output for Temperature
	OUT DDRE, R16 ; Port E is output for Moisture
	STS DDRF, R16 ; Port F is output for Water level


	; Initialize Stack pointer so we can use subroutines with no problem
	; Note RAMEND is defined in M128DEF.INC
	LDI R16, LOW(RAMEND)
	OUT SPL, R16
	LDI R16, HIGH(RAMEND)
	OUT SPH, R16

	; Initialize X as memory pointer to $100 (0x01 00)
	LDI XL, LOW(MEM_START)
	LDI XH, HIGH(MEM_START)

	LDI R16, 0x02 ; to set Acknowledged to 1
	LDI R20, ZEROS ; will be used for NULL character or to change Acknowledged to 0


main:
	; RECEIVING
	; set G[1] (Acknowledged) to 0 
	; if G[0] (request) is 0 go loop back to main
	; else continue with the code
	STS PORTG, R20 ; PORTG = 0
	LDS R21, PING ; R21 = PING
	SBRS R21,0 ; skip jumping back to main if R21 == 1
	RJMP main

	; CAPTURING
	; Reading inputs if request is 1
	IN R17, PINA
	IN R18, PINB
	IN R19, PINC
	
	; ACKNOWLEDGING
	STS PORTG, R16 ; PORTG = 1


	; SANITIZING
	; check Temperature 13 <= T <= 228
	CPI R17, T_LO_LMT
	BRSH check_higher_temp
	LDI R17, ONES
	RJMP check_moisture

check_higher_temp:
	CPI R17, T_HT_LMT
	BRLO check_moisture
	LDI R17, ONES

	; check Moisture 10 <= M <= 180
check_moisture:
	CPI R18, M_LO_LMT
	BRSH check_higher_moisture
	LDI R18, ONES
	RJMP check_water

check_higher_moisture:
	CPI R18, M_HT_LMT
	BRLO check_water
	LDI R18, ONES

	; check Water level 7 <= W <= 245
check_water:
	CPI R19, W_LO_LMT
	BRSH check_higer_water
	LDI R19, ONES
	RJMP display

check_higer_water:
	CPI R19, W_HT_LMT
	BRLO display
	LDI R19, ONES


display:
	; DISPLAY
	OUT PORTD, R17
	OUT PORTE, R18
	STS PORTF, R19

	; LOGGING

	ST X+, R17
	RCALL check_memory
	ST X+, R18
	RCALL check_memory
	ST X+, R19
	RCALL check_memory
	ST X+, R20
	RCALL check_memory


	JMP main


check_memory:
	; if they are equal check the lower byte else return where you left off
	LDI R21, HIGH(MEM_END)
	CP XH, R21
	BREQ maybe_full
	RET
maybe_full:
	; if XL > MEM_END Lower byte reset it else return
	LDI R21, LOW(MEM_END)
	CP R21, XL
	BRMI reset
	RET
reset:
	; reset to the beginning again in round-robin fashion
	LDI XL, LOW(MEM_START)
	LDI XH, HIGH(MEM_START)
	RET