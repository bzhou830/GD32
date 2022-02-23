#ifndef __9235_H__
#define __9235_H__

#include "gd32l23x.h"
#include "systick.h"


#define WINDOW_XADDR_START	  0x50 // Horizontal Start Address Set-ˮƽ��ʼ��ַ����0x50
#define WINDOW_XADDR_END	  0x51 // Horizontal End Address Set---ˮƽ������ַ����0x51
#define WINDOW_YADDR_START	  0x52 // Vertical Start Address Set---��ֱ��ʼ��ַ����0x52
#define WINDOW_YADDR_END	  0x53 // Vertical End Address Set-----��ֱ������ַ����0x53
#define GRAM_XADDR		  	  0x20 // GRAM Horizontal Address Set--GRAMˮƽλ������0x20
#define GRAM_YADDR		  	  0x21 // GRAM Vertical Address Set----GRAN��ֱλ������0x21
#define GRAMWR 			      0x22 // memory write-----------------дGRAM

//Һ����ɫ����
#define White          0xFFFF//��ɫ
#define Black          0x0000//��ɫ
#define Blue           0x001F//��ɫ1
#define Blue2          0x051F//��ɫ2
#define Red            0xF800//��ɫ
#define Magenta        0xF81F//��ɫ
#define Green          0x07E0//��ɫ
#define Cyan           0x7FFF//��ɫ
#define Yellow         0xFFE0//��ɫ

//���ƶ˽ӿ�˵��
#define RS1         gpio_bit_set(GPIOA, GPIO_PIN_1);
#define RS0         gpio_bit_reset(GPIOA, GPIO_PIN_1);
#define RW1         gpio_bit_set(GPIOA, GPIO_PIN_2);
#define RW0         gpio_bit_reset(GPIOA, GPIO_PIN_2);
#define RD1         gpio_bit_set(GPIOA, GPIO_PIN_3);
#define RD0         gpio_bit_reset(GPIOA, GPIO_PIN_3);
#define CS1         gpio_bit_set(GPIOA, GPIO_PIN_4);
#define CS0         gpio_bit_reset(GPIOA, GPIO_PIN_4);
#define REST1       gpio_bit_set(GPIOA, GPIO_PIN_5);
#define REST0       gpio_bit_reset(GPIOA, GPIO_PIN_5);
//���ݿڽ�P4

#define  Gus_LCM_XMAX        240                                        		//����Һ��x��ĵ���
#define  Gus_LCM_YMAX        320                                        		//����Һ��y��ĵ���


#define DataBus(data)     gpio_port_write(GPIOB,(gpio_output_port_get(GPIOB)&0x00FF)|(data<<8))


//��ɫҺ���ĳ�ʼ��
void Init_ILI9325(void);           
//��ʼ��д������
void Init_data(unsigned char x, unsigned int y);
//д������
void Write_Cmd(unsigned char DH,unsigned char DL);
//д������
void Write_Data(unsigned char DH,unsigned char DL);
//�ڸ����ķ�Χ����ʾ��һRGB�����ɫ
void Show_RGB(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1,unsigned int Color);
//д��16λ����
void Write_Data_U16(unsigned int y); 
//�趨Һ����ʾ������
void LCD_SetPos(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1);
//�õ�һRGB��ɫ�������
void ClearScreen(unsigned int bColor);
//������
void DrawCoord(unsigned int x0,unsigned int x1, unsigned int y0, unsigned int y1, unsigned int LineWidth, unsigned int Color);
//��ʾֱ��
void Disline(unsigned int x1,unsigned int x2,unsigned int y1,unsigned int y2,unsigned int color);
//��ʾ��
void DisPoint(unsigned char x, unsigned int y, unsigned int color);
//��ʾ32*32ͼƬ����
void Dis32x32(unsigned int y,unsigned int x,unsigned char *Code32x32,unsigned int fColor,unsigned int bColor);
//��ʾ16*16ͼƬ����
void Dis16x16(unsigned char x,unsigned char y,unsigned char *Code16x16,unsigned int fColor,unsigned int bColor);
//��ʾ16*32ͼƬ��ASC�ַ�
void Dis16x32(unsigned int y, unsigned int x,unsigned char *Code16x32,unsigned int fColor,unsigned int bColor);
//��ʾ16*8�ַ�
void Dis16x8(unsigned int y, unsigned int x,unsigned char *Code16x32,unsigned int fColor,unsigned int bColor);
//��Բ
void LcdDrawCircle(unsigned char cx,unsigned char cy,unsigned char r);
//u8 ShowOneASCII(u8 pcStr,unsigned int x, unsigned int y,unsigned int fColor);
//void writeString(u8 *pcStr,u16 x0,u16 y0,u16 color);

#endif

