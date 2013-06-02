/*
 * vUSB.c
 *
 * Created: 5/7/2012 12:15:14 PM
 *  Author: Parvez
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#define F_CPU 12000000L

#include "usbdrv.h"
#define USB_LED_OFF 0
#define USB_LED_ON  1
#define USB_DATA_OUT 2

#define USART_BAUDRATE 1000
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define ID 40

static uchar power[64] = "Power is OK\0";
static uchar shut[64] = "Power is gone\0";
static uchar user[4][64] = {"User wants shutdown\0"	,
							"User wants restart\0"	,
							"User wants log-off\0"	,
							"User wants hibernate\0"};
volatile uint_fast8_t count=0;

USB_PUBLIC uchar usbFunctionSetup(uchar data[8])
{
	usbRequest_t *rq = (void *)data;	// cast data to correct type
	switch(rq->bRequest)				// custom command is in the bRequest field
	{
		case USB_LED_ON:				// turn LED on
			PORTC |= (1<<3);
			return 0;
		case USB_LED_OFF:				// turn LED off
			PORTC &= ~(1<<3);
			return 0;
		case USB_DATA_OUT:				// send data to PC
			if(PORTC & 0x04)
			{
				uint_fast8_t index = PORTC & 0x03;
				PORTC &= ~(1<<2);
				usbMsgPtr = user[index];
				return sizeof(user[index]);
			}
			if(PINB & 0x02)
			{
				usbMsgPtr = power;
				return sizeof(power);
			}
			else
			{
				usbMsgPtr = shut;
				return sizeof(shut);
			}
	}
}

void USART_Init()
{
	//  Set baud rate
	UBRRH = BAUD_PRESCALE>>8;
	UBRRL = BAUD_PRESCALE;
	//  Enable receiver &  its interrupt
	UCSRB = (1<<RXEN)|(1<<RXCIE);
	//  Set frame format: 8data, 2stop bit
	UCSRC = (1<<URSEL)|(1<<UPM1)|(1<<UPM0)|(1<<USBS)|(1<<UCSZ1)|(1<<UCSZ0);
}

ISR(USART_RXC_vect)
{
	uint_fast8_t data= UDR;
	if(data>>2 == ID)	++count;
	else	count=0;
	if(count==3)
	{
		count=0;
		PORTC &= ~(0x03);
		PORTC |= (1<<2);
		PORTC |= data & 0x03;
	}
}

int main()
{
	DDRB = 0x00;
	DDRC = 0xFF;
	PORTC = 0x00;						//PC1 enables all the sensors
	
	wdt_enable(WDTO_1S);				// enable 1s watchdog timer
	usbInit();
	usbDeviceDisconnect();				// enforce re-enumeration
	
	for(uint_fast8_t i = 0; i<250; i++)				// wait 500 ms
	{
		wdt_reset();
		_delay_ms(2);
	}
	
	usbDeviceConnect();
	USART_Init();
	sei();								// Enable interrupts

	while(1)
	{
		wdt_reset();					// keep the watchdog happy
		usbPoll();
	}

	return 0;
}