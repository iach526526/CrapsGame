//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  Texas Instruments, Inc
//  July 2013
//***************************************************************************************
#include <msp430.h>
#include <iostream>
//const unsigned char digitSegments[] = {
//    0xFC, // 0
//    0x60, // 1
//    0xDA, // 2
//    0xF2, // 3
//    0x66, // 4
//    0xB6, // 5
//    0xBE, // 6
//    0xE0, // 7
//    0xFE, // 8
//    0xF6, // 9
//    0x00,  // Blank
//    // W
//    // L
//};

#define redLED 0x02
#define greenLED 0x04
#define buttonPressed 0x01


const unsigned int digitSegments[] = {
    0x7E, // 0
    0x0C, // 1
    0xB6, // 2
    0x9E, // 3
    0xCC, // 4
    0xDA, // 5
    0xF8, // 6
    0x4E, // 7
    0xFE, // 8
    0xDE, // 9
    0x00,  // Blank
};
#include <time.h>
int rand() {//create a 0~5 random number
    //static���e�@�������G���N
    static long int seed =(time(NULL));;
    seed = (seed * 110351 + 12345) & 0x7fffffff;
    return seed%6;//return 0~5
}
void delay(unsigned int milliseconds) {
    unsigned int i;
    for (i = 0; i < milliseconds; i++) {
        __delay_cycles(1000); // Delay for 1000 cycles
    }
}

void close7()//�����C�q��ܾ�
{
    P1OUT &=0x01;
    P3OUT &=0x00;

}
void closeLED()
{
    P2OUT&=0xF9;
}
void displayNumber(unsigned int digit1, unsigned int digit2){
    //clear
   close7();
   P1OUT |= digitSegments[digit1];
   P3OUT |= digitSegments[digit2];
   int P3BIT3=P3OUT &0b00001000;
   P3BIT3>>=3;
   P3OUT|=P3BIT3;

}
void blink(unsigned int digit1, unsigned int digit2,int repeat,int frcq=300)
{
    int j;
    for(j=0;j<repeat;j++)
    {
        displayNumber(digit1,digit2);
        delay(frcq);
        close7();
        delay(frcq);
        displayNumber(digit1, digit2);
    }
}
void anime()//random animation
{
    int i;
    unsigned short wait=15;
    char table[]={3,1,4,1,5,9,2,6,5,3,5,8,9,7,9,3,3,1,4,1,5,9,2,6,5,0};
    for (i= 0; i < 25; i++){
        displayNumber(table[i],table[24-i]);
        delay(wait);
        wait+=6;
        if (i>22)
            wait+=70;
    }
}
void testDisplay()//����7�q��ܾ�
{
    int i;
    unsigned short wait=300;
    for (i= 0; i < 10; i++){
        displayNumber(i,i);
        delay(wait);
    }
    close7();
}
//��C++�h���ˤ@�ӦP�W�B���Ѽƪ���ơA���F�b���m��ܮɥi�H�������s
void testDisplay(short idle)//���m���աA�ɨ��ˬd���_
{
    int i;
    unsigned short wait=150;
    for (i= 0; i < 10; i++){
         if (P2IN & buttonPressed)
             return;
         else
         {
            displayNumber(i,i);
            delay(wait);
         }
    }
    close7();
}

int throw_dice()//���Y��l���ʧ@
{
    int dice1,dice2,total;
//    displayNumber(NULL, 0, 0);



    anime();
    dice1=rand()%6+1;dice2=rand()%6+1;
    total=dice1+dice2;
    //�{�{�j�թ�X�I��
    blink(dice1,dice2,2,600);

    blink(total/10, total%10,3,100);


    return total;
 }


void gameEnd(unsigned short result){
     int i;
     if (result){
         for (i = 0; i < 5; i++){
             P2OUT ^= greenLED; //iwin
             delay(300);
         }
         P2OUT |= greenLED; //iwin
     }
     else
     {
             for (i = 0; i < 10; i++){
                  P2OUT ^= redLED;
                  delay(200);
             }
         P2OUT |= redLED; //lose
     }
 }

 int game(){
     int totalValue,target;
     totalValue=throw_dice();

     //�Ĥ@����X7or11��Ĺ;2�B3�B12���;��l�]������ؼ�
     if (totalValue==7||totalValue==11){
         gameEnd(1);
         return 0;
     }else if(totalValue==2||totalValue==3||totalValue==12){
         gameEnd(0);
         return 0;
     }else{
         //��totalValue�����ಾ��target�W
         target=totalValue;
         totalValue=-1;
     }
     do{
         //�{�{target���ݫ��U���s
         if (P2IN & buttonPressed)
         {
             totalValue=throw_dice();
         }
         else
         {
             blink(target/10, target%10,2);
         }
     }while(totalValue!=7 && totalValue!=target) ;//�@�����ƪ����Y�X 7 or target

     if (totalValue==7){
         gameEnd(0);
         return 0;//����
     }
     else{
         gameEnd(1);
         return 0;//����
     }


 }


 int main()
 {
     WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
     PM5CTL0 &= ~LOCKLPM5;
     //init P1
     P1DIR |=0xFE;//set P1.1~1.7 as 7 segment display output

     //init P2
     P2DIR |=0x06;//set P2.1,P2.2 as LED output
     P2REN |= BIT0;     // �ҥ�P2.0�������U�Թq���A���X�ʭn��@�Ԩ��޿�0
     P2DIR &= ~BIT0;   // Set P2.0 as btn input
     P2OUT &=0x01;//����LED P2.1,P2.2

     //init P3
     P3DIR |= 0xFF;//set P3.0~3.7 as 7 segment display output
     testDisplay();
//     while(1)
//     {
//         if (P2IN & buttonPressed)
//
//             displayNumber(1,8);
//
//         else
//             displayNumber(0,8);                   // else reset
//     }
     while(1)
     {
         do
         {
             testDisplay(1);//�ݾ���ܼҦ�
         }while(!(P2IN & buttonPressed));
         game();
         close7();
         closeLED();
     }
  }
