#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#define LED_dir DDRB
#define LED PORTB
#define BUZZ PORTK_PK5
#define BUZZ_dir DDRK_DDRK5
#define KEY1 PTIH_PTIH3
#define KEY2 PTIH_PTIH2
#define KEY3 PTIH_PTIH1
#define KEY4 PTIH_PTIH0
#define KEY1_dir DDRH_DDRH3
#define KEY2_dir DDRH_DDRH2
#define KEY3_dir DDRH_DDRH1
#define KEY4_dir DDRH_DDRH0
#define CONT1 PORTK_PK3
#define CONT2 PORTK_PK2
#define CONT3 PORTK_PK1
#define CONT4 PORTK_PK0
#define CONT1_dir DDRK_DDRK3
#define CONT2_dir DDRK_DDRK2
#define CONT3_dir DDRK_DDRK1
#define CONT4_dir DDRK_DDRK0
#define DATA PTP
#define DATA_dir DDRP
#define BUS_CLOCK 32000000 
byte shuma[20]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,       
                0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef};
 
 
byte data1 = 0;
byte data2 = 0;
byte data3 = 0;
byte data4 = 0;
byte single = 1;
byte clock=2;
  


void INIT_PLL(void) 
{
    CLKSEL &= 0x7f;       //set OSCCLK as sysclk
    PLLCTL &= 0x8F;       //Disable PLL circuit
    CRGINT &= 0xDF;
    
    #if(BUS_CLOCK == 40000000) 
      SYNR = 0x44;
    #elif(BUS_CLOCK == 32000000)
      SYNR = 0x43;     
    #elif(BUS_CLOCK == 24000000)
      SYNR = 0x42;
    #endif 

    REFDV = 0x81;         //PLLCLK=2×OSCCLK×(SYNDIV+1)/(REFDIV+1)＝64MHz ,fbus=32M
    PLLCTL =PLLCTL|0x70;  //Enable PLL circuit
    asm NOP;
    asm NOP;
    while(!(CRGFLG&0x08)); //PLLCLK is Locked already
    CLKSEL |= 0x80;        //set PLLCLK as sysclk
}

/************************************************************/
/*                    初始化TIM模块                         */
/************************************************************/
void initialize_TIM(void){
  TSCR1_TFFCA = 1;  
  TSCR1_TEN = 1;    
  TIOS  = 0xff;     
  TCTL1 = 0x00;	    
  TCTL2 = 0x00;     
  TIE   = 0x01;    
  TSCR2 = 0x07;	   
  TFLG1 = 0xff;	    
  TFLG2 = 0xff;     
}

/*************************************************************/
/*                      初始化端口                           */
/*************************************************************/
void INIT_port(void) 
{
  CONT1_dir = 1;
  CONT2_dir = 1;
  CONT3_dir = 1;
  CONT4_dir = 1;
  CONT1 = 0;
  CONT2 = 0;
  CONT3 = 0;
  CONT4 = 0;
  DATA_dir = 0xff;
  DATA = 0x00;
}
void init_key(void)  //初始化按键
{
     KEY1_dir =0;       
     KEY2_dir=0;
     KEY3_dir=0;
     KEY4_dir=0;
     PPSH = 0x00;		      
     PIFH = 0x0f;					
     PIEH = 0x0f;		      
}
void inter_led_buzz(){
      
      LED_dir=0xff;
      LED=0xff;
      BUZZ_dir=1;
      BUZZ=0;
}


/*************************************************************/
/*                        延时函数                           */
/*************************************************************/
void delay(void) 
{
 unsigned int i,j;
 for(j=0;j<40;j++)
 for(i=0;i<60000;i++);
}

/*************************************************************/
/*                      中断扫描函数                         */
/*************************************************************/
#pragma CODE_SEG __NEAR_SEG NON_BANKED
 interrupt void PTH_inter(void) 
{
    
   if(PIFH != 0) //判断中断标志    
   {
      PIFH = 0xff;//清除中断标志     
      if(KEY1 == 0)         
      {
        clock=0; 
             
      }
      if(KEY2 == 0) 
      {
       clock=1;   
      }
      //if(KEY3 == 0)
         
      //if(KEY4 == 0)
        
   }
}

interrupt void scan(void)
{
  if(TFLG1_C0F == 1)
  {
    TFLG1_C0F = 1;
    TC0 = TCNT + 1250;         
  }
  switch(single)
  {
    case 1:
      CONT1 = 1;
      CONT2 = 0;
      CONT3 = 0;
      CONT4 = 0;
      DATA=shuma[data1];
      break;

    case 2:
      CONT1 = 0;
      CONT2 = 1;
      CONT3 = 0;
      CONT4 = 0;
      DATA=shuma[data2];
      break;

    case 3:
      CONT1 = 0;
      CONT2 = 0;
      CONT3 = 1;
      CONT4 = 0;
      DATA=shuma[data3];
      break;

    
    default:
      break;
  }
  
  single +=1;
  if(single == 4) single = 1;
}

#pragma CODE_SEG DEFAULT


/*************************************************************/
/*                         主函数                            */
/*************************************************************/
void main(void) {
  int i=0,j=0,k=0;
  DisableInterrupts; 
  INIT_PLL();
  initialize_TIM();
  INIT_port();
  init_key();
  inter_led_buzz();
  EnableInterrupts;

  TFLG1_C0F = 1;
  TC0 = TCNT + 1250; 
  for(;;) {       
  if(clock==0){
   
   i=i+1;
   if(i>9){
    j=j+1;
    i=0;
   }
   if(j>5){
    k=k+1;
    j=0;
   }
   data1=k;
   delay();
   data2=j;
   delay();
   data3=i;
   delay();
  
   if(k==1&&j==0&&i==0){
      LED=0x7f;
   }
   if(k==2){
      LED=0x3f;
   }
   if(k==2){
    break;
   }
   if(clock==1){
    break;
   }
  
   } 
  }
 for(;;){
  
 if(clock==1){
  if(k==2&&j==0&&i==0){
    k=1;
    j=5;
    i=10;
  }
  if(k==1&&j==0&&i==0){
    k=0;
    j=5;
    i=10;
  }
  i=i-1;
  if(i<1){
    if(j==0){
      break;
    }
    j=j-1;
    i=9;
  }
  if(j<1){
    if(k==0){
      break;
    }
    k=k-1;
    j=5;
  }
    data1=k;
    delay();
    data2=j;
    delay();
    data3=i;
    delay();
   
    if(k==0&&j==0&&i==0){
      BUZZ=1;
    }
    if(k==0&&j==0&&i==0){
      break;
    }
  
  }
    
  }
    
    
  }


  
 




