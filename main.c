/*
 * sleepDetector.c
 *
 * Created: 8/7/2022 9:36:56 AM
 * Author : CHAMITH 
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>

volatile unsigned long timer0_millis = 0;

int SensorStatePrevious = 0;

unsigned long minSensorDuration = 2000/2;
unsigned long minSensorDuration2 = 4000/2;
unsigned long SensorLongMillis;
bool SensorStateLongTime = false;

const int intervalSensor = 50;
unsigned long previousSensorMillis;

unsigned long SensorOutDuration;

//// GENERAL ////

unsigned long currentMillis;

void readSensorState(){
	if(currentMillis - previousSensorMillis > intervalSensor){
		int SensorState = PINB & (1<<PINB2);
		
		if (SensorState == 0 && SensorStatePrevious == 1 && !SensorStateLongTime) {
			SensorLongMillis = currentMillis;
			SensorStatePrevious = 0;
		}
		SensorOutDuration = currentMillis - SensorLongMillis;
		
		if (SensorState == 0 && !SensorStateLongTime && SensorOutDuration >= minSensorDuration){
			SensorStateLongTime = true;
			PORTC |= (1<<PORTC0);	//digitalWrite(Relay,HIGH);
		}
		
		if (SensorState == 0 && SensorStateLongTime && SensorOutDuration >= minSensorDuration2) {
			SensorStateLongTime = true;
			PORTB |= (1<<PORTB5);	//digitalWrite(buzzer,HIGH);
			_delay_ms(1000);
		}
		if (SensorState == 1 && SensorStatePrevious == 0) {
			SensorStatePrevious = 1;
			SensorStateLongTime = false;
			PORTC &= ~(1<<PORTC0);	//digitalWrite(Relay,LOW);
			PORTB &= ~(1<<PORTB5);	//digitalWrite(buzzer,LOW);
		}
		previousSensorMillis = currentMillis;
	}
	
}

unsigned long millis()
{
	unsigned long m;
	uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = timer0_millis;
	SREG = oldSREG;

	return m;
}

int main(void)
{
	
	DDRB |= (1<<DDB2);
	DDRC &= ~(1<<DDC0);
	DDRB &= ~(1<<DDB5);
	
	
    while (1)
    {
		currentMillis = millis();    // store the current time
		readSensorState();           // read the sensor state
    }
}



