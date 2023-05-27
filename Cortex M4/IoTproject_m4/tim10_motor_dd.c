// �� ����
// PB8  : DC Motor M_EN
// PB9  : DC Motor M_D2
// PB10 : DC Motor M_D1
#include "stm32f4xx.h"
#include "tim10_motor.h"

void TIM10_Motor_Init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);  
  
  //pwm
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;     // TIM10_CH1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM10);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;     // motor dir
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
  TIM_TimeBaseStructure.TIM_Prescaler = 168-1;         //168Mhz/168 = 1MHz(1us)
  TIM_TimeBaseStructure.TIM_Period = 17700-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 8850-1;         //50% duty
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM10, &TIM_OCInitStructure);

  //Ÿ�̸�10�� ���۽�Ų��.
  TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM10, ENABLE);
  TIM_Cmd(TIM10, ENABLE);

}