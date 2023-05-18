#include<8051.h>

__bit t0_flag=0;

int counter=0;

__code unsigned char SYMBOLE[10]= {0b0111111, 0b0000110, 0b1011011, 0b1001111,
                                   0b1100110, 0b1101101, 0b1111101, 0b0000111, 0b1111111, 0b1101111
                                  };

unsigned char led_b=1;
unsigned char led_p;


unsigned char seconds;
unsigned char minutes;
unsigned char hours;

unsigned char KEYS[4];
unsigned char CLOCK[6];

__xdata unsigned char *led_wyb=(__xdata unsigned char *) 0xFF30;
__xdata unsigned char *led_led=(__xdata unsigned char *) 0xFF38;

void przelicz(void){
    CLOCK[0]=seconds%10;
    CLOCK[1]=seconds/10;
    CLOCK[2]=minutes%10;
    CLOCK[3]=minutes/10;
    CLOCK[4]=hours%10;
    CLOCK[5]=hours/10;
    return;
}


void obsluga_klawiatury(void){
    switch(KEYS[0]){
        case(0b100010):{
            if(hours!=23) ++hours;
            else hours=0;
        }break;
        
        case(0b100001):{
            if(hours!=0) --hours;
            else hours=23;
        }break;

        case(0b10010):{
            if(minutes!=59) ++minutes;
            else minutes=0;
        }break;

        case(0b10001):{
            if(minutes!=0) --minutes;
            else minutes=59;
        }break;

        case(0b110):{
            if(seconds!=59) ++seconds;
            else seconds=0;
        }break;

        case(0b101):{
            if(seconds!=0) --seconds;
            else seconds=59;
        }break;

        case(0b1000):{
            seconds=0;
            counter=0;
        }break;
    }
    przelicz();
    return;
}

void klawiatura_niestabilna(void){
    KEYS[3]=KEYS[2];
    KEYS[2]=KEYS[1];
    KEYS[1]=KEYS[0];
    KEYS[0]=0;
}


void main() {

    TL0 = 252;
    TH0 = 227;

    //PS = 1;
    ES = 1;
    ET0 = 1;
    EA = 1;
    TR0 = 1;

    hours=11;
    minutes=20;
    seconds=55;
    przelicz();


    KEYS[0]=0;
    KEYS[1]=0;
    KEYS[2]=0;
    KEYS[3]=0;


    P1_6=0;
    *led_wyb=1;
    led_p=0;

    while(1) {
            if(t0_flag) {
            	t0_flag=0;
            	P1_6=1;
                *led_led=SYMBOLE[CLOCK[led_p]];
                ++led_p;

                *led_wyb=led_b;
            	P1_6=0;
            	if(P3_5==1) KEYS[0]=(KEYS[0] | led_b);
             	led_b+=led_b;
             	if(led_b==0b1000000){   //led_p=6;
                    led_b=1;
                    led_p=0;

                    if(KEYS[0]==KEYS[1] && KEYS[0]==KEYS[2] && KEYS[0]!=KEYS[3] && KEYS[0]!=0){
                        obsluga_klawiatury();
                    }
                    klawiatura_niestabilna();
                }

                //noACC7  liczy sekunde
                if(counter==1024){
		            counter=0;
                    ++seconds;
                    if(seconds!=60) przelicz();
                    else{
                        seconds=0;
                        ++minutes;
                        }
                    if(minutes!=60) przelicz();
                    else{
                        minutes=0;
                        ++hours;
                        }
                    if(hours!=24) przelicz();
                    else{
                        hours=0;
                        minutes=0;
                        seconds=0;
                    }
                    przelicz();
                }
            }
        }
}

void t0_int(void)__interrupt(1) {
    TL0 = 252;
    TH0 = 227;
    t0_flag = 1;
    ++counter;
}

