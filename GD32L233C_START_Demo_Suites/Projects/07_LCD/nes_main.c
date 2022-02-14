#include "nes_main.h"
#include "9235.h"
#include "JoyPad.h"
extern JoyPadType JoyPad[2];

uint8 Continue = TRUE;					//初始化为真
int FrameCnt;


//NES 帧周期循环   
void NesFrameCycle(void)
{
 	int	clocks;								//CPU执行时间
// 启动模拟器循环，检查VROM设置，若卡带为0，初始化VROM
//	if ( NesHeader.byVRomSize == 0)
//		设置VROM存储区位置。
	FrameCnt = 0;
	while(Continue)
	{
		/* scanline: 0~19 VBANK 段，若PPU使能NMI，将产生NMI 中断, */
		FrameCnt++;		   //帧计数器
		SpriteHitFlag = FALSE;		
		for(PPU_scanline=0; PPU_scanline<20; PPU_scanline++)
		{ 
			exec6502(CLOCKS_PER_SCANLINE);
//			NesHBCycle();
		}
		/* scanline: 20, PPU加载设置，执行一次空的扫描时间*/
		exec6502(CLOCKS_PER_SCANLINE);
//		NesHBCycle();  //水平周期
		PPU_scanline++;	  //20++
		PPU_Reg.R2 &= ~R2_SPR0_HIT;
		/* scanline: 21~261*/	
		for(; PPU_scanline < SCAN_LINE_DISPALY_END_NUM; PPU_scanline++)
		{
			if((SpriteHitFlag == TRUE) && ((PPU_Reg.R2 & R2_SPR0_HIT) == 0))
			{
				clocks = sprite[0].x * CLOCKS_PER_SCANLINE / NES_DISP_WIDTH;
				exec6502(clocks);
				PPU_Reg.R2 |= R2_SPR0_HIT;
				exec6502(CLOCKS_PER_SCANLINE - clocks);
			}
			else
			{
			    exec6502(CLOCKS_PER_SCANLINE);
			}

			if(PPU_Reg.R1 & (R1_BG_VISIBLE | R1_SPR_VISIBLE))
			{					//若为假，关闭显示
				if(SpriteHitFlag == FALSE)
					NES_GetSpr0HitFlag(PPU_scanline - SCAN_LINE_DISPALY_START_NUM);						//查找Sprite #0 碰撞标志
			}
			if(FrameCnt%5 == 0)
			{										 //每2帧显示一次
				NES_RenderLine(PPU_scanline - SCAN_LINE_DISPALY_START_NUM);					//水平同步与显示一行
			}	
		}
		/* scanline: 262 完成一帧*/
		exec6502(CLOCKS_PER_SCANLINE);
		PPU_Reg.R2 |= R2_VBlank_Flag;	//设置VBANK 标志
		/*若使能PPU VBANK中断，则设置VBANK*/
		if(PPU_Reg.R0 & R0_VB_NMI_EN)
		{
			NMI_Flag = SET1;	//完成一帧扫描，产生NMI中断
		}

	   	/*设置帧IRQ标志，同步计数器，APU等*/
		
		/* A mapper function in V-Sync 存储器切换垂直VBANK同步*/
//		MapperVSync();
		
		/*读取控制器JoyPad状态,更新JoyPad控制器值*/
		NES_JoyPadUpdateValue();	 //systick 中断读取按键值
		//JoyPad[0].value = 0x07;
		/*设置循环标志，是否退出循环*/
//		if(){
//		 	Continue = FALSE;
//		}
	}
}


void nes_main(void)
{	
	NesHeader *neshreader = (NesHeader *) rom_file;

/* 	由于NES文件头类型以0x1A结束，（0x1a是Ctrl+Z,这是模拟文件结束的符号）
 *	所以使用strcncmp比较前3个字节。
 */
	if(strncmp(neshreader->filetype, "NES", 3) != 0)
	{
		//writeString("file not found!\n",0,10,White);
		return;
	}
	else
	{
		//writeString("file add OK !",0,30,White);
		if((neshreader->romfeature & 0x01) == 0)
		{
			//writeString("shuipingjingxiang",0,50,White);
		}
		else
		{
	 		//writeString("chuizhijingxiang",0,50,White);
		}

		if((neshreader->romfeature & 0x02) == 0)
		{
			//writeString("wu ji yi!",0,70,White);
		}
		else
		{
	 		//writeString("you ji yi!",0,70,White);
		}

		if((neshreader->romfeature & 0x04) == 0)
		{
			//writeString("wu 512 de trainer",0,90,White);
		}
		else
		{
	 		//writeString("you 512 de trainer",0,90,White);
		}

		if((neshreader->romfeature & 0x08) == 0)
		{
			//writeString("2--VRAM",0,110,White);
		}
		else
		{
	 		//writeString("4--VRAM",0,110,White);
		}

	}
	
	//初始化nes 模拟器
	init6502mem( 0,									//exp_rom*/
				 0,									//sram 由卡类型决定, 暂不支持*/
				(&rom_file[0x10]),					//prg_rombank, 存储器大小 由卡类型决定*/
			     neshreader->romnum 	
				);  								//初始化6502存储器镜像
	reset6502();
	PPU_Init((&rom_file[0x10] + (neshreader->romnum * 0x4000)), (neshreader->romfeature & 0x01));	/*PPU_初始化*/	
	NES_JoyPadInit();
	//nes 模拟器运行循环
	NesFrameCycle();
}


void NES_JoyPadUpdateValue(void)
{
	//u16 Value;
	//Value = GPIO_ReadInputData(GPIOC);
	//Value &= 0x07c0;
	/*
	if(GPIOCin(6) ==  0)
	{
		JoyPad[0].value = 1 << 20;
		JoyPad[0].value |= 0x10;
	}
	if(GPIOCin(7) ==  0)
	{
		JoyPad[0].value = 1 << 20;
		JoyPad[0].value |= 0x20;
	}
	if(GPIOCin(8) ==  0)
	{
		JoyPad[0].value = 1 << 20;
		JoyPad[0].value |= 0x40;
	
	}
	if(GPIOCin(9) ==  0)
	{
		JoyPad[0].value = 1 << 20;
		JoyPad[0].value |= 0x80;
	}
	if(GPIOCin(10) ==  0)
	{
		JoyPad[0].value = 1 << 20;
		JoyPad[0].value |= 0x01;
	}
	*/
//	switch(Value)
//	{
//		case 0x0780:   writeString("4",0,110,White); ; break; //上
//		case 0x0740:   writeString("5",0,110,White);JoyPad[0].value = 0x05; break;//下
//		case 0x06c0:   writeString("6",0,110,White);JoyPad[0].value = 0x06; break;//下
//		case 0x05c0:   writeString("7",0,110,White);JoyPad[0].value = 0x07; break;//下
//		case 0x03c0:   writeString("3",0,110,White);JoyPad[0].value = 0x03; break;//中
//	}
//	//sprintf();
//	writeString("NO",0,110,White);
}




