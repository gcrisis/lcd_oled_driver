#include "LCD_common.h"
#include "stdlib.h"
#include "usart.h"	 
#include "delay.h"	 

//////////////////////////////////////////////////////////////////////////////////	 
				 
//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;
	 
//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(vu16 regval)
{   
	regval=regval;		//使用-O2优化的时候,必须插入的延时
	LCD->LCD_REG=regval;//写入要写的寄存器序号	 
}
//写LCD数据
//data:要写入的值
void LCD_WR_DATA(vu16 data)
{	 
	data=data;			//使用-O2优化的时候,必须插入的延时
	LCD->LCD_RAM=data;		 
}
//读LCD数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{		
	vu16 ram;			//防止被优化
	ram=LCD->LCD_RAM;
	return ram;		 
}					   
//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteRegData(vu16 LCD_Reg, vu16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		//写入要写的寄存器序号	 
	LCD->LCD_RAM = LCD_RegValue;//写入数据	    		 
}	   
//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据
u16 LCD_ReadRegData(vu16 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);		//写入要读的寄存器序号
	delay_us(5);		  
	return LCD_RD_DATA();		//返回读到的值
}   
//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}	 
//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;//写十六位GRAM
}
//从ILI93xx读出的数据为BGR格式，而我们写入的时候为RGB格式。
//c:BGR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 
//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
	while(i--);
}





























