#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h> // for sprintf
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>



#define F_CPU 4000000UL
#define BR_Calc 0x19

#define gen 0xD4

#define IMEM_START 0x0800
#define IMEM_END (RAMEND- 0x100) // to reserve 128bytes for stack: 0x10FF - 0x800 = 0x08FF //4351 - 2048 = 2303 bytes from internal SRAM for the logger
#define XMEM_START (RAMEND+1) // 0x1100
#define XMEM_END 0X18FF // because we are using 2KB external memory

// commands
#define Reset_command 0x00
#define Acknowledge 0x40
#define Log_command 0x20
#define Error_repeat 0x60

// Maximum buffer sizes for transmission:
#define USER_TR_BUFFER_SIZE 100

// messages indexes
#define USER_MENU 0
#define SENSOR_RST_MESSAGE 1
#define INVALID_OPTION 2
#define NOTHING_TO_SHOW 3
#define MEM_DUMP_START 4
#define MEM_DUMP_END 5
#define LAST_ENTRY_START 6
#define LAST_ENTRY_END 7
#define INIT_DONE 8
#define MASTER_WD_MENU 9
#define SLAVE_WD_MENU 10
#define SLAVE_TIMER_EXPIRED 11

// global variables

const unsigned char messages [][USER_TR_BUFFER_SIZE] = {
	"1-Memory Dump\n2-Last Entry\n3-Restart\0",
	"Sensor reset!\n\0",
	"Invalid option!\nChoose again\0",
	"Nothing to show\nlog file is empty\0",
	"Memory dump:\n",
	"Dump finished\0",
	"Last Entry: \n",
	"Finished\0",
	"Init Done\n",
	"Enter your option for Master Watchdog (& period):\rA-30ms\rB-250ms\rC-500ms \0",
	"Enter your option for Slave Watchdog (& period):\rA-11s\rB-13s\rC-15s \0",
	"Timer Expired!\0"
};

uint8_t TOS;
uint8_t is_tos_empty = 1; // 1 empty, 0 full
uint8_t* logFile = IMEM_START;
// to ensure not writing while reading and vice versa
uint8_t bluetooth_mutex = 0x01; // (0: mutex locked, 1: mutex unlocked)

// USART0 communication buffers and indexes
unsigned char user_tr_buffer[USER_TR_BUFFER_SIZE] = "";
uint8_t user_tr_ind = 0;
unsigned char user_rc_buffer;// because will only hold one character option

// USART1 communication buffers
uint8_t sensor_tr_buffer;



// functions prototypes
void initialize();
void usart_init();
void init_sensor();
void memory_dump();
void last_entry();
void user_transmit(unsigned char);
void sensor_transmit(uint8_t);
void repeat_request();
void log_request();

void message_user(uint8_t);

// WD functions
void ConfigMasterWD();
void ConfigSlaveWD();
void slave_wdt_reset();

// CRC functions
uint8_t CRC3(uint8_t);
uint8_t CRC11(uint8_t);
uint8_t CRC3_CHECK(uint8_t);
uint8_t CRC11_CHECK(uint8_t);

// helper functions
void pushTOS(uint8_t);
void popTOS();
void log_data(uint8_t);
uint8_t is_data_packet(uint8_t);
uint8_t log_file_empty();
void lock_bluetooth();
void unlock_bluetooth();

void display_data(unsigned char*);


uint8_t str_to_hex(unsigned char*); // for accepting inputs from sensor
void hex_to_str(uint8_t, unsigned char *); //  for user transmitting

int main(){
	
	initialize();

	while(1){
		//wdt_reset();
		sleep_enable();
		sleep_cpu();
	}
	
	return 0;
}

// check for user buffer when transmitting is completed
ISR(USART0_UDRE_vect){
	sleep_disable();
	// bluetooth mutex is locked and user buffer didn't finish yet
	if(!bluetooth_mutex){
		user_transmit(user_tr_buffer[user_tr_ind]);
		
		if( user_tr_buffer[user_tr_ind] == '\0'|| user_tr_ind == USER_TR_BUFFER_SIZE){
			unlock_bluetooth();
			return;
		}
		user_tr_ind++;
		_delay_ms(50);
	}

}

// receiving input from user
ISR(USART0_RX_vect){
	sleep_disable();
	
	// check for period to know that the previous input was the option
	while (!(UCSR0A & (1 << RXC0)));
	unsigned char input = UDR0;
	
	// check for period to know that the previous input was the option
	if(input != '.'){
		user_rc_buffer = input;
		return;
	}
	
	
	sei();
	// here the input was dot and we want to check the user input
	switch(user_rc_buffer){
		// memory dump
		case '1':
		memory_dump();
		break;
		
		case '2':
		last_entry();
		break;
		
		case '3':
		// reset sensor
		init_sensor();
		break;
		
		default:
		// invalid option
		message_user(INVALID_OPTION);
	}

}

ISR(USART1_UDRE_vect){
	sleep_disable();
	
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = sensor_tr_buffer;
	_delay_ms(10);

	
	UCSR1B &= ~((1 << TXEN1) | (1 << UDRIE1));
	UCSR1B |= (1 << RXEN1) | (1 << RXCIE1);
	
}

// receiving input from sensor
ISR(USART1_RX_vect){
	sleep_disable();
	
	uint8_t packet_in = UDR1;
	
	
	// if packet in is data packet
	if(is_data_packet(packet_in)){
		pushTOS(packet_in);
		return;
	}
	// packet_in is command packet
	if(is_data_packet(TOS)){
		// if check fails empty the TOS, send repeat request and return
		if(!CRC11_CHECK(packet_in)){
			popTOS();
			repeat_request();
			return;
		}
		// crc11 passed
		log_request();
		return;
	}
	
	// no data in TOS do crc3 check
	// check if it fails crc3
	if(!CRC3_CHECK(packet_in)){
		popTOS();
		repeat_request();
		return;
	}
	
	// mask the 7th and the 6th bit
	if((packet_in & 0x60) == Acknowledge){
		popTOS();
		return;
	}
	else if((packet_in & 0x60) == Error_repeat){
		if(is_tos_empty)return;
		
		// if TOS is not empty
		sensor_transmit(TOS);
	}
	
	
}

// slave watchdog will use timer1
ISR(TIMER1_COMPA_vect){
	sleep_disable();
	sei();
	TIMSK &= ~(1<<OCIE1A);

	message_user(SLAVE_TIMER_EXPIRED);
	init_sensor();
	ConfigSlaveWD();
}


uint8_t is_data_packet(uint8_t packet){
	return packet & 0x80;
}

void pushTOS(uint8_t value){
	TOS = value;
	is_tos_empty = 0;
}

void popTOS(){
	is_tos_empty=1;
}

void usart_init(){
	
	// User USART0
	// setting the Baud rate
	UBRR0H = (uint8_t)(BR_Calc >> 8); // loading the most significant byte
	UBRR0L = (uint8_t)BR_Calc;
	//setting the width to 8 bits
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	
	// Sensor USART1
	UBRR1H = (uint8_t)(BR_Calc >> 8);
	UBRR1L = (uint8_t)BR_Calc;
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	
	
}

void init_sensor(){
	pushTOS(CRC3(Reset_command));
	sensor_transmit(TOS);
	message_user(SENSOR_RST_MESSAGE);
}

void initialize(){
	
	MCUCR = (1 << SRE); // External memory enable
	XMCRB = (1<<XMM2)|(1<<XMM0); // release C pins that are not needed
	
	// enable USART communications
	usart_init();
	
	// set global interrupt enable
	sei();

	// enable watchdog timers
	//ConfigMasterWD(); // master will wdt_enable at the end is commented
	//ConfigSlaveWD(); // for sensor: diabled for testing
	
	
	UCSR1B &= ~((1 << TXEN1) | (1 << UDRIE1));
	UCSR1B |= (1 << RXEN1) | (1 << RXCIE1);
	// initialize sensor
	init_sensor();
	
	// show menu to the user
	message_user(USER_MENU);
}

void log_request(){
	*(logFile) = TOS;
	logFile++;
	if(logFile > XMEM_END) logFile = IMEM_START; // reset to the beginning
	else if(logFile> IMEM_END) logFile = XMEM_START; // skip stack
	
	pushTOS(CRC3(Acknowledge));
	
	sensor_transmit(TOS);
}

uint8_t log_file_empty(){
	// nothing to show
	if(logFile == IMEM_START){
		message_user(NOTHING_TO_SHOW);
		return 1;
	}
	
	return 0;
}

void lock_bluetooth(){
	// mutex is locked
	// means last transmition didn't finish yet
	while(!bluetooth_mutex){
		sleep_enable();
		sleep_cpu();
	}
	
	// lock mutex to send data
	bluetooth_mutex = 0; // locked and we can send the entire buffer
	// disable receiving interrupt from the user and enabling only transmitting
	UCSR0B &= ~((1 << RXEN0) | (1 << RXCIE0));
	UCSR0B |= (1 << TXEN0) | (1 << UDRIE0);
}

void unlock_bluetooth(){
	bluetooth_mutex = 1;// unlock the mutex
	user_tr_ind = 0;
	
	UCSR0B &= ~((1 << TXEN0) | (1 << UDRIE0)); // disabling
	UCSR0B |= (1 << RXEN0) | (1 << RXCIE0);
}

void message_user(uint8_t message_index){
	_delay_ms(300);
	strcpy(user_tr_buffer, messages[message_index]);
	user_tr_ind = 0;
	lock_bluetooth();

}

void display_data(unsigned char* data){
	_delay_ms(300);
	strcpy(user_tr_buffer, data);
	user_tr_buffer[2] = ' ';
	user_tr_buffer[3] = '\0';
	user_tr_ind = 0;
	
	
	lock_bluetooth();
}

void memory_dump(){
	
	if(log_file_empty()) return;
	
	//message_user(MEM_DUMP_START);
	uint8_t* ptr = IMEM_START;
	
	unsigned char temp[4] = "";
	
	while(ptr< logFile){
		hex_to_str(*ptr, temp);

		display_data(temp);
		ptr++;
		if(ptr > IMEM_END) ptr = XMEM_START; // skip the stack
	}
	
	//message_user(MEM_DUMP_END);
	
	
	
	
}

void last_entry(){
	if(log_file_empty()) return;
	
	//message_user(LAST_ENTRY_START);
	unsigned char temp[4] = "";
	hex_to_str(*(logFile-1), temp);
	
	display_data(temp);
	
	//message_user(LAST_ENTRY_END);
	
}

void repeat_request(){
	uint8_t out = CRC3(Error_repeat);
	sensor_transmit(out);
}

void user_transmit(unsigned char ch){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = ch;
}

void sensor_transmit(uint8_t packet_out){
	
	sensor_tr_buffer = packet_out;

	UCSR1B &= ~((1 << RXEN1) | (1 << RXCIE1));
	UCSR1B |= (1 << TXEN1) | (1 << UDRIE1);
	
}

uint8_t str_to_hex(unsigned char* buffer){
	return strtol(buffer,NULL, 16);
}

void hex_to_str(uint8_t hex, unsigned char * buffer){
	sprintf(buffer,"%x",hex);
	buffer[2]=' ';
	buffer[3]='\0';
}

uint8_t CRC3(uint8_t c){
	uint8_t new_c = c & 0xE0; //extract the first three bits of the command packet
	uint8_t temp = new_c;
	int msb;
	int i;
	for(i=0; i<3;i++)
	{
		msb = 1 << 7;

		if (temp & msb)   // if msb is set we do the xor operation
		temp =  temp ^ gen;

		temp = temp << 1;  //shift left

	}

	temp = temp >> 3;   //to shift the CRC bits so that they are in the correct position.


	new_c |= temp;   //appending the crc bits.

	return new_c;

}

uint8_t CRC3_CHECK(uint8_t command_in){
	uint8_t trueCRC= CRC3(command_in);

	return trueCRC == command_in ? 1 : 0 ; // if the data is corrupted return 0 else 1

}

uint8_t CRC11(uint8_t command_in){
	//get upper byte from TOS
	uint8_t data = TOS;
	uint8_t temp = data;
	uint8_t c_new = command_in & 0xE0;  //extract the first three bits of the command packet

	uint8_t c_cpy =  c_new;

	int msb;
	int count=0;

	for(int i=0; i<11;i++){
		msb = 1 << 7;

		if (temp & msb)   // if msb is set we do the xor operation
		temp =  temp ^ gen;

		temp = temp << 1;  //shift left

		if(count!=3) {
			temp= temp + (   (c_new & msb)  >> 7);
			c_new = c_new << 1;
			count++;
		}

	}
	
	
	temp = temp >> 3;   //to shift the CRC bits so that they are in the correct position.



	c_cpy |= temp;   //appending the crc bits.

	return c_cpy;


}

uint8_t CRC11_CHECK(uint8_t command_in){
	uint8_t trueCRC = CRC11(command_in);

	return trueCRC == command_in ? 1 : 0  ;   // if the data is corrupted return 0 else 1

}

void ConfigMasterWD(){
	
	uint16_t temp = eeprom_read_word((const uint16_t*)0x50);

	uint16_t config = 'C';

	if(temp == 0xFFFF){
		message_user(MASTER_WD_MENU);
		_delay_ms(10);
		
		// take input from user using polling
		uint8_t input = 0;
		
		cli(); // disable interrupts to so we don't mix things with user menu
		UCSR0B |= (1 << RXEN0);
		while (1){
			while(input != '.'){
				config = input;
				while(!(UCSR0A & (1<<RXC0)));
				input = UDR0;
			}
			if(!(config >= 'A' && config <= 'C')){
				sei();
				message_user(INVALID_OPTION);
				cli();
				config='C';
			}
			else{
				break;
			}
		}
		
		eeprom_write_word((const uint16_t*)0x50, config);
		
		UCSR0B &= ~(1 << RXEN0);


		
	}
	
	else {
		config = temp;
	}
	
	uint8_t duration;
	switch(config){
		// configure master watch dog
		case '1':
		duration = WDTO_30MS;
		break;
		
		case '2':
		duration = WDTO_250MS;
		break;
		
		case '3':
		duration = WDTO_500MS;
		default:
		duration = WDTO_500MS;
	}
	
	wdt_reset();
	//wdt_enable(duration);
	
	sei(); // enable interrupts again

}

void ConfigSlaveWD(){
	
	uint16_t temp = eeprom_read_word((const uint16_t*)0x0006);

	uint16_t config = 'C';

	if(temp == 0xFFFF){
		message_user(SLAVE_WD_MENU);
		_delay_ms(10);
		
		// take input from user using polling
		uint8_t input = 0;
		
		cli(); // disable interrupts to so we don't mix things with user menu
		UCSR0B |= (1 << RXEN0);
		while (1){
			while(input != '.'){
				config = input;
				while(!(UCSR0A & (1<<RXC0)));
				input = UDR0;
			}
			if(!(config >= 'A' && config <= 'C')){
				sei();
				message_user(INVALID_OPTION);
				cli();
				config='C';
			}
			else{
				break;
			}
		}
		
		eeprom_write_word((const uint16_t*)0x0006, config);
		
		UCSR0B &= ~(1 << RXEN0);
		sei(); // enable interrupts again


		
	}
	
	else {
		config = temp;
	}
	// using pre-scaler 256 so we dont have fractions
	uint16_t duration;
	switch(config){
		// configure slave watch dog
		case '1':
		duration = 44000; // 11s
		break;
		
		case '2':
		duration = 52000; // 13s
		break;
		
		case '3':
		duration = 60000; // 15s
		break;
		default:
		duration = 60000;
	}
	
	
	// reset timer
	TCNT1 = 0;
	// set output compare
	OCR1A = duration;
	// set 256 pre-scaling and activate CTC mode
	TCCR1B = (1<<CS12) | (1<<WGM12);
	
	// Enable interrupt on A match
	TIMSK = (1<<OCIE1A);
	

}

void slave_wdt_reset(){
	TCNT1 = 0;
}
