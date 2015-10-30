#include "pinmap.h"

// How many milliseconds to keep each digit on
#define DIGIT_TIME 1

uint16_t digit_time=DIGIT_TIME;

Timer timerCheckTime(100, q_check_time); // Do this 10 times per second

double freemem;

int mode=0; // 0=clock   1=minutes/seconds   2=A0   3=share price   4=clock & share price
uint8_t tenths=0;
uint16_t price=9999;

uint8_t digits[]={ DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_LIGHT };

// Map of segments to turn on for the numbers 0 - 9
uint8_t numbers[]={ 1+2+4+8+16+32, 6, 3+8+16+64, 1+2+4+8+64 , 2+4+32+64,1+4+8+32+64,1+4+8+16+32+64,7,1+2+4+8+16+32+64,1+2+4+8+32+64 };

uint8_t display[]={ 0, 0, 0, 0, 0, 0, 0, 0, 3,0 };

uint8_t i=0; // Pointer into display[] for which digit we're showing
uint8_t offset=0; // Offset to GMT

uint8_t lastSecond=255;

// Incremement the digit pointer, show that digit, then queue the next increment
void next_digit() {
    uint8_t ni=(i+2) % 10;
    
    // Precalc as much as possible before fiddling with the display for fastest update possible
    uint8_t n= (ni==8) ? display[ni]: numbers[ display[ni] ] ;
    uint8_t a=(n & 1)? ON: OFF;
    uint8_t b=(n & 2)? ON: OFF;
    uint8_t c=(n & 4)? ON: OFF;
    uint8_t d=(n & 8)? ON: OFF;
    uint8_t e=(n & 16)? ON: OFF;
    uint8_t f=(n & 32)? ON: OFF;
    uint8_t g=(n & 64)? ON: OFF;
    uint8_t h=display[ni+1]? ON : OFF ;
    
    digitalWriteFast( digits[i/2], !COMMON ); // Hide the last digit

    digitalWriteFast( digits[ni/2], COMMON ); 
    digitalWriteFast(SEG_A, a );
    digitalWriteFast(SEG_B, b );
    digitalWriteFast(SEG_C, c );
    digitalWriteFast(SEG_D, d );
    digitalWriteFast(SEG_E, e );
    digitalWriteFast(SEG_F, f );
    digitalWriteFast(SEG_G, g );
    digitalWriteFast(SEG_DP, h ); 
    delayMicroseconds(digit_time);
    i=ni;
}

void free_memory() {
    freemem = System.freeMemory();
}

// If the second has changed, display something depending on the mode we're in or advance the mode
void q_check_time() {
    uint8_t hour, min, sec;
    uint16_t num;
    sec=Time.second();
    if ((sec%10)==0) free_memory();
    tenths++;
    display[8]=0;
    if (tenths<8) {
        if (mode==0 || mode==1) display[8]=3;
        if (mode==4 && ((sec%10)/5)==0 ) display[8]=3; 
    }
    
    if (sec!=lastSecond) {
        tenths=0;
        num=0;
        uint16_t t=analogRead(A0)/256;
        min=Time.minute();
        hour=(Time.hour()+offset)%24;
        if (t>8) {
            mode+=1;
            if (mode==5) mode=0;
        }
        switch (mode) {
            case 0:
                num=hour*100+min;
                break;
            case 1:
                num=min*100+sec;
                break;
            case 2:
                num=t;
                break;
            case 3:
                num=price;
                break;
            case 4:
                switch ((sec%10)/5) {
                    case 0:
                        num=hour*100+min;
                        break;
                    case 1:
                        num=price;
                        break;
                }
                break;
        }
        display[0]=num/1000;
        display[2]=(num/100)%10;
        display[4]=(num/10)%10;
        display[6]=num%10;
        lastSecond=sec;
    }

}


void setup() {
    pinMode( DIGIT_1, OUTPUT);
    pinMode( DIGIT_2, OUTPUT);
    pinMode( DIGIT_3, OUTPUT);
    pinMode( DIGIT_4, OUTPUT);
    pinMode( DIGIT_LIGHT, OUTPUT);
    pinMode( SEG_A, OUTPUT);
    pinMode( SEG_B, OUTPUT);
    pinMode( SEG_C, OUTPUT);
    pinMode( SEG_D, OUTPUT);
    pinMode( SEG_E, OUTPUT);
    pinMode( SEG_F, OUTPUT);
    pinMode( SEG_G, OUTPUT);
    pinMode( SEG_DP, OUTPUT);
    pinMode( D0, INPUT);
    digitalWrite( D0, LOW);

    timerCheckTime.start(); // Start checking for time updates

    bool success = Particle.function("mode", set_mode);
    success = Particle.function("price", set_price);
    success = Particle.function("refresh", set_refresh);
    Particle.variable("memory", freemem);
    Particle.variable("mode", mode);

}

void loop() {
    next_digit();
}

int set_mode(String command) {
    if (command.length()>0) mode=command.toInt();
    return mode;
}

int set_price(String command) {
    if (command.length()>0) price=command.toInt();
    return price;
}

int set_refresh(String command) {
    if (command.length()>0) digit_time=command.toInt();
    return digit_time;
}
