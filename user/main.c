#include "includes.h"
void RCC_Configuration(void);
//void NVIC_Configuration(void);
void  USART_Configuration(void);
void PrintCom(USART_TypeDef* USARTx, uint8_t *Data);
extern void  LD3320_main(void);
/*********************主函数***************************/
int main(void)
{
  RCC_Configuration();  //外设时钟配置
  USART_Configuration();//USART 配置	
	LD3320_main();				//LD3320执行函数
	while (1)
  {	
	}
}
/********************串口配置****************************/
void  USART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);	//类似与GPIO口，配置完后初始化一次
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //使能接收 中断 
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  //使能发送 中断 
  USART_Cmd(USART1, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         //USART1 TX
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    //A端口 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         //USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //复用开漏输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         //A端口 
}
/********************串口数据打印****************************/
void PrintCom(USART_TypeDef* USARTx, uint8_t *Data)
{ 
	
		while(*Data)
		{
			USART_SendData(USARTx, *Data++);    /*发送单个数据 */
			while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);/* 检查指定的USART标志位即RESET＝1时发送完成*/
		}													
}
/**********************配置外设时钟**************************/
void RCC_Configuration(void)
{
  SystemInit();  
  /* Enable USART1, GPIOA, GPIOx and AFIO clocks */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE); //开启相关的AHP外设时钟
  /* Enable USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}



