#ifndef __9235_H__
#define __9235_H__

#include "gd32l23x.h"
#include "systick.h"


#define WINDOW_XADDR_START	  0x50 // Horizontal Start Address Set-水平开始地址设置0x50
#define WINDOW_XADDR_END	  0x51 // Horizontal End Address Set---水平结束地址设置0x51
#define WINDOW_YADDR_START	  0x52 // Vertical Start Address Set---垂直开始地址设置0x52
#define WINDOW_YADDR_END	  0x53 // Vertical End Address Set-----垂直结束地址设置0x53
#define GRAM_XADDR		  	  0x20 // GRAM Horizontal Address Set--GRAM水平位置设置0x20
#define GRAM_YADDR		  	  0x21 // GRAM Vertical Address Set----GRAN垂直位置设置0x21
#define GRAMWR 			      0x22 // memory write-----------------写GRAM

//液晶颜色声明
#define White          0xFFFF//白色
#define Black          0x0000//黑色
#define Blue           0x001F//蓝色1
#define Blue2          0x051F//蓝色2
#define Red            0xF800//红色
#define Magenta        0xF81F//紫色
#define Green          0x07E0//绿色
#define Cyan           0x7FFF//灰色
#define Yellow         0xFFE0//黄色

//控制端接口说明
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
//数据口接P4

#define  Gus_LCM_XMAX        240                                        		//定义液晶x轴的点数
#define  Gus_LCM_YMAX        320                                        		//定义液晶y轴的点数


#define DataBus(data)     gpio_port_write(GPIOB,(gpio_output_port_get(GPIOB)&0x00FF)|(data<<8))


//彩色液晶的初始化
void Init_ILI9325(void);           
//初始化写入数据
void Init_data(unsigned char x, unsigned int y);
//写入命令
void Write_Cmd(unsigned char DH,unsigned char DL);
//写入数据
void Write_Data(unsigned char DH,unsigned char DL);
//在给定的范围内显示单一RGB组合颜色
void Show_RGB(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1,unsigned int Color);
//写入16位数据
void Write_Data_U16(unsigned int y); 
//设定液晶显示的坐标
void LCD_SetPos(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1);
//用单一RGB颜色填充整屏
void ClearScreen(unsigned int bColor);
//画坐标
void DrawCoord(unsigned int x0,unsigned int x1, unsigned int y0, unsigned int y1, unsigned int LineWidth, unsigned int Color);
//显示直线
void Disline(unsigned int x1,unsigned int x2,unsigned int y1,unsigned int y2,unsigned int color);
//显示点
void DisPoint(unsigned char x, unsigned int y, unsigned int color);
//显示32*32图片或汉字
void Dis32x32(unsigned int y,unsigned int x,unsigned char *Code32x32,unsigned int fColor,unsigned int bColor);
//显示16*16图片或汉字
void Dis16x16(unsigned char x,unsigned char y,unsigned char *Code16x16,unsigned int fColor,unsigned int bColor);
//显示16*32图片或ASC字符
void Dis16x32(unsigned int y, unsigned int x,unsigned char *Code16x32,unsigned int fColor,unsigned int bColor);
//显示16*8字符
void Dis16x8(unsigned int y, unsigned int x,unsigned char *Code16x32,unsigned int fColor,unsigned int bColor);
//画圆
void LcdDrawCircle(unsigned char cx,unsigned char cy,unsigned char r);
//u8 ShowOneASCII(u8 pcStr,unsigned int x, unsigned int y,unsigned int fColor);
//void writeString(u8 *pcStr,u16 x0,u16 y0,u16 color);

#endif

