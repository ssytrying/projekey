/* -------------------------------------------------------------------------
 *                                                                           *
 *   Authors: Bengisu YUCEL, Seyit Semih YIGITARSLAN     Date: June 20, 2021 *
 *                                                                           *
 *   This file takes the inputs from the keypad, and shows this values in    * 
 *   Teraterm via UART. Also, addition, multiplication, division,            *
 *   subtraction operations exist.                                           *
 *                                                                           *
 *   REVISION HISTORY                                                        *
 *        DATE         AUTHORS       DESCRIPTION                             *
 *        --------     ------       -------------------------------------    *
 *        20.06.21     by, ssy       Release of MatrixKeypadProject.c        *
 *                                                                           *
   ------------------------------------------------------------------------- */
#include <stdint.h>
#include "PLL.h"
#include "UART.h"
#include "inc/tm4c123gh6pm.h"
#define NVIC_EN0_INT2           0x00000004  // Interrupt 2 enable
#define NVIC_EN0_INT4           0x00000010  // Interrupt 4 enable

int addition = 0;                        // addition process
int subtraction = 0;                        // substract process
int keyElements[16]={0};					   //	keypad elements	controlled the pressing repeat (initilaize whole key with 0) if it is pressed, it's value will one.
//int equal = 0;  it is written in function the equalivent process
int multiplication = 1;                      // multiplicationing process
int division = 1;                       // division process
int numArray[16] ={4, 5, 6, 0, 7, 8, 9,0,0,0,0,0,1,2, 3, 0};      // {these are 4,5,6,B,7,8,9,C,*,0,3,#,D,1,2,3,A}
//---------------------OutCRLF---------------------
/* -------------------------------------------------------------------------- *
 *                                                                            *
 *   this part is for UART newline process its copied from Valvano's Code.    *
 *                                                                            *
 * -------------------------------------------------------------------------- */

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
	
/* -------------------------------------------------------------------------- *
 *                                                                            *
 *   calculateDivide() takes two input values, print to TeraTerm result.      *
 *                                                                            *
 * -------------------------------------------------------------------------- */

void calculateDivide(int k,int l){
	if(l == 0){
		if(k == 0){
			UART_OutString("Result is undefined! ");		// if 0/0 is occured
		}else{
			UART_OutString("Result is infinity! ");			// if number/0 is occured
		}
	}
	else{
		int res1 = (k*10)/l;													// multiply with 10 to take float point
		int temp1 = res1/10;												
		UART_OutString(" %_% Result: ");	
		UART_OutUDec(k);															// show numerator to Teraterm	
		UART_OutString(" / ");	
		UART_OutUDec(l);															// show denominator to Teraterm		
		UART_OutString(" = ");
		int temp2=k/l;
		UART_OutUDec(temp2);													// show 1st digit of the result
		UART_OutChar('.');
		int lastResult = res1-temp1*10;								// calculate fractional number after the dot
		UART_OutUDec(lastResult);											// show fractional number after the dot
	}
}
/* -------------------------------------------------------------------------- *
 *                                                                            *
 *   joker() is taking the values from keypad as "1", "2", "3", "4", "A"etc. *
 *   "1", "2", "3", "5", "6", "7", "8", "9", "0", "A", "B", "C", "D", "*",    *
 *   "#" values.                                                              *
 *                                                                            *
 * -------------------------------------------------------------------------- */

void joker(int *array, int key){
	int i = 0;
	int equal; 
	for (i = 0; i< 16; i++){
		if(key == 15){						// if user entered the "A" value to make addition operation from keypad, jump this condition.
			if(array[i] == 1){			 
				addition = numArray[i];	  //addition gets the value	which is before the "A" pressed	
			}
			numArray[15] = 1; 			// make numArray's addition operator character 1 to imply function that	addition is occured
		}
		else if(key == 8){				//if user entered the "*" value to make multiplication operation from keypad, jump this condition.
			if(array[i] == 1){
				multiplication = numArray[i]; // multiplication value makes equals to numArray's wanted value to take user's number input.		
			}
			numArray[8] = 1;        //In the numArray, the place where the multiplication process is 1 and it is understood that the last multiplication is done.
		}		
		else if(key == 3){				//if user entered the "B" value to make division operation from keypad, jump this condition.
			if(array[i] == 1){
				division = numArray[i]; // multiplication value makes equals to numArray's wanted value to take user's number input.		
			}
			numArray[3] = 1;       //In the numArray, the place where the division process is 1 and it is understood that the last division is done.
		}		
		else if(key == 7){				//if user entered the "C" value to make division operation from keypad, jump this condition.
			if(array[i] == 1){
				subtraction = numArray[i]; // multiplication value makes equals to numArray's wanted value to take user's number input.		
			}
			numArray[7] = 1;       //In the numArray, the place where the division process is 1 and it is understood that the last division is done.
		}			
		else if(key == 11){      // if D value is entered
			if(array[i] == 1){			
				OutCRLF();
				if(numArray[15] == 1){												// enter the addition operation
					equal = addition + numArray[i];									// making addition operation
					UART_OutString("(¯L¯) Result: ");											
					UART_OutUDec(addition); UART_OutString(" + ");
					UART_OutUDec(numArray[i]);									// into Teraterm, write the equation
					UART_OutString(" = ");
					UART_OutUDec(equal);					
					numArray[15] = 0;														// addition operation is completed, makes 0 to "+"'s value to imply not do again addition operation
				}else if(numArray[8] == 1){										// enter the multiplication operation
					equal = multiplication * numArray[i];								//making addition operation						
					UART_OutString("*u* Result:  ");
					UART_OutUDec(multiplication); UART_OutString(" * ");
					UART_OutUDec(numArray[i]);									// into Teraterm, write the equation						
					UART_OutString(" = ");
					UART_OutUDec(equal);	
					numArray[8] = 0;														// multiplication operation is completed, makes 0 to "*"'s value to imply not do again multiplication operation
					
				}else if(numArray[3] == 1){										// enter the multiplication operation											
					calculateDivide(division,numArray[i]);				// make division operation		
					numArray[3] = 0;														// multiplication operation is completed, makes 0 to "*"'s value to imply not do again multiplication operation
				}else if(numArray[7] == 1){												// enter the addition operation
					if( subtraction >= numArray[i]){
						equal = subtraction - numArray[i];									// making subtraction operation
						UART_OutString(":-> Result: ");											
						UART_OutUDec(subtraction); UART_OutString(" - ");
						UART_OutUDec(numArray[i]);									// into Teraterm, write the equation
						UART_OutString(" = ");
						UART_OutUDec(equal);					
					}else{
						equal = numArray[i] - subtraction;									// making subtraction operation
						UART_OutString(" :-< Result: ");											
						UART_OutUDec(subtraction); UART_OutString(" - ");
						UART_OutUDec(numArray[i]);									// into Teraterm, write the equation
						UART_OutString(" = -");
						UART_OutUDec(equal);							
					}
					numArray[7] = 0;														// addition operation is completed, makes 0 to "+"'s value to imply not do again addition operation
				}else{
					UART_OutString("No operation ¯\\_(^'.'^)_\\¯");		// after the "D" is entered, if addition or multiplication is not occured, print error message
				}
			}
		}		
		else if(i != key){                                //if this key is not pressed, then its value is zero.
			array[i] = 0;
		}else{
			array[key] = 1;                                 //if this key is pressed, then its value is one.
		}
	}
	OutCRLF();
	bekle();                                           //delaying
}

void MatrixKeypad_Init(void){
  DisableInterrupts();
  // activate port C and port E
  SYSCTL_RCGCGPIO_R |= 0x14;  // enable ports C and E
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
		//UART_OutString("PC4\n");
  }	
  else if(GPIO_PORTC_RIS_R&0x20){  // poll PC5
    GPIO_PORTC_ICR_R = 0x20;  // acknowledge flag4
		//UART_OutString("PC5\n");
  }	
  else if(GPIO_PORTC_RIS_R&0x40){  // poll PC6
    GPIO_PORTC_ICR_R = 0x40;  // acknowledge flag4
		//UART_OutString("PC6\n");
  }	
  else if(GPIO_PORTC_RIS_R&0x80){  // poll PC7
		GPIO_PORTC_ICR_R = 0x80;  // acknowledge flag4
  }	
	else{
		volatile uint32_t errorCport = GPIO_PORTC_RIS_R;							// if unexpected Cport is entered, detect it.
	}
	GPIO_PORTC_ICR_R = 0xF0;
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
		if(GPIO_PORTC_RIS_R&0x10 && keyElements[12] == 0){       // poll PC4    
			UART_OutString("1");
			joker(keyElements,12);                                 //in joker function, the twelfth element in array is "1".     
		}	
		else if(GPIO_PORTC_RIS_R&0x20 && keyElements[13] == 0){  // poll PC5
			UART_OutString("2");
			joker(keyElements,13);
		}	
		else if(GPIO_PORTC_RIS_R&0x40 && keyElements[14] == 0){  // poll PC6
			UART_OutString("3");
			joker(keyElements,14);
		}		
		else if(GPIO_PORTC_RIS_R&0x80 && keyElements[15] == 0){  // poll PC7
			UART_OutString("A");
			joker(keyElements,15);
		}				
    GPIO_PORTE_ICR_R = 0x20;  // acknowledge flag4
		//UART_OutString("PE5\n");
  }	
  else if(GPIO_PORTE_RIS_R&0x04){  // poll PE2
		if(GPIO_PORTC_RIS_R&0x10 && keyElements[0] == 0){  // poll PC4      
			UART_OutString("4");
			joker(keyElements,0);
		}	
		else if(GPIO_PORTC_RIS_R&0x20 && keyElements[1] == 0){  // poll PC5
			UART_OutString("5");
			joker(keyElements,1);
		}	
		else if(GPIO_PORTC_RIS_R&0x40 && keyElements[2] == 0){  // poll PC6
			UART_OutString("6");
			joker(keyElements,2);
		}		
		else if(GPIO_PORTC_RIS_R&0x80 && keyElements[3] == 0){  // poll PC7
			UART_OutString("B");
			joker(keyElements,3);
		}						
    GPIO_PORTE_ICR_R = 0x04;  // acknowledge flag4
		//UART_OutString("PE2\n");
  }	
  else if(GPIO_PORTE_RIS_R&0x08 && keyElements[4] == 0){  // poll PE3
		if(GPIO_PORTC_RIS_R&0x10){  // poll PC4      
			UART_OutString("7");
			joker(keyElements,4);
		}	
		else if(GPIO_PORTC_RIS_R&0x20 && keyElements[5] == 0){  // poll PC5
			UART_OutString("8");
			joker(keyElements,5);
		}	
		else if(GPIO_PORTC_RIS_R&0x40 && keyElements[6] == 0){  // poll PC6
			UART_OutString("9");
			joker(keyElements,6);
		}		
		else if(GPIO_PORTC_RIS_R&0x80 && keyElements[7] == 0){  // poll PC7
			UART_OutString("C");
			joker(keyElements,7);
		}				
    GPIO_PORTE_ICR_R = 0x08;  // acknowledge flag4
		//UART_OutString("PE3\n");
  }	
  else if(GPIO_PORTE_RIS_R&0x10 && keyElements[8] == 0){  // poll PE4
		if(GPIO_PORTC_RIS_R&0x10){  // poll PC4      
			UART_OutString("*");
			joker(keyElements,8);
		}	
		else if(GPIO_PORTC_RIS_R&0x20 && keyElements[9] == 0){  // poll PC5
			UART_OutString("0");
			joker(keyElements,9);
		}	
		else if(GPIO_PORTC_RIS_R&0x40 && keyElements[10] == 0){  // poll PC6
			UART_OutString("#");
			joker(keyElements,10);
		}		
		else if(GPIO_PORTC_RIS_R&0x80 && keyElements[11] == 0){  // poll PC7
			UART_OutString("D");
			joker(keyElements,11);
		}						
    GPIO_PORTE_ICR_R = 0x10;  // acknowledge flag4
		//UART_OutString("PE4\n");
  }
	else{
		volatile uint32_t asd = GPIO_PORTE_RIS_R;
	}
  GPIO_PORTE_ICR_R = 0x3C;  // acknowledge flag4
	
}

int main(void){

//  char ch;
//  char string[20];  // global to assist in debugging
//  uint32_t n;

  PLL_Init();       // 50  MHz
  UART_Init();      // initialize UART
	MatrixKeypad_Init();
  OutCRLF();				// Jump new line
	UART_OutString("Welcome  (^3^)");
  OutCRLF();				// Jump new line
  while(1){
    WaitForInterrupt();							// wait until the user press the key from matrix keypad
  }		

}
