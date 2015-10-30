#define CATHODE 0
#define ANODE 1

// Change as to ANODE or CATHODE for you LED display
#define COMMON CATHODE

#define ON !COMMON
#define OFF !ON

#define SEG_A D4
#define SEG_B D2
#define SEG_C D5
#define SEG_D A4
#define SEG_E DAC
#define SEG_F D7
#define SEG_G D3
#define SEG_DP RX

#define DIGIT_1 A2
#define DIGIT_2 A3
#define DIGIT_3 WKP
#define DIGIT_4 TX
#define DIGIT_LIGHT A5