// UARTTestMain.c
// Runs on LM4F120/TM4C123
// Used to test the UART.c driver
// Daniel Valvano
// May 30, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1
#include <stdint.h>
#include "PLL.h"
#include "UART.h"
#include "inc/tm4c123gh6pm.h"
#define NVIC_EN0_INT2           0x00000004  // Interrupt 2 enable
#define NVIC_EN0_INT4           0x00000010  // Interrupt 4 enable

int a = 0;
int tekrarA = 0;
int b = 0;
int tekrarB = 0;
int tekrarBl = 0;
int arti = 0;
int abbay[16]={0};
int esit = 0;
int numArray[16] ={4, 5, 6, 0, 7, 8, 9,0,0,0,0,0,0,1, 2, 3};
//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
//debug code


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// global variables visible in Watch window of debugger
// set when corresponding button pressed
volatile uint32_t SW1, SW2;
void bilmem(int *array, int klm){
	int i= 0;
	for (i = 0; i< 16; i++){
		if(klm == 15){
			if(array[i] == 1){
				arti = numArray[i];
			}
		}
		if(klm == 11){
			if(array[i] == 1){
				int esit = arti + numArray[i];
				OutCRLF();
				UART_OutString("Sonuc: ");
				UART_OutUDec(arti); UART_OutString(" + ");
				UART_OutUDec(numArray[i]);
				UART_OutString(" = ");
				UART_OutUDec(esit);
			}
		}		
		if(i != klm){
			array[i] = 0;
		}else{
			array[klm] = 1;
		}
	}
	OutCRLF();
}
void VectorButtons_Init(void){
  DisableInterrupts();
  // activate port C and port E
  SYSCTL_RCGCGPIO_R |= 0x14;  // enable ports C and E
  SW1 = 0;                    // clear semaphores
  SW2 = 0;
  GPIO_PORTC_DIR_R &= ~0xF0;  // make PC4 in (PC4 button) (default setting)
  GPIO_PORTC_AFSEL_R &= ~0xF0;// disable alt funct on PC4 (default setting)
  GPIO_PORTC_DEN_R |= 0xF0;   // enable digital I/O on PC4
  GPIO_PORTC_PCTL_R &= ~0xFFFF0000;// configure PC4 as GPIO (default setting)
  GPIO_PORTC_AMSEL_R &= ~0xF0;// disable analog functionality on PC4 (default setting)
  GPIO_PORTC_IS_R &= ~0xF0;   // PC4 is edge-sensitive (default setting)
  GPIO_PORTC_IBE_R &= ~0xF0;  // PC4 is not both edges (default setting)
  GPIO_PORTC_IEV_R |= 0xF0;   // PC4 rising edge event
  GPIO_PORTC_ICR_R = 0xF0;    // clear flag4
  GPIO_PORTC_IM_R |= 0xF0;    // enable interrupt on PC4
                              // GPIO PortC=priority 2
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00400000; // bits 21-23
  GPIO_PORTE_DIR_R &= ~0x3C;  // make PE4 in (PE4 button) (default setting)
  GPIO_PORTE_AFSEL_R &= ~0x3C;// disable alt funct on PE4 (default setting)
  GPIO_PORTE_DEN_R |= 0x3C;   // enable digital I/O on PE4
                              // configure PE4 as GPIO (default setting)
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFF0FFFF)+0x00000000;
  GPIO_PORTE_AMSEL_R &= ~0x3C;// disable analog functionality on PE4 (default setting)
  GPIO_PORTE_IS_R &= ~0x3C;   // PE4 is edge-sensitive (default setting)
  GPIO_PORTE_IBE_R &= ~0x3C;  // PE4 is not both edges (default setting)
  GPIO_PORTE_IEV_R |= 0x3C;   // PE4 rising edge event
  GPIO_PORTE_ICR_R = 0x3C;    // clear flag4
  GPIO_PORTE_IM_R |= 0x3C;    // enable interrupt on PE4
                              // GPIO PortE=priority 2
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000040; // bits 5-7
                              // enable interrupts 2 and 4 in NVIC
  NVIC_EN0_R = (NVIC_EN0_INT2+NVIC_EN0_INT4);
  EnableInterrupts();
}
void GPIOPortC_Handler(void){
	
	//unsigned long one = GPIO_PORTC_ICR_R ;     //acknowledge of
	//GPIO_PORTC_ICR_R = 0xF0;
  if(GPIO_PORTC_RIS_R&0x10){  // poll PC4
    GPIO_PORTC_ICR_R = 0x10;  // acknowledge flag4
    SW1 = 1;                  // signal SW1 occurred
		//UART_OutString("PC4\n");
  }	
  else if(GPIO_PORTC_RIS_R&0x20){  // poll PC5
    GPIO_PORTC_ICR_R = 0x20;  // acknowledge flag4
    SW1 = 1;                  // signal SW1 occurred
		//UART_OutString("PC5\n");
  }	
  else if(GPIO_PORTC_RIS_R&0x40){  // poll PC6
    GPIO_PORTC_ICR_R = 0x40;  // acknowledge flag4
    SW1 = 1;                  // signal SW1 occurred
		//UART_OutString("PC6\n");
  }	
  else if(GPIO_PORTC_RIS_R&0x80){  // poll PC7
		GPIO_PORTC_ICR_R = 0x80;  // acknowledge flag4
    SW1 = 1;                  // signal SW1 occurred
//	if (tekrarBl != 0){
//			if (b == 1){
//				UART_OutString("B Harfi\n");
//				b = 0;
//				tekrarBl =0;
//			}else{
//				tekrarBl = 1;
//				b = 1;
//			}
//		}			
//		if (tekrarA != 0){
//			if (a == 1){
//				//UART_OutString("A Harfi\n");
//				a = 0;
//				tekrarA =0;
//			}else{
//				tekrarA = 1;
//				a = 1;
//			}
//		}		
//    GPIO_PORTC_ICR_R = 0x80;  // acknowledge flag4
//    SW1 = 1;                  // signal SW1 occurred
		//UART_OutString("PC7\n");
  }	
	else{
		volatile uint32_t asd = GPIO_PORTE_RIS_R;
	}
	GPIO_PORTC_ICR_R = 0xF0;
  SW1 = 1;                    // signal SW1 occurred
}
void GPIOPortE_Handler(void){
  if(GPIO_PORTE_RIS_R&0x20){  // poll PE5
//		if (tekrarA != 0){
//			if (a == 1){
//				//UART_OutString("A Harfi\n");
//				a = 0;
//				tekrarA =0;
//			}else {
//				tekrarA = 1;
//				a = 1;
//			}
//		}	
//		tekrarA = 1;
		if(GPIO_PORTC_RIS_R&0x10 && abbay[12] == 0){  // poll PC4      // signal SW1 occurred
			UART_OutString("1");
			bilmem(abbay,12);
		}	
		else if(GPIO_PORTC_RIS_R&0x20 && abbay[13] == 0){  // poll PC5
			UART_OutString("2");
			bilmem(abbay,13);
		}	
		else if(GPIO_PORTC_RIS_R&0x40 && abbay[14] == 0){  // poll PC6
			UART_OutString("3");
			bilmem(abbay,14);
		}		
		else if(GPIO_PORTC_RIS_R&0x80 && abbay[15] == 0){  // poll PC7
			UART_OutString("A");
			bilmem(abbay,15);
		}				
    GPIO_PORTE_ICR_R = 0x20;  // acknowledge flag4
    //SW1 = 1;                  // signal SW1 occurred
		//UART_OutString("PE5\n");
  }	
  else if(GPIO_PORTE_RIS_R&0x04){  // poll PE2
		if(GPIO_PORTC_RIS_R&0x10 && abbay[0] == 0){  // poll PC4      // signal SW1 occurred
			UART_OutString("4");
			bilmem(abbay,0);
		}	
		else if(GPIO_PORTC_RIS_R&0x20 && abbay[1] == 0){  // poll PC5
			UART_OutString("5");
			bilmem(abbay,1);
		}	
		else if(GPIO_PORTC_RIS_R&0x40 && abbay[2] == 0){  // poll PC6
			UART_OutString("6");
			bilmem(abbay,2);
		}		
		else if(GPIO_PORTC_RIS_R&0x80 && abbay[3] == 0){  // poll PC7
			UART_OutString("B");
			bilmem(abbay,3);
		}						
    GPIO_PORTE_ICR_R = 0x04;  // acknowledge flag4
    //SW1 = 1;                  // signal SW1 occurred
		//UART_OutString("PE2\n");
  }	
  else if(GPIO_PORTE_RIS_R&0x08 && abbay[4] == 0){  // poll PE3
		if(GPIO_PORTC_RIS_R&0x10){  // poll PC4      // signal SW1 occurred
			UART_OutString("7");
			bilmem(abbay,4);
		}	
		else if(GPIO_PORTC_RIS_R&0x20 && abbay[5] == 0){  // poll PC5
			UART_OutString("8");
			bilmem(abbay,5);
		}	
		else if(GPIO_PORTC_RIS_R&0x40 && abbay[6] == 0){  // poll PC6
			UART_OutString("9");
			bilmem(abbay,6);
		}		
		else if(GPIO_PORTC_RIS_R&0x80 && abbay[7] == 0){  // poll PC7
			UART_OutString("C");
			bilmem(abbay,7);
		}				
    GPIO_PORTE_ICR_R = 0x08;  // acknowledge flag4
    //SW1 = 1;                  // signal SW1 occurred
		//UART_OutString("PE3\n");
  }	
  else if(GPIO_PORTE_RIS_R&0x10 && abbay[8] == 0){  // poll PE4
		if(GPIO_PORTC_RIS_R&0x10){  // poll PC4      // signal SW1 occurred
			UART_OutString("*");
			bilmem(abbay,8);
		}	
		else if(GPIO_PORTC_RIS_R&0x20 && abbay[9] == 0){  // poll PC5
			UART_OutString("0");
			bilmem(abbay,9);
		}	
		else if(GPIO_PORTC_RIS_R&0x40 && abbay[10] == 0){  // poll PC6
			UART_OutString("#");
			bilmem(abbay,10);
		}		
		else if(GPIO_PORTC_RIS_R&0x80 && abbay[11] == 0){  // poll PC7
			UART_OutString("D");
			bilmem(abbay,11);
		}						
    GPIO_PORTE_ICR_R = 0x10;  // acknowledge flag4
    //SW1 = 1;                  // signal SW1 occurred
		//UART_OutString("PE4\n");
  }
	else{
		volatile uint32_t asd = GPIO_PORTE_RIS_R;
	}
  GPIO_PORTE_ICR_R = 0x3C;  // acknowledge flag4
  //SW2 = 1;                    // signal SW2 occurred
	
}

int main(void){

  char ch;
  char string[20];  // global to assist in debugging
  uint32_t n;

  PLL_Init();       // 50  MHz
  UART_Init();              // initialize UART
  OutCRLF();
  for(ch='A'; ch<='Z'; ch=ch+1){// print the uppercase alphabet
    UART_OutChar(ch);
  }
  OutCRLF();
  UART_OutChar(' ');
  for(ch='a'; ch<='z'; ch=ch+1){// print the lowercase alphabet
    UART_OutChar(ch);
  }
  OutCRLF();
  UART_OutChar('-');
  UART_OutChar('-');
  UART_OutChar('>');
	VectorButtons_Init();
  while(1){
    WaitForInterrupt();
  }		
  while(1){
    UART_OutString("InString: ");
    UART_InString(string,19);
    UART_OutString(" OutString="); UART_OutString(string); OutCRLF();

    UART_OutString("InUDec: ");  n=UART_InUDec();
    UART_OutString(" OutUDec="); UART_OutUDec(n); OutCRLF();

    UART_OutString("InUHex: ");  n=UART_InUHex();
    UART_OutString(" OutUHex="); UART_OutUHex(n); OutCRLF();

  }
}
