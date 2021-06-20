/* -------------------------------------------------------------------------
 *                                                                           *
 *   Authors: Bengisu YUCEL, Seyit Semih YIGITARSLAN     Date: June 20, 2021 *
 *                                                                           *
 *   This file takes the inputs from the keypad, and shows this values in    * 
 *   Teraterm via UART. Also, adding, and multiplying                        *
 *	 operations exists.                                                      *
 *                                                                           *
 *   REVISION HISTORY                                                        *
 *        DATE         AUTHORS       DESCRIPTION                             *
 *        --------     ------       -------------------------------------    *
 *        20.06.21     by, ssy       Release of ( ).c                        *
 *                                                                           *
   ------------------------------------------------------------------------- */
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
int abbay[16]={0};																							// initilaize whole key with 0, it will use if 
int esit = 0;
int carpma = 1;
int numArray[16] ={4, 5, 6, 0, 7, 8, 9,0,0,0,0,0,1,2, 3, 0};
//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none

// This function used as enter in Teraterm 
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

/* -------------------------------------------------------------------------- *
 *                                                                            *
 *   bekle() waitings after the press key, and disabled ports to enter new    *
 *   values.                                                                  *
 *                                                                            *
 * -------------------------------------------------------------------------- */

void bekle(){
	int i = 0;
	for(i=1; i< 10000000;i++){
	}
	GPIO_PORTE_ICR_R = 0x3C;		//Acknowledge Port E, and it can take new E port values as PE5, PE2, PE3, PE4
	GPIO_PORTC_ICR_R = 0xF0;		//Acknowledge Port C, and it can take new E port values as PC4, PC5, PC6, PC7
}
	
// global variables visible in Watch window of debugger
// set when corresponding button pressed
volatile uint32_t SW1, SW2;

/* -------------------------------------------------------------------------- *
 *                                                                            *
 *   bilmem() is taking the values from keypad as "1", "2", "3", "4", "A"etc. *
 *   "1", "2", "3", "5", "6", "7", "8", "9", "0", "A", "B", "C", "D", "*",    *
 *   "#" values.                                                              *
 *   values.                                                                  *
 *                                                                            *
 * -------------------------------------------------------------------------- */

void bilmem(int *array, int klm){
	int i= 0;
	int esit;
	for (i = 0; i< 16; i++){
		if(klm == 15){						// if user entered the "A" value to make adding calculation from keypad, jump this condition.
			if(array[i] == 1){			// 
				arti = numArray[i];		// arti degeri numArray(numarraydaki deger) degerine atandi			
			}
			numArray[15] = 1; 			// make numArray's adding operator character 1 to imply function that	adding is occured
		}
		else if(klm == 8){				//if user entered the "*" value to make multiplication calculation from keypad, jump this condition.
			if(array[i] == 1){
				carpma = numArray[i]; // carpma value makes equals to numArray's wanted value to take user's number input.		
			}
			numArray[8] = 1; //numArrayde carpma isleminin oldugu yer 1 oldu ve en son carpma isleminin yapildigi anlasiliyor.
		}		
		else if(klm == 11){
			if(array[i] == 1){			
				OutCRLF();
				if(numArray[15] == 1){												//Toplama Islemine giriyor
					esit = arti + numArray[i];
					UART_OutString("Result: ");
					UART_OutUDec(arti); UART_OutString(" + ");
					UART_OutUDec(numArray[i]);
					UART_OutString(" = ");
					UART_OutUDec(esit);					
					numArray[15] = 0;
				}else if(numArray[8] == 1){										// Carpma Islemine giriyor.
					esit = carpma * numArray[i];
					UART_OutString("Result: ");
					UART_OutUDec(carpma); UART_OutString(" * ");
					UART_OutUDec(numArray[i]);
					UART_OutString(" = ");
					UART_OutUDec(esit);	
					numArray[8] = 0;														// Carpma islemi degerini sifirla, yani carpma gorevini tamamladi.
					
				}else{
					UART_OutString("No adding or multiplication is found! ");
				}
			}
		}		
		else if(i != klm){
			array[i] = 0;
		}else{
			array[klm] = 1;
		}
	}
	OutCRLF();
	bekle();
}
void MatrixKeypad_Init(void){
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
		volatile uint32_t errorCport = GPIO_PORTC_RIS_R;							// if unexpected Cport is entered, detect it
	}
	GPIO_PORTC_ICR_R = 0xF0;
  //SW1 = 1;                    // signal SW1 occurred
}

// PE5 && PC4 -> "1"
// PE5 && PC5 -> "2"
// PE5 && PC6 -> "3"
// PE5 && PC7 -> "A"
// PE2 && PC4 -> "4"
// PE2 && PC5 -> "5"
// PE2 && PC6 -> "6"
// PE2 && PC7 -> "B"
// PE3 && PC4 -> "7"
// PE3 && PC5 -> "8"
// PE3 && PC6 -> "9"
// PE3 && PC7 -> "C"
// PE4 && PC4 -> "*"
// PE4 && PC5 -> "0"
// PE4 && PC6 -> "#"
// PE4 && PC7 -> "D"

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

//  char ch;
//  char string[20];  // global to assist in debugging
//  uint32_t n;

  PLL_Init();       // 50  MHz
  UART_Init();      // initialize UART
  OutCRLF();				// Jump new line
/*
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
*/
	MatrixKeypad_Init();
  while(1){
    WaitForInterrupt();
  }		
/*  while(1){
    UART_OutString("InString: ");
    UART_InString(string,19);
    UART_OutString(" OutString="); UART_OutString(string); OutCRLF();

    UART_OutString("InUDec: ");  n=UART_InUDec();
    UART_OutString(" OutUDec="); UART_OutUDec(n); OutCRLF();

    UART_OutString("InUHex: ");  n=UART_InUHex();
    UART_OutString(" OutUHex="); UART_OutUHex(n); OutCRLF();

  }
*/
}
