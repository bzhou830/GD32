typedef    unsigned int     uint16;  
typedef    unsigned int     UINT;  
typedef    unsigned int     uint;  
typedef    unsigned int     UINT16;  
typedef    unsigned int     WORD;  
typedef    unsigned int     word; 
typedef    int              int16;  
typedef    int              INT16;  
typedef    unsigned long    uint32;  
typedef    unsigned long    UINT32;  
typedef    unsigned long    DWORD;  
typedef    unsigned long    dword;  
typedef    long             int32;  
typedef    long             INT32;  
typedef    signed  char     int8; 
typedef    signed  char     INT8;  
typedef    unsigned char    byte;  
typedef    unsigned char    BYTE;  
typedef    unsigned char    uchar; 
typedef    unsigned char    UINT8;  
typedef    unsigned char    uint8; 
typedef    unsigned char    BOOL; 
#include "9235.h"
//#include "hz16.h"



/*******************************************************************************
文件中坐标的设定：（统一按照竖屏）
 
              (239,319) -----------------(0,319)
                       |                 |
                       |                 |
                       |                 |
                       |                 |
                       |                 |
                       |                 |
                       |                 |
                       |                 |
                       |                 |
                       |                 |
               (239,0) <-----------------(0,0)
********************************************************************************/
uint16 bColor = Black;



/*******************************************************************************
函数名称：void ClearScreen(uint16 Color)
函数功能：刷屏使得全屏显示单一的颜色
入口参数：Clolr:屏幕背景的颜色（直接写入颜色的英文，在.h中有定义）
********************************************************************************/
void ClearScreen(uint16 Color)
{
  uint16 i,j;
  LCD_SetPos(0,320,0,240);                                  //设置起始地址为（0，0） ~ (320,240)
  for (i = 0;i < 325;i++)
  {
    for (j = 0;j < 240;j++)
    {
      Write_Data_U16(Color);
    }
  }
}


/*******************************************************************************
函数名称：void Show_RGB (uint16 x0,uint16 x1,uint16 y0,uint16 y1,uint16 Color)
函数功能：在屏幕的（x0,y0）到（x1,y1）区域显示某一种颜色
入口参数：（x0,y0）和（x1,y1）表示位置；
           Color :显示区域的颜色
备    注：可以实现画点和画水平和垂直的直线
          显示时输入的参数x：0-240，y:0-320（纵向）
*******************************************************************************/
void Show_RGB (uint16 x0,uint16 x1,uint16 y0,uint16 y1,uint16 Color)
{
 
  uint16 i,j;
  LCD_SetPos(x0,x1,y0,y1); 
  for (i = y0;i <= y1;i++)
  { 
    for (j = x0;j <= x1;j++)
    {
      Write_Data_U16(Color);
    }
  }
}


/******************************************************************** 
画直线（可以画任意方向直线，包括横线、竖线、斜线）。
入口参数： (x1,y1)起点, (x2,y2)终点, color颜色。
出口参数: 无
说    明：用指定的颜色在指定的两点间画出一条直线。
***********************************************************************/
void Disline(uint x1,uint x2,uint y1,uint y2,uint color)  
{  
  uint16 t;  
  int xerr=0,yerr=0,delta_x,delta_y,distance;  
  int incx,incy;  
  uint16 row,col;  
  delta_x = x2-x1;                            //计算坐标增量  
  delta_y = y2-y1;  
  col = x1;  
  row = y1;  
  if(delta_x>0) incx=1;                       //设置单步方向  
  else   
  {  
    if(delta_x == 0) incx = 0;                //垂直线  
    else {incx=-1;delta_x=-delta_x;}  
  }  
  if(delta_y > 0)incy = 1;  
  else  
  {  
    if(delta_y == 0) incy = 0;                //水平线  
    else {incy = -1;delta_y = -delta_y;}  
  }  
  if(delta_x>delta_y) distance=delta_x;       //选取基本增量坐标轴  
  else distance=delta_y;  
  
  for(t=0;t<=distance+1;t++)  
  {                                           //画线输出  
    LCD_SetPos(col,col,row,row);
    Write_Data_U16(color);
    xerr+=delta_x;  
    yerr+=delta_y;  
    if(xerr>distance)  
    {  
      xerr-=distance;  
      col+=incx;  
    }  
    if(yerr>distance)  
    {  
      yerr-=distance;  
      row+=incy;  
    }  
  }  
}


/***************************************************
函数名称：LcdDrawCircle(uint8 cx,uint8 cy,uint8 r)
函数功能：画圆
输入参数：圆心和半径
输出参数：
备    注：
****************************************************/
void LcdDrawCircle(uint8 cx,uint8 cy,uint8 r)
{
	uint8 x,y;
	int8 d;
	x = 0;
	y = r;
	d = 3 - 2 * r;
	while(x <= y)
	{
		LCD_SetPos(cx + x,cx + x,cy + y,cy + y);
        Write_Data_U16(Red);
        
        LCD_SetPos(cx - x,cx - x,cy + y,cy + y);
        Write_Data_U16(Red);
        
        LCD_SetPos(cx + x,cx + x,cy - y,cy - y);
        Write_Data_U16(Red);
        
        LCD_SetPos(cx - x,cx - x,cy - y,cy - y);
        Write_Data_U16(Red);

        LCD_SetPos(cx + y,cx + y,cy + x,cy + x);
        Write_Data_U16(Red);
        
        LCD_SetPos(cx - y,cx - y,cy + x,cy + x);
        Write_Data_U16(Red);
        
        LCD_SetPos(cx + y,cx + y,cy - x,cy - x);
        Write_Data_U16(Red);
        
        LCD_SetPos(cx - y,cx - y,cy - x,cy - x);
        Write_Data_U16(Red);
              
		if(d < 0)
		{
			d += 4 * x + 5;
			x++;
		}
		else
		{
			d += 4 * (x - y) + 3;
			y--;
			x++;
		}		
	}
}


/*******************************************************************************
函数名称：void DrawCoord(uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
函数功能：在屏幕上画坐标，屏幕是横放的
入口参数：（x0,y0）和（x1,y1）表示位置；
           Color :显示区域的颜色
           Line_Width:坐标轴的线宽
*******************************************************************************/
void DrawCoord(uint16 x0,uint16 x1,uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
{
  LineWidth = LineWidth-1;
  Show_RGB (x0,x0 + LineWidth,y0,y1,Color);  //竖
  Show_RGB (x0,x1,y1 - LineWidth,y1,Color);  //横
}



/*******************************************************************************
函数名称：void Show_Xmark(uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
函数功能：在屏幕上画横线，屏幕是横放的
入口参数：（x0,y0）和（x1,y1）表示位置；
           Color :显示区域的颜色
           Line_Width:坐标轴的线宽
*******************************************************************************/
void Show_Xmark(uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
{
  LineWidth = LineWidth-1;
  Show_RGB(x0,x0 + LineWidth,y0,y1,Color);  //竖
}


/*******************************************************************************
函数名称：void Show_Xmark(uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
函数功能：在屏幕上画竖线，屏幕是横放的
入口参数：（x0,y0）和（x1,y1）表示位置；
           Color :显示区域的颜色
           Line_Width:坐标轴的线宽
*******************************************************************************/
void Show_Ymark (uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
{
  LineWidth = LineWidth - 1;
  Show_RGB(x0,x1,y0,y0 + LineWidth,Color);
}



/*******************************************************************************
函数名称：void  Init_data (uint8 x,uint16 y)
函数功能：些数据到寄存器
入口参数：x,y 需要写入的数据
*******************************************************************************/
void  Init_data (uint8 x,uint16 y)
{
  uint8 m,n;
  m = y >> 8;
  n = y;
  Write_Cmd(0x00,x);
  Write_Data(m,n);
}


/*******************************************************************************
函数名称：void  Write_Data_U16(uint16 y)
函数功能：写入颜色
入口参数：y 颜色数据
*******************************************************************************/
void  Write_Data_U16(uint16 y)
{
  uint8 m,n;
  m = y >> 8;
  n = y;
  Write_Data(m,n);
}


/*******************************************************************************
函数名称：void Write_Cmd(uint8 DH,uint8 DL)
函数功能：写指令
入口参数：DH 高8位数据  DL  低8位数据
*******************************************************************************/
void Write_Cmd(uint8 DH,uint8 DL)
{
  CS0
  RS0
  DataBus(DH);
  RW0
  //delay_us(2);  
  RW1
  DataBus(DL);
  RW0
  //delay_us(2); 
  RW1
  CS1
}


/*******************************************************************************
函数名称：void Write_Data(uint8 DH,uint8 DL)
函数功能：写数据
入口参数：DH 高8位数据  DL  低8位数据
*******************************************************************************/
void Write_Data(uint8 DH,uint8 DL)
{
  CS0
  //delay_us(2); 
  RS1
  DataBus(DH);
  RW0
  //delay_us(2); 
  RW1
  DataBus(DL);
  RW0
  //delay_us(2);  
  RW1
  CS1
}




/*******************************************************************************
函数名称：void Init_ILI9325()
函数功能：初始化液晶模块
*******************************************************************************/
void Init_ILI9325()
{ 
	/*
	GPIO_InitTypeDef  GPIO_InitStructure;				   //定义配置结构体
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);  //打开端口D时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;			   //选中端口0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		   //输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		   //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //端口速度
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		   //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);
  */
	RD1  
	CS1
	REST0
	delay_1ms(1);
	//delay_us(2);   
	REST1
	Init_data(0x00e5, 0x8000);	
	Init_data(0x0000, 0x0001);	
	Init_data(0x0001, 0x0000);//输出方式控制0x00,0x100,0x400,0x500,正常显示用0x00
	Init_data(0x0002, 0x0700);//可设置为0x400或0x700;
	Init_data(0x0003, 0x1018);//可设置为0x1030,0x1000,0x1010,0x1020,0x1038,0x1028,0x1018,0x1008;
	        //还可以把最前面的1去掉，如:0x18;
	        //BGR = " 0 ":遵循gb顺序来写的像素数据【即去掉那个1】
	        //BGR = " 1 ":将RGB数据换成BGR数据再写入GRAM【即不去掉那个1】
	        //横向设置0x1018最好/纵向设置0x1000最好
	Init_data(0x0004, 0x0000);//显示大小尺寸控制寄存器0x00为1倍大小，即240x320
	Init_data(0x0008, 0x0202);//可设置为0x0202,0x00,	
	Init_data(0x0009, 0x0000);//扫描设置	
	Init_data(0x000A, 0x0000);
	Init_data(0x000C, 0x0000);	
	Init_data(0x000D, 0x0000);	
	Init_data(0x000F, 0x0000);	
	//-----Power On sequence
	Init_data(0x0010, 0x0000);	
	Init_data(0x0011, 0x0007);	
	Init_data(0x0012, 0x0000);	
	Init_data(0x0013, 0x0000);	
	Init_data(0x0010, 0x17B0);	
	Init_data(0x0011, 0x0007);	
	Init_data(0x0012, 0x013A);	
	Init_data(0x0013, 0x1A00);	
	Init_data(0x0029, 0x000c);		
	//-----Gamma control
	Init_data(0x0030, 0x0000);	
	Init_data(0x0031, 0x0505);	
	Init_data(0x0032, 0x0004);	
	Init_data(0x0035, 0x0006);	
	Init_data(0x0036, 0x0707);	
	Init_data(0x0037, 0x0105);	
	Init_data(0x0038, 0x0002);	
	Init_data(0x0039, 0x0707);	
	Init_data(0x003C, 0x0704);	
	Init_data(0x003D, 0x0807);	
	//-----Set RAM area
	Init_data(0x0050, 0x0000);
	Init_data(0x0051, 0x00EF);
	Init_data(0x0052, 0x0000);
	Init_data(0x0053, 0x013F);
	Init_data(0x0060, 0x2700);
	Init_data(0x0061, 0x0001);
	Init_data(0x006A, 0x0000);
	Init_data(0x0021, 0x0000);	
	Init_data(0x0020, 0x0000);	
	//-----Partial Display Control
	Init_data(0x0080, 0x0000);	
	Init_data(0x0081, 0x0000);
	Init_data(0x0082, 0x0000);
	Init_data(0x0083, 0x0000);
	Init_data(0x0084, 0x0000);	
	Init_data(0x0085, 0x0000);
	//-----Panel Contro
	Init_data(0x0090, 0x0010);	
	Init_data(0x0092, 0x0000);
	Init_data(0x0093, 0x0003);
	Init_data(0x0095, 0x0110);
	Init_data(0x0097, 0x0000);	
	Init_data(0x0098, 0x0000);
	//-----Display on
	Init_data(0x0007, 0x0173);
	//Show_RGB(0,239,0,319,White);		
}

/*******************************************************************************
函数名称：void LCD_SetPos(uint16 x0,uint16 x1,uint16 y0,uint16 y1)
函数功能：定义显示窗体（区域）
入口参数：x0:  窗体中X坐标中较小者
          x1:  窗体中X坐标中较大者
          y0:  窗体中Y坐标中较小者
          y1:  窗体中Y坐标中较大者 
*******************************************************************************/
void LCD_SetPos(uint16 x0,uint16 x1,uint16 y0,uint16 y1)
{
  Init_data(WINDOW_XADDR_START,x0);
  Init_data(WINDOW_XADDR_END,x1);
  Init_data(WINDOW_YADDR_START,y0);
  Init_data(WINDOW_YADDR_END,y1);
  Init_data(GRAM_XADDR,x0);
  Init_data(GRAM_YADDR,y0);
  Write_Cmd (0x00,0x22);
}


void DisPoint(uint8 x, uint16 y, uint16 color)
{ 
  LCD_SetPos(x,x,y,y);
  Write_Data_U16(color);
}

void Dis32x32(uint16 y,uint16 x,uint8 *Code32x32,uint16 fColor,uint16 bColor)
{
  uint16 i,j;
  LCD_SetPos(x,x+31,y,y+31);
  LCD_SetPos(x,x+31,y,y+31);
  for(i=0;i<128;i++)                                 //按照字模成分描点 
  {
    uint8 w = Code32x32[i];
    for(j=0;j<8;j++) 
    {
      uint16 Color= (w & 0x80) ? fColor : bColor;    //当为字模成份时描前景色                                                                                                              //否则描背景色
      Write_Data_U16(Color);
      w<<=1;
    } 
  }
      	
}


//void Dis16x16(uint8 x,uint8 y,uint8 *Code16x16,uint16 fColor)
//{
//  uint16 i,j;
//  LCD_SetPos(x,x+15,y,y+15);
//  for(i=0;i<32;i++)                               //按照字模成分描点 
//  {
//    uint8 w = Code16x16[i];
//    for(j=0;j<8;j++) 
//    {
//      uint16 Color= (w&0x80)? fColor:bColor;      //条件运算符，当为字模成份时描前景色
//                                                  //否则描背景色
//      Write_Data_U16(Color);
//      w<<=1;
//    } 
//  }	
//}

/*
uint8 ShowOneASCII(uint8 pcStr,uint16 x, uint16 y,uint16 fColor)
{
    uint8 i,j;
	uint8 *ucChar;
	ucChar = (uint8 *)&ASCII[(pcStr - 0x20)][0]; 
    LCD_SetPos(x,x+7,y,y+15);
    for(i=0;i<16;i++)                             //按字模成分描点
    {
      for(j=0;j<8;j++)
      {
        uint16 Color= (((*ucChar)<<j)&0x80)? fColor:bColor;    //条件运算符，当为字模成份时描前景色                                          
        Write_Data_U16(Color);							//否则描背景色
      }
	   ucChar++;               
    }
	return 8;
}

*/
/*******************************************************************************
**函数功能：显示一个汉字函数
**入口参数：*pucMask,字库中要显示字符的指针 
**备    注：要显示的指针需要使用组库检索找到
*******************************************************************************/
/*
u32 __writeOneHzChar(u8 *pucMsk,u16 x0,u16 y0,u16 color)
{
    u32 i,j;
    u16 mod[16];                                             					//当前字模
    u16 *pusMsk;                                           						//当前字库地址
    u16 y;
	Init_data(0x50,x0);           											//水平方向GRAM起始地址
	Init_data(0x51,x0+15);													//水平方向GRAM结束地址
	Init_data(0x52,y0);           											//垂直方向GRAM起始地址
	Init_data(0x53,y0+15);    												//垂直方向GRAM结束地址	
	   
    pusMsk = (u16 *)pucMsk;
    for(i=0; i<16; i++)                                             			//保存当前汉字点阵式字模
    {
        mod[i] = *pusMsk++;                                             		//取得当前字模，半字对齐访问
        mod[i] = ((mod[i] & 0xff00) >> 8) | ((mod[i] & 0x00ff) << 8);   		//为显示需要，字模交换高低字节
    }
    y = y0;
    for(i=0; i<16; i++) 														//16行
	{                                               		
  		Init_data(GRAM_XADDR,x0);
  		Init_data(GRAM_YADDR,y);
  		Write_Cmd (0x00,0x22);			 	  
        for(j=0; j<16; j++) 													// 16列
		{                         							
            if((mod[i] << j) & 0x8000) 											// 显示字模
			{                						
				Write_Data_U16(color);    
            } 
			else 
			{
				Write_Data_U16(bColor);          								//用读方式跳过写空白点的像素
            }
        }
        y++;
    }
    return (16);                                            					//返回16位列宽
}
*/
/*
void writeString(u8 *pcStr,u16 x0,u16 y0,u16 color)
{
    u16 usIndex = 0;
    u16 usWidth = 0;
    FNT_GB16 *ptGb16 = 0;    
    ptGb16 = (FNT_GB16 *)GBHZ_16;
    while(1)
    {
        if(*pcStr == 0) 													   //字符串结束
		{
            break;                                                      		
        }
        
        x0 = x0 + (usWidth);                                            		//调节字符串显示松紧度
        if(*pcStr > 0x80) 
		{                                                               		//判断为汉字
            if((x0 + 16) >= Gus_LCM_XMAX) 
			{                                                            		//检查剩余空间是否足够
                x0 = 0;
                y0 = y0 + 16;                                           		//改变显示坐标
                if(y0 >= Gus_LCM_YMAX) 											//纵坐标超出
				{                                
                    y0 = 0;
                }
            }
            usIndex = findHzIndex(pcStr);
            usWidth = __writeOneHzChar((u8 *)&(ptGb16[usIndex].Msk[0]), x0, y0, color);
                                                                        		//显示字符
            pcStr += 2;
        } 
		else 
		{                                                         				//判断为非汉字
            if (*pcStr == '\n') 
			{                                                     				//换行
                y0 = y0 + 16;                                     				//改变显示坐标
                if(y0 >= Gus_LCM_YMAX) 
				{                                                 				//纵坐标超出
                    y0 = 0;
                }
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else if (*pcStr == '\r') 										   	//对齐到起点
			{                           				
                x0 = 0;
                pcStr++;
                usWidth = 0;
                continue;
            }
			else
			{
                if((x0 + 8) >= Gus_LCM_XMAX) 									//检查剩余空间是否足够
				{                                                       
                    x0 = 0;
                    y0 = y0 + 16;                                       		//改变显示坐标
                    if(y0 >= Gus_LCM_YMAX) 										//纵坐标超出
					{                                                   
                        y0 = 0;
                    }
                }
                usWidth = ShowOneASCII(*pcStr, x0, y0, color);
                                                                        		//ASCII码表21H的值对应区位码3区
                pcStr += 1;
            }
        }
    }
	//恢复窗体大小	 
	Init_data(0x50, 0x0000); 												//水平方向GRAM起始地址
	Init_data(0x51, 0x00EF); 												//水平方向GRAM结束地址
	Init_data(0x52, 0x0000); 												//垂直方向GRAM起始地址
	Init_data(0x53, 0x013F); 												//垂直方向GRAM结束地址
}
*/

////写16X32字符
//void Dis16x32(uint16 y, uint16 x,uint8 *Code16x32,uint16 fColor,uint16 bColor)
//{
//    uint16 i,j;
//    LCD_SetPos(x,x+31,y,y+15);
//    
//    for(i=0;i<64;i++)                             //按字模成分描点
//    {
//      uint8 w = Code16x32[i];
//      for(j=0;j<8;j++)
//      {
//        uint16 Color= (w&0x80)? fColor:bColor;    //条件运算符，当为字模成份时描前景色
//                                                  //否则描背景色
//        Write_Data_U16(Color);
//        w<<=1;
//      }
//                    
//    }
//	
//}






