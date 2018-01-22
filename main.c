/*----------------------------------------------------------------------------
 * Name:    Zhou Shen
 * Purpose: Individual Project
 * Note(s): answer questions and get score at the end
 *            
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "LPC17xx.H" 
#include "stdio.h"
#include "stdlib.h"
#include "type.h"
#include "UART.h"
#include "GLCD.h"
#include "LED.h"
#include "ADC.h"
#include "Serial.h"
#include "CRIS_UTILS.h"

#define PORT_USED 0
#define DAC_BIAS (0x1<<16)
#define DATA_LENGTH	0x400
#define DEBUG 1 //for bluetooth

char text[10];

extern uint16_t AD_last;  // Last converted value    
extern uint8_t  AD_done;  // AD conversion done flag  
extern uint8_t  clock_1s; // Flag activated each second

uint8_t input;//keyboard buffer for character recognition
extern volatile uint32_t UART0_Count;
extern volatile uint32_t UART2_Count;
extern volatile uint8_t UART0_Buffer[256];
extern volatile uint8_t UART2_Buffer[10];
char printAnswer[10];//answer display buffer

char* answers;
char* buffer;

int numbers;
int result;
int c, deep, timer; //for bluetooth

/*
* Make some sound
*/
void play_sound(){//from D2L examples
  int am=0;
	int k=0;
  uint32_t m;
  
	while ( am < 4000) {//length of tone
		LPC_DAC->DACR = (k << 13) | DAC_BIAS; //tone;13
		k++;
		for (m = 800; m>1; m--);
		if ( k == DATA_LENGTH) {
			k = 0;
		}
		am++;
  }
}
/*
void answerDisplay(){ //Display answers are correct or wrong
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Green);
	GLCD_DisplayString(1, 50, (unsigned char *) "Your answer is ");
	sprintf(printAnswer, "%s", answers);
}
*/

void answerDetect(){ //Detect whether users got right or wrong answer
	if(input == 'A'){ //get the correct answer
		numbers++;
		result++;
	}
	else if (input == 'B' || input == 'C' || input == 'D'){ //get the wrong answer
		numbers++;
		result--;
	}
}

void TIMER0_IRQHandler(void){
	if((LPC_TIM0->IR & 0x01) == 0x01){
		LPC_TIM0->IR |= 1 << 0;
		answerDetect();
		//answerDisplay();
		timer = !timer;
		//if(booms > 0)
		deep++;
	}
}

void delay_sec(int del){
	int i;
	for(i = 0; i < (10000000 * del); i++);
}

void receive(void){ //for bluetooth
	deep=0;
	while ((deep < 5) && (UART0_Count < BUFSIZE)){
		if ((c = SER_getChar_nb(2)) != 0){
			UART0_Buffer[UART0_Count++] = c;
		}
	}
	
	UARTSend(0, (uint8_t *)UART0_Buffer, UART0_Count);
	UART0_Count = 0;
}


void endPage(){//Display win condition
	NVIC_DisableIRQ(TIMER0_IRQn);//Stop timer
	GLCD_Clear(Black);
	play_sound();
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Red);
	GLCD_DisplayString(0, 0,(unsigned char *) "You score is: ");
	sprintf(printAnswer, "%d", result);
	GLCD_DisplayString(1, 0, (unsigned char *) printAnswer);
	GLCD_DisplayString(2, 0,(unsigned char *) "Please wait for upd-");
	GLCD_DisplayString(3, 0,(unsigned char *) "ate");	
}

/*
* Dirty delay function for seeting uo time
*/
void delay(int n){   //Dirty delay function
  volatile int d;
  for (d=0; d<n*10000; d++){}
}

/*
* Reset clears the screen compleetly 
*/
void reset(){
	GLCD_Clear(White);//Draw white multiple times to actually delay the function
	GLCD_Clear(White);
	GLCD_Clear(White);
	GLCD_Clear(White);
	delay(300);
	
	GLCD_SetTextColor(Blue);
	//delay(5);
	
	//reset to the first question
	GLCD_DisplayString(0, 0,(unsigned char *) "1. 1+1 = ?");
	GLCD_DisplayString(1, 0,(unsigned char *) "A. 2  B.3  C. 4  D.5");
	GLCD_DisplayString(2, 0,(unsigned char *) "2. 2*3 = ?");
	GLCD_DisplayString(3, 0,(unsigned char *) "A. 6  B.5  C. 7  D.2");
	GLCD_DisplayString(4, 0,(unsigned char *) "3. 32/16 = ?");
	GLCD_DisplayString(5, 0,(unsigned char *) "A. 2  B.3  C. 4  D.1");
	GLCD_DisplayString(6, 0,(unsigned char *) "Bonus 4");
	GLCD_DisplayString(7, 0,(unsigned char *) "How is Dr.Ababei ?");
	GLCD_DisplayString(8, 0,(unsigned char *) "A. Awesome!!!");
}

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) 
{
  uint32_t ad_avg = 0;
  uint16_t ad_val = 0, ad_val_previous = 0xFFFF;
   
  // (1) initializations;
  SystemInit();
  
  // Generate interrupt each 10 ms;
  SysTick_Config(SystemCoreClock/100);  
  
  // LED Initialization
  LED_Init(); 
  
  // ADC Initialization 
  ADC_Init(); 
  
  // UART0 initialization;
	UARTInit(0, 9600);
	UARTInit(2, 9600);
	SER_init(2, 9600);
  
  // LCD display initialization;
  GLCD_Init();
	GLCD_Clear(White);
	//delay(200);
  //LCD_PutText(32,144, "Welcome to Math World", White, Red);
  //LCD_PutText(32,160, "Young explorer!", White, Red);  
  GLCD_DisplayString(1, 102, (unsigned char *) "Welcome");
	GLCD_DisplayString(2, 98, (unsigned char *) "Young Explorer!");
	CRIS_draw_smiley_face(150, 150, 25); //draw a smelly face after welcome logo
 
	// Timer 0 Configuration
	LPC_SC->PCONP |= 1 << 1; // Power up Timer 0 
  LPC_SC->PCLKSEL0 |= 1 << 2; // Clock for timer = CCLK, i.e., CPU Clock
  LPC_TIM0->MR0 = 1 << 25; // 24: give a value suitable for the LED blinking  // frequency based on the clock frequency 
  LPC_TIM0->MCR |= 1 << 0; // Interrupt on Match 0 compare 
  LPC_TIM0->MCR |= 1 << 1; // Reset timer on Match 0    
  LPC_TIM0->TCR |= 1 << 1; // Manually Reset Timer 0 (forced) 
  LPC_TIM0->TCR &= ~(1 << 1); // Stop resetting the timer 
  
	NVIC_EnableIRQ(TIMER0_IRQn);
	//start the questions
	delay(1500); //delay for welcome interface
	numbers = 0;
	reset();
	LPC_TIM0->TCR |= 1 << 0; // Start timer 
	
	//Setup sound
	LPC_PINCON->PINSEL1 |= 0x02<<20;
	LPC_SC->PCLKSEL0 |= 1<<24;
	LPC_DAC->DACCTRL &= 0xE;
 
  // (2) the "forever" loop;
  while (1) {
    // (a) get AD convertion result and compute average of the last 16 values; 
    if (AD_done) { // If conversion has finished 
      AD_done = 0; // reset to prepare for a new conversion

      ad_avg += AD_last << 8; // Add AD value to averaging 
      ad_avg ++;
      if ((ad_avg & 0xFF) == 0x10) { // average over 16 values  
        ad_val = (ad_avg >> 8) >> 4; // average devided by 16
        ad_avg = 0;
      }
    }
		
		//(b) play the questions at the same time
		if ( UART2_Count != 0 ){  //USE uart 2
      LPC_UART2->IER = IER_THRE | IER_RLS; // Disable RBR 
			input = *UART2_Buffer;
      //UARTSend( 2, (uint8_t *)UART2_Buffer, UART2_Count );
      UART2_Count = 0;
      LPC_UART2->IER = IER_THRE | IER_RLS | IER_RBR; // Re-enable RBR 
    }
		//Keyboard Control, only A is the right answer
		//WILL override the previous answer
		GLCD_SetBackColor(White);
		GLCD_SetTextColor(Red);
		if(input == 'A' )
			GLCD_DisplayString(0, 15, (unsigned char *) "Y");
		else if(input == 'B' )//right
			GLCD_DisplayString(0, 15, (unsigned char *) "N");
		else if(input == 'C' )//down
			GLCD_DisplayString(0, 15, (unsigned char *) "N");
		else if(input == 'D' )//left
			GLCD_DisplayString(0, 15, (unsigned char *) "N");
		
		//End condition
		if(numbers >=4){
			endPage(); 
	
			
			//below for BT	
			//PURPOSE is to send the encouragement for several times
			SER_putString(2, "Congratualations for complement! Hope you learned!\r\n");
			receive();
			break;
		}
		
  } // the "forever" loop;
 
}