/********************开发板测试、演示****************************
#include "includes.h"
/**********************延时n毫秒*************************************/
void Delayms(uint16 i)
{
	unsigned char a,b;
	for(;i>0;i--)
	        for(b=4;b>0;b--)
	            for(a=113;a>0;a--);	

}
/***********************实现流水灯功能************************************/
void Glide_LED(void)
{
	GPIO_SetBits(GPIOC,GPIO_Pin_3);
	Delayms(0xfff);
	GPIO_SetBits(GPIOC,GPIO_Pin_2);
	Delayms(0xfff);
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);
	Delayms(0xfff);
	GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	Delayms(0xfff);
}
/*************************实现灯闪烁功能**********************************/
void Flicker_LED(void)
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);
	GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	Delayms(0XFFF);
	GPIO_SetBits(GPIOC,GPIO_Pin_3);
	GPIO_SetBits(GPIOC,GPIO_Pin_2);
	Delayms(0XFFF);
}
/*********************按键触发LED显示(独立按键点亮独立LED)******************/
void Key_LED(void)
{

}
/************************关闭全部LED***********************************/
void Off_LED(void)
{
	GPIO_SetBits(GPIOC,GPIO_Pin_3);
	GPIO_SetBits(GPIOC,GPIO_Pin_2);
}

/**************************开发板测试程序****************************/
void Board_text(uint8 Code_Val)
{																					 
	 switch(Code_Val)		   /*对结果执行相关操作,客户修改*/
			  {
				  case CODE_LSD:			/*命令“流水灯”*/
							 					Glide_LED();
													 break;
					case CODE_SS:	 /*命令“闪烁”*/
												Flicker_LED();
													 break;
					case CODE_AJCF:		/*命令“按键触发”*/
												Key_LED();
													break;
					case CODE_QM:		/*命令“全灭”*/
												Off_LED();
													break;
					default:break;
				}	
}
