#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "uart4.h"
#include "uart2.h"
#include "lcd.h"
#include "tim10_motor.h"
#include "led.h"

/******************************************************************************
* General-purpose timers TIM10
포트 연결:

PA0,PA1 : UART4 RX,TX  Bluetooth 
PA2,PA3 : UART2 RX,TX  Debug
PA5 ~ PA12 : CLCD D0~D7
PB0 ~ PB2  : CLCD RS, RW, E

PB8  : Servo Motor SERVO_PIN
PB9  : DC Motor M_D2
PB10 : DC Motor M_D1
******************************************************************************/
#define CMD_SIZE 50
#define ARR_CNT 5  

extern volatile unsigned char rx2Flag;
extern char rx2Data[50];
extern volatile unsigned char rx4Flag;
extern char rx4Data[50];
int t_cnt = 0;
int AngleCount = 0;
int timerIsrFlag =0;
int LEDFlag = 0;
long map(long x, long in_min, long in_max, long out_min, long out_max);

extern volatile unsigned long systick_sec;            //1sec
extern volatile unsigned long systick_count;
extern volatile int systick_secFlag;


void Motor_Start();
void Motor_Stop();
void LED();


int main()
{
  
  UART2_init();
  UART4_init();
  TIM10_Motor_Init();
  PORTC_Led_Init();
  
  GPIOAB_LCD_Init();
  lcd(0,0,"Drive safely");    // 문자열 출력
  lcd(0,1,"");
  
  Serial2_Send_String("start main()\r\n");
  
  while(1)
  {    
    if(rx2Flag)
    {
      printf("rx2Data : %s\r\n",rx2Data);
      
      rx2Flag = 0;
    }  
    if(rx4Flag)
    {
      Serial4_Event();
      rx4Flag = 0;
    }
    LED();
  }
}

void Serial4_Event()
{  
  int i=0;
  char * pToken;
  char * pArray[ARR_CNT]={0};
  char recvBuf[CMD_SIZE]={0};
  strcpy(recvBuf,rx4Data);
  i=0; 
  //  Serial2_Send_String(recvBuf);
  //  Serial2_Send_String("\n\r");
  printf("rx4Data : %s\r\n",recvBuf);
  
  pToken = strtok(recvBuf,"[@]");
  
  
  while(pToken != NULL)
  {
    pArray[i] =  pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[@]");
  }
  
  printf("pArray[1] : %s\r\n", pArray[1]); 
  
  if(!strcmp(pArray[1],"ON"))
  {
    TIM10->CCR1 = 700; // 서보모터 아래로 작동   창문열기 , 진동 센서 작동
    GPIOAB_LCD_Init();
    lcd(0,0,"Take a break");    // 문자열 출력
    lcd(0,1,"Place : ");
    Motor_Start();
  }
  else if(!strcmp(pArray[1], "OFF"))
  {
    TIM10->CCR1 = 1500; // 서보모터 복귀         창문닫기, 진동 센서 중지
    GPIOAB_LCD_Init();
    lcd(0,0,"Navigation");    // 문자열 출력
    lcd(0,1,"Safe");
    Motor_Stop();
    LEDFlag = 0;
  }
  else if(!strcmp(pArray[1], "TA"))
  {
    TIM10->CCR1 = 700; // 서보모터 복귀         창문닫기, 진동 센서 중지
    GPIOAB_LCD_Init();
    lcd(0,0,"Traffic");    // 문자열 출력
    lcd(0,1,"Accident");
    Motor_Stop();
    LEDFlag = 1;
  }   
}

void LED()
{
  int temp = systick_sec%2;
  if(LEDFlag)
  {
    if(temp == 1)
      GPIO_Write(GPIOC,0x55);
    else
      GPIO_Write(GPIOC,0xAA); 
  }
  else
    GPIO_Write(GPIOC,0x00); 
}

void Motor_Start()
{
  GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_SET);
  GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_SET); 
  GPIO_WriteBit(GPIOB,GPIO_Pin_11,Bit_RESET); 
}

void Motor_Stop()
{
  GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_RESET);
  GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_RESET);  
  GPIO_WriteBit(GPIOB,GPIO_Pin_11,Bit_RESET); 
}
