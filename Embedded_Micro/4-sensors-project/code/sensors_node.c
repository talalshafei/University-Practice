
/* Includes */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h> // for sprintf
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/wdt.h>

/* Define statements*/


#define F_CPU 4000000UL
// ((F_CPU/16/BaudRate)-1) = 0x19
#define BR_Calc 0x19

#define gen 0xD4 // CRC polynomial

// easiest way to reset the memory and all registers is to leave the Watchdog timer to expire
#define COMMAND_TYPE(x)	(x&0x60)
	
// LCD
//Command_to_LCD(0x10);	//shift cursor to right
#define LCD_START_LINE_1 0x80
#define LCD_START_LINE_2 0xC0
#define LCD_INIT_8BIT 0x38 // 2 Lines and 5x7 matrix (D0-D7, 8-bit)
#define LCD_DISPLAY_ON_CURSOR_OFF 0x0C //Display on, cursor off
#define LCD_CLEAR_DISPLAY 0x01 
#define LCD_LEFT_TO_RIGHT 0x06

#define LCD_DDR DDRE
#define LCD_PORT PORTE // for sending data to LCD
#define LCD_CTRL_DIR DDRD 
#define LCD_CTRL PORTD // for controlling the LCD flags
// PORTD bits
#define RS 5
#define RW 6
#define E 7

// commands
#define Reset_command 0x00
#define Acknowledge 0x40
#define Log_command 0x20
#define Error_repeat 0x60

// ADC
#define ADC_DDR DDRF


// sensor indexes
#define TEMPERATURE 0
#define MOISTURE 1
#define WATER 2
#define BATTERY 3 

// MOTOR 
// since our PWM wave will be on OC1B which is PB6 and we can also use the other pins for the L293D motor driver
#define MOTOR_DDR DDRB
#define MOTOR_PORT PORTB

/* Global variables*/
uint8_t TOS; // in case we received a repeat request
uint8_t is_tos_empty = 1; // 1 empty, 0 full

uint8_t central_tr_buffer; // for transmitting byte to the central MCU
uint8_t central_rc_buffer; // to receive byte from the central MCU

uint8_t sensor_input_buffer[4] = {0, 0, 0, 0}; // because we have 4 sensors

/* Prototypes*/
void initialize();
void message_central(uint8_t);
void init_timer3_10s();

// helper
void pushTOS(uint8_t);
void popTOS();

// LCD functions
void lcd_command(uint8_t);
void lcd_print(unsigned char*);
void lcd_display_sensor_screen();
void lcd_display_battery_low();

// sensor functions
void read_sensors();
uint16_t get_sensor(uint8_t);
void receive();
void reset_sensor();


// CRC functions
uint8_t CRC3(uint8_t);
uint8_t CRC11(uint8_t);
uint8_t CRC3_CHECK(uint8_t);


int main(void){
	initialize();
    while (1){	
		sleep_enable();
		sleep_cpu();
    }
}

// to send data for the central MCU
ISR(USART1_UDRE_vect){
	sleep_disable();
	
	while(!(UCSR1A & (1<<UDRE1))); // double check
	UDR1 = central_tr_buffer;
	_delay_ms(100);

	// disabling transmitting and enabling receiving again
	UCSR1B &= ~((1 << TXEN1) | (1 << UDRIE1));
	UCSR1B |= (1 << RXEN1) | (1 << RXCIE1);
}
// to receive data from the central MCU
ISR(USART1_RX_vect){
	sleep_disable();
	
	while (!(UCSR1A & (1 << RXC1))); //double check as in slides ;)
	uint8_t input = UDR1; // capture input
	
	// data logger will only send repeat requests, reset command, or acknowledge
	sei();
	if(!CRC3_CHECK(input)){
		// send error repeat request
		message_central(CRC3(Error_repeat));
	}
	
	else if(Reset_command == COMMAND_TYPE(input)){
		reset_sensor();
	}
	else if(Error_repeat == COMMAND_TYPE(input)){
		message_central(CRC3(TOS));
	}
	
	else if(Acknowledge == COMMAND_TYPE(input)){
		// since acknowledge we can discard TOS contents
		popTOS();
	}
	
	else {
		// send repeat error 
		message_central(CRC3(Error_repeat));		
	}
	
	
}

// monitor conversion
ISR(ADC_vect){
	// this will prompt us back to get sensor
	sleep_disable();
}

// work every 10s
ISR(TIMER3_COMPA_vect){
	sleep_disable();
	// read sensors
	read_sensors();
	// check battery
	// 3.2V -> 0x04 digital from the Battery equation
	if(sensor_input_buffer[BATTERY] < 0x04){
		lcd_display_battery_low();
	}
	else{
		// display values
		lcd_display_sensor_screen();
	}
	
	
	// send values
	sei();
	for(uint8_t i =0; i<0x04; i++){
		
		// set data flag, data id, 5 bit data parameter as explained in module 2
		uint8_t data = (0x80 | (i<<5)) | sensor_input_buffer[i];
		// get crc11 for the data with the log request
		uint8_t log_request = CRC11(data);
		
		// send data first
		message_central(data);
		pushTOS(data);
		// then send log request
		message_central(log_request);
		pushTOS(log_request);
		receive();
		
	}
	_delay_ms(100);
	
	init_timer3_10s();
	
}

// start motor after 10s
ISR(TIMER1_COMPB_vect){
	sleep_disable();
	// start the motor
	// duty cycle proportional to Moisture
	// so we can make it based on the limits given as (moisture-0x02)/0x1E which is max-min normalization
	// which is equal to (0xFF - OCRO)/0xFF
	// arrange it in way to avoid float numbers
	// finally we want to multiply the full duty cycle by 60% and shift it by 20% of Timer0 cycles
	// to have 20% to 80% duty cycle as explained in the manual
	// 20% is 0.2*255 = 51 cycles
	// 60% is  153/255 which is 0x99/0xFF
	
	// phase correct PWM, inverted, 1024 prescaler
	TCCR0= (1<<WGM00) | (1<<COM01) | (1<<COM00) | (1<<CS02) |(1<<CS01);//|(1<<CS00);	
	// proportional to Moisture
	OCR0 = ((0x1320- 0x99*sensor_input_buffer[MOISTURE])/0x1E) + 0x33;

}

// stop motor after the 5s
ISR(TIMER1_OVF_vect){
	sleep_disable();
	// stop the motor
	// note since OC0 connected to EN in L293D all we need is to stop the PWM
	TCCR0 = 0;
	
}

void pushTOS(uint8_t value){
	TOS = value;
	is_tos_empty = 0;
}

void popTOS(){
	is_tos_empty=1;
}

void reset_sensor(){
	// will force reset in 15ms
	wdt_enable(WDTO_15MS);
	while(1);
}

void init_timer3_10s(){
	// reset timer
	TCNT3 = 0;
	// set output compare
	OCR3A = 40000;
	// set 1024 prescaling and activate CTC mode
	TCCR3B = (1<<CS32) | (1<<CS30)| (1<<WGM32);
	// Enable interrupt on a compare match
	ETIMSK = (1<<OCIE3A);
}

void receive(){
	// disabling transmitting and enabling receiving again
	UCSR1B &= ~((1 << TXEN1) | (1 << UDRIE1));
	UCSR1B |= (1 << RXEN1) | (1 << RXCIE1);
	
}

void initialize(){
	// USART initialization
	UBRR1H = (uint8_t)(BR_Calc>>8);
	UBRR1L = (uint8_t)(BR_Calc);
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10); // setting USART to 8 bit width
	UCSR1B |= (1 << RXEN1) | (1 << RXCIE1); // enabling receive and receive interrupt

	// ADC initialization
	ADC_DDR = 0x00; // input chanels for ADC, but only the first 4 bits
	//ADMUX = (1<<REFS0); // choose AVCC as the voltage reference 
	// enable ADC, and prescaling by 64 to make the 4.096MHZ -> 64KHZ to be valid
	ADCSRA =  (1<<ADEN) | (1<< ADPS2) | (1<< ADPS1);
	
	// initialize LCD
	LCD_CTRL_DIR |= 0xE0;
	LCD_DDR = 0xFF; // D[7:0] of LCD
	_delay_ms(20); // documentation says +15ms
	lcd_command(LCD_INIT_8BIT); // make it 8 bit mode and 2 lines
	lcd_command(LCD_LEFT_TO_RIGHT);
	lcd_command(LCD_DISPLAY_ON_CURSOR_OFF); // display on cursor off (or cursor on with 0x0E)
	lcd_command(LCD_CLEAR_DISPLAY); // clear display screen
	lcd_display_sensor_screen();	
	
	// configure timer 3 to wait 10s
	init_timer3_10s();
	
	
	
	// configure timer 1 for Fast PWM mode 15
	// we will have 10s waiting and 5s serving the motor
	// make interrupt for the match that will occur after 10s to start timer0 PWM to serve the motor
	// thus duty cycle for timer1 is 33.33% 
	// since we are using mode 15 we can define our TOP to be 60000 cycle of Clk_mcu
	// 40000(10s) then sets it for 20000(5s) until counter reach the TOP and clear it
	// set output compare
	OCR1A = 60000; // TOP
	OCR1B = 40000; // 10s
	TCCR1A = (1<<WGM11) | (1<<WGM10); // WGM for setting to mode 15 also keeping COM zeros to disconnect OC
	TCCR1B = (1<<CS12) | (1<<CS10) | (1<<WGM13) | (1<<WGM12);	// set 1024 prescaling
	TIMSK = (1<<OCIE1B) | (1<<TOIE1);// Enable interrupt on compare match and on overflow
	MOTOR_DDR = 0xFF; // make PORTB an output
	MOTOR_PORT = 0x01; // sets the direction
	// no need to activate COMM since we are not concerned of the output of OC1
	// enable interrupts
	sei();
}

void message_central(uint8_t packet){
	central_tr_buffer = packet;
	
	// disabling the receive and the receive interrupt
	UCSR1B &= ~((1 << RXEN1) | (1 << RXCIE1));
	// enabling the transmitting and the USART buffer empty interrupt
	UCSR1B |= (1 << TXEN1) | (1 << UDRIE1);
	
	
}

uint16_t get_sensor(uint8_t index){
	// choose the input voltage to read from
	ADMUX = (1<<REFS0)|index;
	// begin A to D and enable interrupts
	//ADCSRA |= (1<<ADSC) | (1<<ADIE) ;
	
	//sleep_enable();
	//sleep_cpu();
	
	
	// stop ADC conversion
	//ADCSRA &= ~((1<<ADSC) | (1<<ADIE));
	
	// polling 
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF)) == 0);
	
	//uint16_t value, valueL;
	//valueL = ((uint16_t)ADCL);
	//value = ((uint16_t)ADCH) * 256;
	//
	//value = value + valueL;
	//return (value);
	return ADC;
}

void read_sensors(){
	
	// we are using  Vref 5 volts and 10 bits resolution
	// therefore step size = 5/1024 = 4.88 mV (5/1024)
	// and digital output will be Vin/step size 
	// after that we will scale each one based on its limits to become 5bits
	
	// calculations and equations are in the report
	
	uint32_t temp;
	
	// TEMPERATURE 
	// m = 15/256 
	// c = -21
	temp = get_sensor(TEMPERATURE);
	temp = (15*temp / 256) - 21;
	sensor_input_buffer[TEMPERATURE] = (uint8_t)temp;
	_delay_ms(1);
	
	// MOISTURE
	// m = 175/3072
	// c = -19
	temp = get_sensor(MOISTURE);
	//temp = 0.0568*temp - 19;
	temp = (175*temp / 3072) - 19 + 1;
	sensor_input_buffer[MOISTURE] = (uint8_t)temp;
	_delay_ms(1);
	
	// WATER
	// m = 275/2048
	// c = -51 
	temp = get_sensor(WATER);
	temp = (275*temp / 2048) - 51;
	sensor_input_buffer[WATER] = (uint8_t)temp;;
	_delay_ms(1);
	
	// BATTERY
	// m = 75/1024
	// c = -44
	temp = get_sensor(BATTERY);
	//temp = 0.074*temp - 44;
	temp = (75*temp / 1024) - 44 + 1;
	sensor_input_buffer[BATTERY] = (uint8_t)temp;;
	_delay_ms(1);
		
}

void lcd_command(uint8_t command){
	LCD_CTRL &= ~((1<<RS) | (1<< RW)); // select command register and set it as writing
	LCD_PORT = command;
	
	// now we send H-To-L pulse on E to apply changes
	// delays might be excessive here but they are safe as shown in the lecture slides
	LCD_CTRL |= (1<<E);
	_delay_ms(1);
	LCD_CTRL &= ~(1<<E);
	_delay_ms(20);
	
}

void lcd_print(unsigned char* str){
	for(uint8_t i = 0; str[i]; i++){
		LCD_CTRL |= (1<<RS); //select data register
		LCD_CTRL &= ~(1<<RW); // set it for writing
		LCD_PORT = str[i];
		LCD_CTRL |= (1<<E);
		_delay_ms(1);
		LCD_CTRL &= ~(1<<E);
		_delay_ms(20);
	}
	
}

void lcd_display_sensor_screen(){
	unsigned char str[17];
	
	//clear screen
	lcd_command(LCD_CLEAR_DISPLAY);
	
	// put the cursor at the beginning of the first line
	lcd_command(LCD_START_LINE_1);
	sprintf(str, " T = %2x  M = %2x ", sensor_input_buffer[TEMPERATURE], sensor_input_buffer[MOISTURE]);
	lcd_print(str);
	
	// put the cursor at the beginning of the second line
	lcd_command(LCD_START_LINE_2);
	sprintf(str," W = %2x  B = %2x ", sensor_input_buffer[WATER], sensor_input_buffer[BATTERY]);
	lcd_print(str);
}

void lcd_display_battery_low(){
	//clear screen
	lcd_command(LCD_CLEAR_DISPLAY);
	lcd_command(LCD_START_LINE_1);
	lcd_print(" Change Battery \0");
	lcd_command(LCD_START_LINE_2);
	lcd_print(" Immediately :( \0");
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

uint8_t CRC11(uint8_t data){
	uint8_t temp = data;
	uint8_t c_new = Log_command & 0xE0;  //extract the first three bits of the command packet

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