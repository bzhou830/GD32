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
�ļ���������趨����ͳһ����������
 
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
�������ƣ�void ClearScreen(uint16 Color)
�������ܣ�ˢ��ʹ��ȫ����ʾ��һ����ɫ
��ڲ�����Clolr:��Ļ��������ɫ��ֱ��д����ɫ��Ӣ�ģ���.h���ж��壩
********************************************************************************/
void ClearScreen(uint16 Color)
{
  uint16 i,j;
  LCD_SetPos(0,320,0,240);                                  //������ʼ��ַΪ��0��0�� ~ (320,240)
  for (i = 0;i < 325;i++)
  {
    for (j = 0;j < 240;j++)
    {
      Write_Data_U16(Color);
    }
  }
}


/*******************************************************************************
�������ƣ�void Show_RGB (uint16 x0,uint16 x1,uint16 y0,uint16 y1,uint16 Color)
�������ܣ�����Ļ�ģ�x0,y0������x1,y1��������ʾĳһ����ɫ
��ڲ�������x0,y0���ͣ�x1,y1����ʾλ�ã�
           Color :��ʾ�������ɫ
��    ע������ʵ�ֻ���ͻ�ˮƽ�ʹ�ֱ��ֱ��
          ��ʾʱ����Ĳ���x��0-240��y:0-320������
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
��ֱ�ߣ����Ի����ⷽ��ֱ�ߣ��������ߡ����ߡ�б�ߣ���
��ڲ����� (x1,y1)���, (x2,y2)�յ�, color��ɫ��
���ڲ���: ��
˵    ������ָ������ɫ��ָ��������仭��һ��ֱ�ߡ�
***********************************************************************/
void Disline(uint x1,uint x2,uint y1,uint y2,uint color)  
{  
  uint16 t;  
  int xerr=0,yerr=0,delta_x,delta_y,distance;  
  int incx,incy;  
  uint16 row,col;  
  delta_x = x2-x1;                            //������������  
  delta_y = y2-y1;  
  col = x1;  
  row = y1;  
  if(delta_x>0) incx=1;                       //���õ�������  
  else   
  {  
    if(delta_x == 0) incx = 0;                //��ֱ��  
    else {incx=-1;delta_x=-delta_x;}  
  }  
  if(delta_y > 0)incy = 1;  
  else  
  {  
    if(delta_y == 0) incy = 0;                //ˮƽ��  
    else {incy = -1;delta_y = -delta_y;}  
  }  
  if(delta_x>delta_y) distance=delta_x;       //ѡȡ��������������  
  else distance=delta_y;  
  
  for(t=0;t<=distance+1;t++)  
  {                                           //�������  
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
�������ƣ�LcdDrawCircle(uint8 cx,uint8 cy,uint8 r)
�������ܣ���Բ
���������Բ�ĺͰ뾶
���������
��    ע��
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
�������ƣ�void DrawCoord(uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
�������ܣ�����Ļ�ϻ����꣬��Ļ�Ǻ�ŵ�
��ڲ�������x0,y0���ͣ�x1,y1����ʾλ�ã�
           Color :��ʾ�������ɫ
           Line_Width:��������߿�
*******************************************************************************/
void DrawCoord(uint16 x0,uint16 x1,uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
{
  LineWidth = LineWidth-1;
  Show_RGB (x0,x0 + LineWidth,y0,y1,Color);  //��
  Show_RGB (x0,x1,y1 - LineWidth,y1,Color);  //��
}



/*******************************************************************************
�������ƣ�void Show_Xmark(uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
�������ܣ�����Ļ�ϻ����ߣ���Ļ�Ǻ�ŵ�
��ڲ�������x0,y0���ͣ�x1,y1����ʾλ�ã�
           Color :��ʾ�������ɫ
           Line_Width:��������߿�
*******************************************************************************/
void Show_Xmark(uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
{
  LineWidth = LineWidth-1;
  Show_RGB(x0,x0 + LineWidth,y0,y1,Color);  //��
}


/*******************************************************************************
�������ƣ�void Show_Xmark(uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
�������ܣ�����Ļ�ϻ����ߣ���Ļ�Ǻ�ŵ�
��ڲ�������x0,y0���ͣ�x1,y1����ʾλ�ã�
           Color :��ʾ�������ɫ
           Line_Width:��������߿�
*******************************************************************************/
void Show_Ymark (uint16 x0,uint16 x1, uint16 y0, uint16 y1, uint16 LineWidth, uint16 Color)
{
  LineWidth = LineWidth - 1;
  Show_RGB(x0,x1,y0,y0 + LineWidth,Color);
}



/*******************************************************************************
�������ƣ�void  Init_data (uint8 x,uint16 y)
�������ܣ�Щ���ݵ��Ĵ���
��ڲ�����x,y ��Ҫд�������
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
�������ƣ�void  Write_Data_U16(uint16 y)
�������ܣ�д����ɫ
��ڲ�����y ��ɫ����
*******************************************************************************/
void  Write_Data_U16(uint16 y)
{
  uint8 m,n;
  m = y >> 8;
  n = y;
  Write_Data(m,n);
}


/*******************************************************************************
�������ƣ�void Write_Cmd(uint8 DH,uint8 DL)
�������ܣ�дָ��
��ڲ�����DH ��8λ����  DL  ��8λ����
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
�������ƣ�void Write_Data(uint8 DH,uint8 DL)
�������ܣ�д����
��ڲ�����DH ��8λ����  DL  ��8λ����
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
�������ƣ�void Init_ILI9325()
�������ܣ���ʼ��Һ��ģ��
*******************************************************************************/
void Init_ILI9325()
{ 
	/*
	GPIO_InitTypeDef  GPIO_InitStructure;				   //�������ýṹ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);  //�򿪶˿�Dʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;			   //ѡ�ж˿�0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		   //���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		   //�������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //�˿��ٶ�
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		   //����
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
	Init_data(0x0001, 0x0000);//�����ʽ����0x00,0x100,0x400,0x500,������ʾ��0x00
	Init_data(0x0002, 0x0700);//������Ϊ0x400��0x700;
	Init_data(0x0003, 0x1018);//������Ϊ0x1030,0x1000,0x1010,0x1020,0x1038,0x1028,0x1018,0x1008;
	        //�����԰���ǰ���1ȥ������:0x18;
	        //BGR = " 0 ":��ѭgb˳����д���������ݡ���ȥ���Ǹ�1��
	        //BGR = " 1 ":��RGB���ݻ���BGR������д��GRAM������ȥ���Ǹ�1��
	        //��������0x1018���/��������0x1000���
	Init_data(0x0004, 0x0000);//��ʾ��С�ߴ���ƼĴ���0x00Ϊ1����С����240x320
	Init_data(0x0008, 0x0202);//������Ϊ0x0202,0x00,	
	Init_data(0x0009, 0x0000);//ɨ������	
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
�������ƣ�void LCD_SetPos(uint16 x0,uint16 x1,uint16 y0,uint16 y1)
�������ܣ�������ʾ���壨����
��ڲ�����x0:  ������X�����н�С��
          x1:  ������X�����нϴ���
          y0:  ������Y�����н�С��
          y1:  ������Y�����нϴ��� 
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
  for(i=0;i<128;i++)                                 //������ģ�ɷ���� 
  {
    uint8 w = Code32x32[i];
    for(j=0;j<8;j++) 
    {
      uint16 Color= (w & 0x80) ? fColor : bColor;    //��Ϊ��ģ�ɷ�ʱ��ǰ��ɫ                                                                                                              //�����豳��ɫ
      Write_Data_U16(Color);
      w<<=1;
    } 
  }
      	
}


//void Dis16x16(uint8 x,uint8 y,uint8 *Code16x16,uint16 fColor)
//{
//  uint16 i,j;
//  LCD_SetPos(x,x+15,y,y+15);
//  for(i=0;i<32;i++)                               //������ģ�ɷ���� 
//  {
//    uint8 w = Code16x16[i];
//    for(j=0;j<8;j++) 
//    {
//      uint16 Color= (w&0x80)? fColor:bColor;      //�������������Ϊ��ģ�ɷ�ʱ��ǰ��ɫ
//                                                  //�����豳��ɫ
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
    for(i=0;i<16;i++)                             //����ģ�ɷ����
    {
      for(j=0;j<8;j++)
      {
        uint16 Color= (((*ucChar)<<j)&0x80)? fColor:bColor;    //�������������Ϊ��ģ�ɷ�ʱ��ǰ��ɫ                                          
        Write_Data_U16(Color);							//�����豳��ɫ
      }
	   ucChar++;               
    }
	return 8;
}

*/
/*******************************************************************************
**�������ܣ���ʾһ�����ֺ���
**��ڲ�����*pucMask,�ֿ���Ҫ��ʾ�ַ���ָ�� 
**��    ע��Ҫ��ʾ��ָ����Ҫʹ���������ҵ�
*******************************************************************************/
/*
u32 __writeOneHzChar(u8 *pucMsk,u16 x0,u16 y0,u16 color)
{
    u32 i,j;
    u16 mod[16];                                             					//��ǰ��ģ
    u16 *pusMsk;                                           						//��ǰ�ֿ��ַ
    u16 y;
	Init_data(0x50,x0);           											//ˮƽ����GRAM��ʼ��ַ
	Init_data(0x51,x0+15);													//ˮƽ����GRAM������ַ
	Init_data(0x52,y0);           											//��ֱ����GRAM��ʼ��ַ
	Init_data(0x53,y0+15);    												//��ֱ����GRAM������ַ	
	   
    pusMsk = (u16 *)pucMsk;
    for(i=0; i<16; i++)                                             			//���浱ǰ���ֵ���ʽ��ģ
    {
        mod[i] = *pusMsk++;                                             		//ȡ�õ�ǰ��ģ�����ֶ������
        mod[i] = ((mod[i] & 0xff00) >> 8) | ((mod[i] & 0x00ff) << 8);   		//Ϊ��ʾ��Ҫ����ģ�����ߵ��ֽ�
    }
    y = y0;
    for(i=0; i<16; i++) 														//16��
	{                                               		
  		Init_data(GRAM_XADDR,x0);
  		Init_data(GRAM_YADDR,y);
  		Write_Cmd (0x00,0x22);			 	  
        for(j=0; j<16; j++) 													// 16��
		{                         							
            if((mod[i] << j) & 0x8000) 											// ��ʾ��ģ
			{                						
				Write_Data_U16(color);    
            } 
			else 
			{
				Write_Data_U16(bColor);          								//�ö���ʽ����д�հ׵������
            }
        }
        y++;
    }
    return (16);                                            					//����16λ�п�
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
        if(*pcStr == 0) 													   //�ַ�������
		{
            break;                                                      		
        }
        
        x0 = x0 + (usWidth);                                            		//�����ַ�����ʾ�ɽ���
        if(*pcStr > 0x80) 
		{                                                               		//�ж�Ϊ����
            if((x0 + 16) >= Gus_LCM_XMAX) 
			{                                                            		//���ʣ��ռ��Ƿ��㹻
                x0 = 0;
                y0 = y0 + 16;                                           		//�ı���ʾ����
                if(y0 >= Gus_LCM_YMAX) 											//�����곬��
				{                                
                    y0 = 0;
                }
            }
            usIndex = findHzIndex(pcStr);
            usWidth = __writeOneHzChar((u8 *)&(ptGb16[usIndex].Msk[0]), x0, y0, color);
                                                                        		//��ʾ�ַ�
            pcStr += 2;
        } 
		else 
		{                                                         				//�ж�Ϊ�Ǻ���
            if (*pcStr == '\n') 
			{                                                     				//����
                y0 = y0 + 16;                                     				//�ı���ʾ����
                if(y0 >= Gus_LCM_YMAX) 
				{                                                 				//�����곬��
                    y0 = 0;
                }
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else if (*pcStr == '\r') 										   	//���뵽���
			{                           				
                x0 = 0;
                pcStr++;
                usWidth = 0;
                continue;
            }
			else
			{
                if((x0 + 8) >= Gus_LCM_XMAX) 									//���ʣ��ռ��Ƿ��㹻
				{                                                       
                    x0 = 0;
                    y0 = y0 + 16;                                       		//�ı���ʾ����
                    if(y0 >= Gus_LCM_YMAX) 										//�����곬��
					{                                                   
                        y0 = 0;
                    }
                }
                usWidth = ShowOneASCII(*pcStr, x0, y0, color);
                                                                        		//ASCII���21H��ֵ��Ӧ��λ��3��
                pcStr += 1;
            }
        }
    }
	//�ָ������С	 
	Init_data(0x50, 0x0000); 												//ˮƽ����GRAM��ʼ��ַ
	Init_data(0x51, 0x00EF); 												//ˮƽ����GRAM������ַ
	Init_data(0x52, 0x0000); 												//��ֱ����GRAM��ʼ��ַ
	Init_data(0x53, 0x013F); 												//��ֱ����GRAM������ַ
}
*/

////д16X32�ַ�
//void Dis16x32(uint16 y, uint16 x,uint8 *Code16x32,uint16 fColor,uint16 bColor)
//{
//    uint16 i,j;
//    LCD_SetPos(x,x+31,y,y+15);
//    
//    for(i=0;i<64;i++)                             //����ģ�ɷ����
//    {
//      uint8 w = Code16x32[i];
//      for(j=0;j<8;j++)
//      {
//        uint16 Color= (w&0x80)? fColor:bColor;    //�������������Ϊ��ģ�ɷ�ʱ��ǰ��ɫ
//                                                  //�����豳��ɫ
//        Write_Data_U16(Color);
//        w<<=1;
//      }
//                    
//    }
//	
//}






