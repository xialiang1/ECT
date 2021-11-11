#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#define LED_dir DDRB
#define LED PORTB
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
                
                
void INIT_PLL(void)  //��ʼ�����໷
{
    CLKSEL &= 0x7f;       
    PLLCTL &= 0x8F;       
    CRGINT &= 0xDF;
    
    #if(BUS_CLOCK == 40000000) 
      SYNR = 0x44;
    #elif(BUS_CLOCK == 32000000)
      SYNR = 0x43;     
    #elif(BUS_CLOCK == 24000000)
      SYNR = 0x42;
    #endif 

    REFDV = 0x81;         
    PLLCTL =PLLCTL|0x70;  
    asm NOP;
    asm NOP;
    while(!(CRGFLG&0x08)); 
    CLKSEL |= 0x80;        
}


void INIT_port(void)//��ʼ������� 
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


void init_key(void)  //��ʼ������
{
     KEY1_dir =0;       
     KEY2_dir=0;
     KEY3_dir=0;
     KEY4_dir=0;
     PPSH = 0x00;		      
     PIFH = 0x0f;					
     PIEH = 0x0f;		      
}


void Output_Compare()   //��ʼ������Ƚ�
{                      
  ECT_TSCR1_TFFCA=1;
  ECT_TSCR1_TEN=1;
  ECT_TIOS=0xff;
  ECT_TCTL1=0x00;
  ECT_TCTL2=0x00;
  ECT_TIE=0x00;
  ECT_TSCR2=0x07;
  ECT_TFLG1=0xff;
  ECT_TFLG2=0xff;
}


void Input_Capture()   //��ʼ�����벶׽
{  
  ECT_TSCR1_TFFCA=1;
  ECT_TSCR1_TEN=1;
  ECT_TIOS=0x00;
  ECT_TCTL1=0x00;
  ECT_TCTL2=0x00;
  ECT_TIE=0x00;
  ECT_TSCR2=0xc7;
  ECT_TFLG1=0xff;
  ECT_TFLG2=0xff;
}



void INIT_MDC()     //��ʼ��ģ���ݼ�
{
  ECT_MCCTL=0xc7;
  ECT_MCCNT=50000;
  ECT_MCCTL_FLMC=1;
}
  


#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt void PTH_inter(void) 
{
   if(PIFH != 0) //�ж��жϱ�־    
   {
      PIFH = 0xff;//����жϱ�־     
      if(KEY1 == 0)         
      {
         time_jia();
      }
      if(KEY2 == 0) 
      {
          time+=1;
          if(time>10)
              time=10;
      }
      if(KEY3 == 0)
          direction=0;
      if(KEY4 == 0)
          direction=1;
   }
}
#pragma CODE_SEG DEFAULT




void clock1()//����ECT�ݼ�ʱ��Ϊ1s;
{
  Output_Compare();
  unsigned int i,j;
  for(i=0;i<32;i++){
    ECT_TC0=ECT_TCNT+31250;
  } 
}



void clock2()  // ����ECT�ݼ�ʱ��Ϊ1s;
{
  Input_Capture()
  unsigned int i,j;
  for(i=0;i<40;i++){
     INIT_MDC();
  } 
}




void delay()   //��ʱ����
 { 
    unsigned int i;
    for(i=0;i<5000;i++);
 }
 
 
void  time_jia()
{
  unsigned int i=0,j=0,k=0;
  clock1();
  if(ECT_TFLG1_C0F==1){
  i=i+1;
  CONT4=1;
  DATA=shuma[i];
  delay();
  if(i>9){
  j=j+1;
  i=0;
  CONT4=0;
  CONT3=1;
  DATA= shuma[j];
  delay();
  }
  if(j>6){
    j=0;
    if(k==2)break;// �������2����ʱֹͣ
    k=k+1;
    CONT4=0;
    CONT3=0;
    CONT2=1;
    DATA=shuma[k];
    delay();
  }
  if(k==1){
    LED=0x7f;
  }
  if(k==2){
    LED=0x3f;
  } 
 }
 
 
 
  
void time_jian()
{
  unsigned int i=10,j=6,k=2;
  clock2();
  if(ECT_TFLG1_C0F==1){
    i=i-1;
    j=j-1;
    k=k-1;
    CONT2=1;
    CONT3=0;
    CONT4=0;
    DATA=shuma[k];
    delay();
    CONT2=0;
    CONT3=1;
    CONT4=0;
    DATA=shuma[j];
    delay();
    CONT2=0;
    CONT3=0;
    CONT4=1;
    DATA=shuma[i];
 
    
  

  
void main()
{
DisableInterrupts;
  INIT_PLL();
  INIT_port();
  C0NT4=1;
  DATA=shuma[0];
 EnableInterrupts;
 for( ; ; ){
  
 }
  
    
    
  
 


