/******************************语音模块底层驱动*****************/
#include "includes.h"
uint8 nLD_Mode = LD_MODE_IDLE;		//	用来记录当前是在进行ASR识别还是在播放MP3
uint8 ucRegVal;
extern uint8  nAsrStatus;
/********************LD芯片硬件初始化**************************/
void LD_reset(void)
{
	LD_RST_H();
	LD3320_delay(100);
	LD_RST_L();
	LD3320_delay(100);
	LD_RST_H();
	LD3320_delay(100);
	LD_CS_L();
	LD3320_delay(100);
	LD_CS_H();		
	LD3320_delay(100);
}
/**********************初始化命令**************************/
void LD_Init_Common(void)
{

	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); 
	LD3320_delay(5);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  
	LD3320_delay(5);
	LD_WriteReg(0xCF, 0x43);   
	LD3320_delay(5);
	LD_WriteReg(0xCB, 0x02);
	LD_WriteReg(0x11, LD_PLL_11);       
	if (nLD_Mode == LD_MODE_MP3)
	{
		LD_WriteReg(0x1E, 0x00); 
		LD_WriteReg(0x19, LD_PLL_MP3_19);   
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);   
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD_WriteReg(0x1E,0x00);
		LD_WriteReg(0x19, LD_PLL_ASR_19); 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);		
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);
	}
	LD3320_delay(5);
	
	LD_WriteReg(0xCD, 0x04);
	LD_WriteReg(0x17, 0x4c); 
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);
	LD_WriteReg(0xCF, 0x4F); 
	LD_WriteReg(0x6F, 0xFF); 
}
/*********************初始化ASR	***************************/
void LD_Init_ASR(void)
{
	nLD_Mode=LD_MODE_ASR_RUN;
	LD_Init_Common();

	LD_WriteReg(0xBD, 0x00);
	LD_WriteReg(0x17, 0x48);	
	LD3320_delay( 5 );
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
	LD_WriteReg(0x40, 0);          
	LD_WriteReg(0x42, 8);
	LD_WriteReg(0x44, 0);    
	LD_WriteReg(0x46, 8); 
	LD3320_delay( 1 );
}
/*********************识别处理函数(可以利用查询方式，或中断方式进行调用)***************************/
void ProcessInt0(void)
{
	uint8 nAsrResCount=0;

	ucRegVal = LD_ReadReg(0x2B);
		// 语音识别产生的中断
			// （有声音输入，不论识别成功或失败都有中断）
			LD_WriteReg(0x29,0) ;
			LD_WriteReg(0x02,0) ;
	
			if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
				{	 
						nAsrResCount = LD_ReadReg(0xba);
			
						if(nAsrResCount>0 && nAsrResCount<=4) 
						{
							nAsrStatus=LD_ASR_FOUNDOK; 				
						}
						else
					    {
							nAsrStatus=LD_ASR_FOUNDZERO;
							}	
				}
			else
			 {
					nAsrStatus=LD_ASR_FOUNDZERO;	//执行没有识别
				}

		  LD_WriteReg(0x2b, 0);
		  LD_WriteReg(0x1C,0);/*写0:ADC不可用*/
		
			LD_WriteReg(0x29,0) ;
			LD_WriteReg(0x02,0) ;
			LD_WriteReg(0x2B,  0);
			LD_WriteReg(0xBA, 0);	
			LD_WriteReg(0xBC,0);	
			LD_WriteReg(0x08,1);	 /*清除FIFO_DATA*/
			LD_WriteReg(0x08,0);	/*清除FIFO_DATA后 再次写0*/
			//LD3320_delay(1);
}
/***************检测 ASR 是否忙状态(flag ---1：空闲状态)*********************************/
uint8 LD_Check_ASRBusyFlag_b2(void)
{
	uint8 j;
	uint8 flag = 0;
	for (j=0; j<10; j++)
	{
		if (LD_ReadReg(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		LD3320_delay(10);		
	}
	return flag;
}
/*********************ASR初始化***************************/
void LD_AsrStart(void)
{
	LD_Init_ASR();
}
/*******************ASR执行函数*****************************/
uint8 LD_AsrRun(void)
{
	LD_WriteReg(0x35, MIC_VOL);
	LD_WriteReg(0x1C, 0x09);
	LD_WriteReg(0xBD, 0x20);
	LD_WriteReg(0x08, 0x01);
	LD3320_delay( 5 );
	LD_WriteReg(0x08, 0x00);
	LD3320_delay( 5);

	if(LD_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	
	LD_WriteReg(0x37, 0x06);
	LD_WriteReg(0x37, 0x06);
	LD3320_delay( 5);
	LD_WriteReg(0x1C, 0x0b);
	LD_WriteReg(0x29, 0x10);
	
	LD_WriteReg(0xBD, 0x00);   
	return 1;
}

/***************************添加识别关键词语*****************/
uint8 LD_AsrAddFixed(void)
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 4    /*数组二维数值*/
	#define DATE_B 20		/*数组一维数值*/
	 uint8  sRecog[DATE_A][DATE_B] = {
	 																			"liu shui deng",\
																				"shan shuo",\
																				"an jian chu fa",\
																				 "quan mie"\
																				};	/*添加关键词*/
	 uint8  pCode[DATE_A] = {
	 															CODE_LSD,\
																CODE_SS,\
																 CODE_AJCF,\
																  CODE_QM\
															};	/*添加识别码*/
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD_WriteReg(0xc1, pCode[k] );
		LD_WriteReg(0xc3, 0 );
		LD_WriteReg(0x08, 0x04);
		LD3320_delay(1);
		LD_WriteReg(0x08, 0x00);
		LD3320_delay(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD_WriteReg(0xb9, nAsrAddLength);
		LD_WriteReg(0xb2, 0xff);
		LD_WriteReg(0x37, 0x04);
//		LD_WriteReg(0x37, 0x04);
	}	 

    return flag;
}
/*********************获取识别结果(返回识别码 LD_ReadReg(0xc5 );)***************************/
uint8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5 );
}



