#include "nes_main.h"
#include "9235.h"
#include "JoyPad.h"
extern JoyPadType JoyPad[2];

uint8 Continue = TRUE;					//��ʼ��Ϊ��
int FrameCnt;


//NES ֡����ѭ��   
void NesFrameCycle(void)
{
 	int	clocks;								//CPUִ��ʱ��
// ����ģ����ѭ�������VROM���ã�������Ϊ0����ʼ��VROM
//	if ( NesHeader.byVRomSize == 0)
//		����VROM�洢��λ�á�
	FrameCnt = 0;
	while(Continue)
	{
		/* scanline: 0~19 VBANK �Σ���PPUʹ��NMI��������NMI �ж�, */
		FrameCnt++;		   //֡������
		SpriteHitFlag = FALSE;		
		for(PPU_scanline=0; PPU_scanline<20; PPU_scanline++)
		{ 
			exec6502(CLOCKS_PER_SCANLINE);
//			NesHBCycle();
		}
		/* scanline: 20, PPU�������ã�ִ��һ�οյ�ɨ��ʱ��*/
		exec6502(CLOCKS_PER_SCANLINE);
//		NesHBCycle();  //ˮƽ����
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
			{					//��Ϊ�٣��ر���ʾ
				if(SpriteHitFlag == FALSE)
					NES_GetSpr0HitFlag(PPU_scanline - SCAN_LINE_DISPALY_START_NUM);						//����Sprite #0 ��ײ��־
			}
			if(FrameCnt%5 == 0)
			{										 //ÿ2֡��ʾһ��
				NES_RenderLine(PPU_scanline - SCAN_LINE_DISPALY_START_NUM);					//ˮƽͬ������ʾһ��
			}	
		}
		/* scanline: 262 ���һ֡*/
		exec6502(CLOCKS_PER_SCANLINE);
		PPU_Reg.R2 |= R2_VBlank_Flag;	//����VBANK ��־
		/*��ʹ��PPU VBANK�жϣ�������VBANK*/
		if(PPU_Reg.R0 & R0_VB_NMI_EN)
		{
			NMI_Flag = SET1;	//���һ֡ɨ�裬����NMI�ж�
		}

	   	/*����֡IRQ��־��ͬ����������APU��*/
		
		/* A mapper function in V-Sync �洢���л���ֱVBANKͬ��*/
//		MapperVSync();
		
		/*��ȡ������JoyPad״̬,����JoyPad������ֵ*/
		NES_JoyPadUpdateValue();	 //systick �ж϶�ȡ����ֵ
		//JoyPad[0].value = 0x07;
		/*����ѭ����־���Ƿ��˳�ѭ��*/
//		if(){
//		 	Continue = FALSE;
//		}
	}
}


void nes_main(void)
{	
	NesHeader *neshreader = (NesHeader *) rom_file;

/* 	����NES�ļ�ͷ������0x1A��������0x1a��Ctrl+Z,����ģ���ļ������ķ��ţ�
 *	����ʹ��strcncmp�Ƚ�ǰ3���ֽڡ�
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
	
	//��ʼ��nes ģ����
	init6502mem( 0,									//exp_rom*/
				 0,									//sram �ɿ����;���, �ݲ�֧��*/
				(&rom_file[0x10]),					//prg_rombank, �洢����С �ɿ����;���*/
			     neshreader->romnum 	
				);  								//��ʼ��6502�洢������
	reset6502();
	PPU_Init((&rom_file[0x10] + (neshreader->romnum * 0x4000)), (neshreader->romfeature & 0x01));	/*PPU_��ʼ��*/	
	NES_JoyPadInit();
	//nes ģ��������ѭ��
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
//		case 0x0780:   writeString("4",0,110,White); ; break; //��
//		case 0x0740:   writeString("5",0,110,White);JoyPad[0].value = 0x05; break;//��
//		case 0x06c0:   writeString("6",0,110,White);JoyPad[0].value = 0x06; break;//��
//		case 0x05c0:   writeString("7",0,110,White);JoyPad[0].value = 0x07; break;//��
//		case 0x03c0:   writeString("3",0,110,White);JoyPad[0].value = 0x03; break;//��
//	}
//	//sprintf();
//	writeString("NO",0,110,White);
}




