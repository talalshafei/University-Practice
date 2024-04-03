/* Include statements*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h> // for sprintf
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>


/* Define statements */
#define F_CPU  4000000UL
#define BR_Calc  0x19



// Variables and ports definitions
#define LCD_PORT PORTB   //writes to LCD
#define LCD_DDR  DDRB
#define LCD_CTRL PORTA   // for RS, RW and E
#define LCD_CTRL_DIR DDRA
#define KEY_PORT PORTC
#define KEY_DIR	 DDRC
#define KEY_PIN	 PINC   //for user input to KEYPAD


#define ROW1	   0xFE
#define ROW2	   0xFD
#define ROW3	   0xFB
#define ROW4       0xF7
#define LINEONE    0x80
#define LINETWO    0xC0
#define LINETHREE  0x94
#define LINEFOUR   0xD4

#define LCD_INIT_8BIT 0x38 // 2 Lines and 5x7 matrix (D0-D7, 8-bit)
#define LCD_DISPLAY_ON_CURSOR_OFF 0x0C //Display on, cursor off
#define LCD_CLEAR_DISPLAY 0x01
#define LCD_LEFT_TO_RIGHT 0x06



#define RS 7   
#define RW 6
#define E 5

#define central_rc_buffer_SIZE 100



/*Global variables*/

// to ensure not writing while reading and vice versa
uint8_t bluetooth_mutex = 0x01; // (0: mutex locked, 1: mutex unlocked)
unsigned char central_rc_buffer[central_rc_buffer_SIZE] = "";
uint8_t central_rc_ind = 0;
uint8_t line = 1;

// debugger
char* debugger = 0x500; 

/* functions */
void lock_bluetooth();
void unlock_bluetooth();
void central_transmit(unsigned char);
void lcd_print(unsigned char);
void sysINIT();
void lcd_command(uint8_t);
unsigned char getKeyPressed();
void lcd_choose_line();
void display_message();


int main(){
	sysINIT();
	
	while(1){
		sleep_enable();
		sleep_cpu();
	}
	
	return 0;

}


//keypad interrupt
ISR(INT0_vect){
	sleep_disable(); // disable sleep once an interrupt wakes CPU up
	// get row and column from keypad

	

	unsigned char userKey = getKeyPressed();
	KEY_PORT = 0x0F;
	
	// Send character to main logger
	
	central_transmit(userKey);
	central_transmit('.');
	
}

// the user interface module is getting data from the data logger module
ISR(USART0_RX_vect){
	sleep_disable(); // disable sleep once an interrupt wakes CPU up
	
	// bluetooth mutex is locked and user buffer didn't finish yet
	if(!bluetooth_mutex){
		central_rc_buffer[central_rc_ind] = UDR0;
		*(debugger++) = central_rc_buffer[central_rc_ind];
		
		if( central_rc_buffer[central_rc_ind] == '\0'|| central_rc_ind == central_rc_buffer_SIZE){
				unlock_bluetooth();
				return;
		}
		central_rc_ind++;
	}
	
}


void lcd_print(unsigned char ch){
	LCD_CTRL |= (1<<RS); //select data register
	LCD_CTRL &= ~(1<<RW); // set it for writing
	
	LCD_PORT = ch;
	
	LCD_CTRL |= (1<<E);
	_delay_ms(1);
	LCD_CTRL &= ~(1<<E);
	_delay_ms(15);
}

void sysINIT(){
	
	
	// initialize LCD
	LCD_CTRL_DIR = 0xE0;
	LCD_DDR = 0xFF; // D[7:0] of LCD
	_delay_ms(20); // documentation says +15ms
	lcd_command(LCD_INIT_8BIT); // make it 8 bit mode and 2 lines
	lcd_command(LCD_DISPLAY_ON_CURSOR_OFF); // display on cursor off (or cursor on with 0x0E)
	lcd_command(LCD_LEFT_TO_RIGHT);
	lcd_command(LCD_CLEAR_DISPLAY);
	lcd_choose_line(line);
	

	UBRR0H = (BR_Calc >> 8);
	UBRR0L = BR_Calc;
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	lock_bluetooth();
	
	//these depend on the connections
	//to enable input from keypad
	KEY_DIR = 0x0F;
	KEY_PORT = 0x0F;
	
	// keypad interrupt
	EIMSK = 0x01;
	EICRA |= (1<<1);

	sei();
}

void lock_bluetooth(){
	// mutex is locked
	// means last transmition didn't finish yet
	
	// lock mutex to send data
	bluetooth_mutex = 0; // locked and we can receive the entire buffer
	UCSR0B &= ~((1 << TXEN0) | (1 << UDRIE0)); // disabling
	UCSR0B |= (1 << RXEN0) | (1 << RXCIE0);
}

void unlock_bluetooth(){
	bluetooth_mutex = 1;// unlock the mutex
	central_rc_ind = 0;
	// disable receiving interrupt from the user and enabling only transmitting
	UCSR0B &= ~((1 << RXEN0) | (1 << RXCIE0));
	UCSR0B |= (1 << TXEN0) | (1 << UDRIE0);
	
	display_message();
}

void central_transmit(unsigned char data_packet){
	UCSR0B &= ~((1 << RXEN0) | (1 << RXCIE0));
	UCSR0B |= (1 << TXEN0) | (1 << UDRIE0);
	while(!(UCSR0A	& (1<<UDRE0)));
	UDR0= data_packet;
	
}

void lcd_command(uint8_t command){
	LCD_CTRL &= ~((1<<RS) | (1<< RW)); // select command register and set it as writing
	LCD_PORT = command;
	
	// now we send H-To-L pulse on E to apply changes
	// delays might be excessive here but they are safe as shown in the lecture slides
	LCD_CTRL |= (1<<E);
	_delay_ms(1);
	LCD_CTRL &= ~(1<<E);
	_delay_ms(15);
	
}

unsigned char getKeyPressed(){
	
	//toggle the rows one by one and check for which column is activated
	
	KEY_PORT = ROW1;  //make ROW1 = 0  ->grounding
	
	if(KEY_PIN == 0xEE ) return '1';
	else if(KEY_PIN == 0xDE) return '2';
	else if(KEY_PIN == 0xBE) return '3';
	
	KEY_PORT = ROW2;  //make ROW2 = 0  ->grounding
	if(KEY_PIN == 0xED ) return '4';
	
	else if(KEY_PIN == 0xDD) return '5';
	else if(KEY_PIN == 0xBD) return '6';
	
	KEY_PORT =  ROW3;   // make ROW3= 0  -> grounding
	
	if(KEY_PIN == 0xEB ) return '7';
	else if(KEY_PIN == 0xDB) return '8';
	else if(KEY_PIN == 0xBB) return '9';
	
	KEY_PORT = ROW4;  //make ROW4=0  -> grounding
	
	if(KEY_PIN == 0xE7) return '*';
	else if(KEY_PIN == 0xD7) return '0';
	else if(KEY_PIN == 0xB7) return '#';
	
	return 0;
}

void lcd_choose_line(){
	if (line == 1)lcd_command(LINEONE);
	else if (line == 2)lcd_command(LINETWO);
	else if (line == 3)lcd_command(LINETHREE);
	else if (line == 4) lcd_command(LINEFOUR);
	else{
		lcd_command(LCD_CLEAR_DISPLAY);
		line =0;
	} // display is full so clear
	line++;
}

void display_message(){
	
	cli();
	//lcd_command(LCD_CLEAR_DISPLAY);
	//lcd_choose_line(line);
	
	unsigned char ch;
	
	for(uint8_t i = 0; central_rc_buffer[i]; i++){
		ch = central_rc_buffer[i];
		if(ch == '\n'){
		 lcd_choose_line();
		 continue;
		}
		
		
		lcd_print(ch);
	}
	
	lock_bluetooth();
	sei();
	
}