#include <msp430.h> 
#include <stdint.h>
#include "ETF_5529_HAL/hal_7seg.h"

/**
 * main.c
 */

#define DELAY_CNT       (104857)


volatile uint16_t data = 0;

uint8_t     lowDigit;
uint8_t     highDigit;
uint8_t i = 0;



typedef enum{
    DISP1,
    DISP2
}active_display;


uint8_t getLowDigit(uint16_t number){
    if(number > 99) return 0x0a;
    return (number - 10*(number/10));
}
uint8_t getHighDigit(uint16_t number){
    if(number > 99) return 0x0a;
    return (number/10);
}

active_display showDisplay(uint16_t data_, active_display activeDisplay){

    lowDigit    = getLowDigit(data_);
    highDigit   = getHighDigit(data_);
    switch(activeDisplay){
    case DISP1:
        HAL_7SEG_DISPLAY_1_ON;
        HAL_7SEG_DISPLAY_2_OFF;
        HAL_7Seg_WriteDigit(highDigit);
        __delay_cycles(DELAY_CNT/40);
        activeDisplay = DISP2;
        break;
    case DISP2:
        HAL_7SEG_DISPLAY_2_ON;
        HAL_7SEG_DISPLAY_1_OFF;
        HAL_7Seg_WriteDigit(lowDigit);
        __delay_cycles(DELAY_CNT/40);
        activeDisplay = DISP1;
        break;
    }
    return activeDisplay;
}



active_display activeDisplay;



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    HAL_7Seg_Init();
    activeDisplay = DISP1;

       P2DIR |= (BIT4|BIT5);   // Configure P2.4 as out
       P2OUT &= ~BIT4;
       P2OUT &= ~BIT5;



         P1DIR &= ~(BIT4|BIT5);             // set P1.4 as in
         P1REN |= (BIT4|BIT5);              // This is important because there is no PullUp Resistor on the board
         P1OUT |= (BIT4|BIT5);              // This is important because there is no PullUp Resistor on the board
         P1IES |= (BIT4|BIT5);              // set P1.4 irq as h->l transition
         P1IFG &= ~(BIT4|BIT5);             // clear P1.4 IFG
         P1IE |= (BIT4|BIT5);               // enable P1.4 irq



    __enable_interrupt();


       while (1)
       {


           activeDisplay = showDisplay(data,activeDisplay);


           if(data< 20 && data>=0){
               P2OUT &= ~BIT4;
               P2OUT &= ~BIT5;
           }
           else if(data< 40 && data>=20)
           {
               P2OUT |= BIT4;
               P2OUT &= ~BIT5;
           }
           else if(data< 60 && data>=40){
               P2OUT |= BIT5;
               P2OUT &= ~BIT4;
           }
           else if(data< 80 && data>=60){
               P2OUT |= BIT4;
               P2OUT |= BIT5;
           }
           else if(data< 90 && data>=80){
            // __delay_cycles(DELAY_CNT*2.5);
             i = 0;
             while(i<40){
                 activeDisplay = showDisplay(data,activeDisplay);
                 i += 1;
             }


                P2OUT ^= BIT4;          // Toggle P4.3
                P2OUT &= ~BIT5;

           }
           else if(data< 100 && data>=90){
              // __delay_cycles(DELAY_CNT*2.5);
               i = 0;
               while(i<40){
                   activeDisplay = showDisplay(data,activeDisplay);
                   i += 1;
               }
               P2OUT ^= BIT5;          // Toggle P4.3
               P2OUT &= ~BIT4;
           }



       }

}

void __attribute__ ((interrupt(PORT1_VECTOR))) P1ISR (void)
{
    __delay_cycles(DELAY_CNT/40);

    if ((BIT4 & P1IFG) != 0)                // check if P2.4 flag is set
    {
        if ((BIT4 & P1IN) == 0)             // check if P2.4 is still pressed
        {
            data +=2;
            data = data>= 100 ? 99:data;

        }
        P1IFG &= ~BIT4;             // clear interrupt flag
    }
    if ((BIT5 & P1IFG) != 0)                // check if P2.4 flag is set
    {
        if ((BIT5 & P1IN) == 0)             // check if P2.4 is still pressed
        {
            data-=2;

            data = data<0?0:data;

        }
        P1IFG &= ~BIT5;             // clear interrupt flag
    }
    return;
}
