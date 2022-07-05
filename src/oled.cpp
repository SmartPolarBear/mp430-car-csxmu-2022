//
/*********************OLED驱动程序**************************/
// ------------------------------------------------------------
// IO口模拟I2C通信
// SCL接P2^3
// SDA接P2^2
// ------------------------------------------------------------

#include <msp430g2553.h>

#include <oledfont.h>
#include <oled.h>

#define SCL_1 OLED_HIGH |= OLED_SCL_LOW
#define SCL_0 OLED_HIGH &= ~OLED_SCL_LOW
#define SDA_1 OLED_HIGH |= OLED_SDA_LOW
#define SDA_0 OLED_HIGH &= ~OLED_SDA_LOW

/*********************OLED延迟程序**************************/
void delay(unsigned int z)
{
  volatile unsigned int x, y;
  for (x = z; x > 0; x--)
    for (y = 100; y > 0; y--)
      ;
}

/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{
  SCL_1;
  SDA_1;
  SDA_0;
  SCL_0;
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
  SCL_0;
  SDA_0;
  SCL_0;
  SDA_0;
}

/**********************************************
// 通过I2C总线写一个字节
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
    if (IIC_Byte & 0x80)
      SDA_1;
    else
      SDA_0;
    SCL_1;
    SCL_0;
    IIC_Byte <<= 1;
  }
  SDA_1;
  SCL_1;
  SCL_0;
}

/*********************OLED写数据************************************/
void OLED_WrDat(unsigned char IIC_Data)
{
  IIC_Start();
  Write_IIC_Byte(0x78);
  Write_IIC_Byte(0x40); // write data
  Write_IIC_Byte(IIC_Data);
  IIC_Stop();
}
/*********************OLED写命令************************************/
void OLED_WrCmd(unsigned char IIC_Command)
{
  IIC_Start();
  Write_IIC_Byte(0x78); // Slave address,SA0=0
  Write_IIC_Byte(0x00); // write command
  Write_IIC_Byte(IIC_Command);
  IIC_Stop();
}
/*********************OLED 设置坐标************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
  OLED_WrCmd(0xb0 + y);
  OLED_WrCmd(((x & 0xf0) >> 4) | 0x10);
  OLED_WrCmd((x & 0x0f) | 0x01);
}
/*********************OLED全屏************************************/
void OLED_Fill(unsigned char bmp_dat)
{
  unsigned char y, x;
  for (y = 0; y < 8; y++)
  {
    OLED_WrCmd(0xb0 + y);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for (x = 0; x < X_WIDTH; x++)
      OLED_WrDat(bmp_dat);
  }
}
/*********************OLED复位************************************/
void OLED_CLS(void)
{
  unsigned char y, x;
  for (y = 0; y < 8; y++)
  {
    OLED_WrCmd(0xb0 + y);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for (x = 0; x < X_WIDTH; x++)
      OLED_WrDat(0);
  }
}
//写字节
void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
  if (cmd)
  {
    OLED_WrDat(dat);
  }
  else
  {
    OLED_WrCmd(dat);
  }
}
//显示汉字
void OLED_ShowChinese(unsigned char x, unsigned char y, unsigned char no)
{
  unsigned char t, adder = 0;
  OLED_Set_Pos(x, y);
  for (t = 0; t < 16; t++)
  {
    OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
    adder += 1;
  }
  OLED_Set_Pos(x, y + 1);
  for (t = 0; t < 16; t++)
  {
    OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
    adder += 1;
  }
}
//在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr)
{
  unsigned char c = 0, i = 0;
  c = chr - ' '; //得到偏移后的值
  if (x > 128 - 1)
  {
    x = 0;
    y = y + 2;
  }

  OLED_Set_Pos(x, y);
  for (i = 0; i < 8; i++)
    OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
  OLED_Set_Pos(x, y + 1);
  for (i = 0; i < 8; i++)
    OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
}
/*********************OLED初始化************************************/
void OLED_Init(void)
{
  OLED_DIR |= (OLED_SCL_LOW + OLED_SDA_LOW);  //设置IO口方向为输出0000 1100
  OLED_HIGH |= (OLED_SCL_LOW + OLED_SDA_LOW); //初始设置为0000 1100

  delay(4000); //初始化之前的延时很重要！

  OLED_WrCmd(0xae);       //--turn off oled panel
  OLED_WrCmd(0x00);       //---set low column address
  OLED_WrCmd(0x10);       //---set high column address
  OLED_WrCmd(0x40);       //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WrCmd(0x81);       //--set contrast control register
  OLED_WrCmd(BRIGHTNESS); // Set SEG Output Current Brightness
  OLED_WrCmd(0xa1);       //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
  OLED_WrCmd(0xc8);       // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
  OLED_WrCmd(0xa6);       //--set normal display
  OLED_WrCmd(0xa8);       //--set multiplex ratio(1 to 64)
  OLED_WrCmd(0x3f);       //--1/64 duty
  OLED_WrCmd(0xd3);       //-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WrCmd(0x00);       //-not offset
  OLED_WrCmd(0xd5);       //--set display clock divide ratio/oscillator frequency
  OLED_WrCmd(0x80);       //--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WrCmd(0xd9);       //--set pre-charge period
  OLED_WrCmd(0xf1);       // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WrCmd(0xda);       //--set com pins hardware configuration
  OLED_WrCmd(0x12);
  OLED_WrCmd(0xdb); //--set vcomh
  OLED_WrCmd(0x40); // Set VCOM Deselect Level
  OLED_WrCmd(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WrCmd(0x02); //
  OLED_WrCmd(0x8d); //--set Charge Pump enable/disable
  OLED_WrCmd(0x14); //--set(0x10) disable
  OLED_WrCmd(0xa4); // Disable Entire Display On (0xa4/0xa5)
  OLED_WrCmd(0xa6); // Disable Inverse Display On (0xa6/a7)
  OLED_WrCmd(0xaf); //--turn on oled panel
  OLED_Fill(0x00);  //初始清屏
  OLED_Set_Pos(0, 0);
}
/*********************OLED驱动程序结束**************************/
//
