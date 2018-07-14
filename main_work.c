

#include<avr/io.h>
#include<avr/wdt.h>
#include <avr/interrupt.h>

static int daydream_cafe[150] = {
0x00,0x00,0x00,0x00,0x00,
0xBD,0xB2,0x9E,0x8D,0x00,0x8D,0x00,0x8D,0x8D,0x9E,0x9E,0x9E,0xB2,0xB2,0xBD,0xB2,0xD4,
0xEE,0xEE,0x9E,0x00,0xEE,0x9E,0x9E,0x8D,0x9E,0xB2,0xBD,
0xB2,0x00,0xB2,0xB2,0xB2,0xBD,0xBD,0x00,0x9E,0x85,
0xD4,0xBD,0xB2,0xBD,0xEE,0xB2,0xBD,0xEE,0x8D,0x9E,0x9E,
0xBD,0xB2,0x9E,0x8D,0x00,0x8D,0x00,0x8D,0x8D,0x9E,0x9E,0x9E,0xB2,0xB2,0xBD,0xB2,0xD4,
0xEE,0xEE,0x9E,0x00,0xEE,0x9E,0x9E,0x8D,0x9E,0xB2,0xBD,
0xB2,0x00,0xB2,0xB2,0xB2,0xBD,0xBD,0x00,0x9E,0x85,
0xD4,0xBD,0xB2,0xBD,0xEE,0xB2,0xBD,0xEE,0x8D,0x9E,0x9E,0x9E,0x9E,0x9E,0x8D,0x9E,0x9E
};

static char sound_led[9] = {
0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00//ド,レ,ミ,ファ,ファ#,ソ,ラ,シ,0
};

char show_led[8] = {0,0,0,0,0,0,0,0xFF};

int sound_cnt = 0;
int cnt = 0;
int call_sound = 0;
int sound_speed = 13;
void update_led();
void set_led(char add_led);

ISR(TIMER1_COMPA_vect){
    cnt++;
    if(cnt >= sound_speed){
        cnt = 0;
        sound_cnt++;
        switch(daydream_cafe[sound_cnt+6]){
            case 0xEE:
                set_led(sound_led[0]);
                break;
            case 0xD4:
                set_led(sound_led[1]);
                break;
            case 0xBD:
                set_led(sound_led[2]);
                break;
            case 0xB2:
                set_led(sound_led[3]);
                break;
            case 0x85:
                set_led(sound_led[4]);
                break;
            case 0x9E:
                set_led(sound_led[5]);
                break;
            case 0x8D:
                set_led(sound_led[6]);
                break;
            case 0x7E:
                set_led(sound_led[7]);
                break;
            case 0x00:
                set_led(sound_led[8]);
                break;
        }

        if(sound_cnt >= 120){
            sound_cnt = 0;
        }
    }
    OCR2A = daydream_cafe[sound_cnt];
}

ISR(TIMER0_COMPA_vect){
    switch((~PINC >>  4) & 0x3){

        case 0:
            sound_speed = 13;
            break;
        case 1:
            sound_speed = 18;
            break;

        case 2:
            sound_speed = 8;
            break;
        case 3:
            break;
    }
    update_led();
}

void set_led(char add_led){
    int n;

    for(n = 6; n >= 0; n--){
        show_led[n+1] = show_led[n];
    }
    show_led[0] = add_led;
    show_led[7] = ~(0xFF & show_led[7]);
}

void update_led() {
    static unsigned char sc = 0xFE;
    static unsigned char scan = 0;

    PORTB = 0;    // 残像対策
    sc = (sc << 1) | (sc >> 7);
    PORTD = (PORTD & 0x0F) | (sc & 0xF0);    // 上位4ビット書き換え
    PORTC = (PORTC & 0xF0) | (sc & 0x0F);    // 下位4ビット書き換え
    scan = (scan + 1) & 7;
    PORTB = show_led[scan];
}

int main()
{
	DDRB = 0xFF;
	DDRC = 0x0F;
	DDRD = 0xFE;

	PORTB = 0xFF;
	PORTC = 0x30;
	PORTD = 0x00;

	TCCR2B = 0x04;
	TCCR2A = 0x12;
	OCR2A = 62;
	OCR2B = 0;

    TCNT1 = 0;
    TCCR1A = 2;
    TCCR1B = 4;
    OCR1A = 30;
    TIMSK1 |= _BV(OCIE1A);

    TCNT0 = 0;
    TCCR0A = 2;
    TCCR0B = 4;
    OCR0A = 30;
    TIMSK0 |= _BV(OCIE0A);

    sei();

	for (;;) {
		wdt_reset();
	}
	return 0;
}
