#include "LCD_common.h"
#include "LCD_ili93xx.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "delay.h"	
#include "Picture.h" 

u16 LCD_ReadPoint(u16 x,u16 y)
{
 	vu16 r=0,g=0,b=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	LCD_SetCursor(x,y);	    
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X2E);//9341/6804/3510 发送读GRAM指令
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2E00);	//5510 发送读GRAM指令
	else LCD_WR_REG(R34);      		 				//其他IC发送读GRAM指令
	if(lcddev.id==0X9320)opt_delay(2);				//FOR 9320,延时2us	    
 	LCD_RD_DATA();									//dummy Read	   
	opt_delay(2);	  
 	r=LCD_RD_DATA();  		  						//实际坐标颜色
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)		//9341/NT35310/NT35510要分2次读出
 	{
		opt_delay(2);	  
		b=LCD_RD_DATA(); 
		g=r&0XFF;		//对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
		g<<=8;
	} 
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0XB505||lcddev.id==0XC505)return r;	//这几种IC直接返回颜色值
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510需要公式转换一下
	else return LCD_BGR2RGB(r);						//其他IC
}			 
//LCD开启显示
void LCD_DisplayOn(void)
{					   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X29);	//开启显示
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2900);	//开启显示
	else LCD_WriteRegData(R7,0x0173); 				 	//开启显示
}	 
//LCD关闭显示
void LCD_DisplayOff(void)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X28);	//关闭显示
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2800);	//关闭显示
	else LCD_WriteRegData(R7,0x0);//关闭显示 
}   
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
 	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{		    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8); 
		LCD_WR_DATA(Xpos&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8); 
		LCD_WR_DATA(Ypos&0XFF);
	}else if(lcddev.id==0X6804)
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏时处理
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8); 
		LCD_WR_DATA(Xpos&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8); 
		LCD_WR_DATA(Ypos&0XFF);
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8); 
		LCD_WR_REG(lcddev.setxcmd+1); 
		LCD_WR_DATA(Xpos&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8); 
		LCD_WR_REG(lcddev.setycmd+1); 
		LCD_WR_DATA(Ypos&0XFF);		
	}else
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏其实就是调转x,y坐标
		LCD_WriteRegData(lcddev.setxcmd, Xpos);
		LCD_WriteRegData(lcddev.setycmd, Ypos);
	}	 
} 		 
//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510等IC已经实际测试	   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  
	u16 xsize,ysize;
	if(lcddev.dir==1&&lcddev.id!=0X6804)//横屏时，对6804不改变扫描方向！
	{			   
		switch(dir)//方向转换
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	}
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510)//9341/6804/5310/5510,很特殊
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510))regval|=0X08;//5310/5510不需要BGR   
		if(lcddev.id==0X6804)regval|=0x02;//6804的BIT6和9341的反了	   
		LCD_WriteRegData(dirreg,regval);
 		if((regval&0X20)||lcddev.dir==1)
		{
			if(lcddev.width<lcddev.height)//交换X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}else  
		{
			if(lcddev.width>lcddev.height)//交换X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}  
		if(lcddev.dir==1&&lcddev.id!=0X6804)
		{
			xsize=lcddev.height;
			ysize=lcddev.width;
		}else
		{
			xsize=lcddev.width;
			ysize=lcddev.height;
		}
		if(lcddev.id==0X5510)
		{
			LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((xsize-1)>>8); 
			LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((xsize-1)&0XFF); 
			LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((ysize-1)>>8); 
			LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((ysize-1)&0XFF);
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((xsize-1)>>8);LCD_WR_DATA((xsize-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((ysize-1)>>8);LCD_WR_DATA((ysize-1)&0XFF);  
		}
  	}else 
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(1<<5)|(1<<4)|(0<<3); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(0<<5)|(1<<4)|(0<<3); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(1<<5)|(0<<4)|(0<<3);
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(0<<5)|(0<<4)|(0<<3); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(1<<5)|(1<<4)|(1<<3); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(1<<5)|(0<<4)|(1<<3); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(0<<5)|(1<<4)|(1<<3); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(0<<5)|(0<<4)|(1<<3); 
				break;	 
		}
		if(lcddev.id==0x8989)//8989 IC
		{
			dirreg=0X11;
			regval|=0X6040;	//65K   
	 	}else//其他驱动IC		  
		{
			dirreg=0X03;
			regval|=1<<12;  
		}
		LCD_WriteRegData(dirreg,regval);
	}
}     
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD->LCD_RAM=POINT_COLOR; 
}
//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8); 
		LCD_WR_DATA(x&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8); 
		LCD_WR_DATA(y&0XFF);
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(x>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(x&0XFF);	  
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(y>>8);  
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(y&0XFF); 
	}else if(lcddev.id==0X6804)
	{		    
		if(lcddev.dir==1)x=lcddev.width-1-x;//横屏时处理
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8); 
		LCD_WR_DATA(x&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8); 
		LCD_WR_DATA(y&0XFF);
	}else
	{
 		if(lcddev.dir==1)x=lcddev.width-1-x;//横屏其实就是调转x,y坐标
		LCD_WriteRegData(lcddev.setxcmd,x);
		LCD_WriteRegData(lcddev.setycmd,y);
	}			 
	LCD->LCD_REG=lcddev.wramcmd; 
	LCD->LCD_RAM=color; 
}	 


//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
			if(lcddev.id==0X6804||lcddev.id==0X5310)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=480;
			lcddev.height=800;
		}else
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R32;
			lcddev.setycmd=R33;  
		}
	}else 				//横屏
	{	  				
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}else if(lcddev.id==0X6804)	 
		{
 			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2B;
			lcddev.setycmd=0X2A; 
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=800;
			lcddev.height=480;
		}else
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R33;
			lcddev.setycmd=R32;  
		}
		if(lcddev.id==0X6804||lcddev.id==0X5310)
		{ 	 
			lcddev.width=480;
			lcddev.height=320; 			
		}
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}	 
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
//68042,横屏时不支持窗口设置!! 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{   
	u8 hsareg,heareg,vsareg,veareg;
	u16 hsaval,heaval,vsaval,veaval; 
	width=sx+width-1;
	height=sy+height-1;
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X6804)//6804横屏不支持
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA(width>>8); 
		LCD_WR_DATA(width&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(height>>8); 
		LCD_WR_DATA(height&0XFF); 
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(sx>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(sx&0XFF);	  
		LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA(width>>8);   
		LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA(width&0XFF);   
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(sy>>8);   
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(sy&0XFF);  
		LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA(height>>8);   
		LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA(height&0XFF);  
	}else	//其他驱动IC
	{
		if(lcddev.dir==1)//横屏
		{
			//窗口值
			hsaval=sy;				
			heaval=height;
			vsaval=lcddev.width-width-1;
			veaval=lcddev.width-sx-1;				
		}else
		{ 
			hsaval=sx;				
			heaval=width;
			vsaval=sy;
			veaval=height;
		} 
		hsareg=0X50;heareg=0X51;//水平方向窗口寄存器
		vsareg=0X52;veareg=0X53;//垂直方向窗口寄存器	   							  
		//设置寄存器值
		LCD_WriteRegData(hsareg,hsaval);
		LCD_WriteRegData(heareg,heaval);
		LCD_WriteRegData(vsareg,vsaval);
		LCD_WriteRegData(veareg,veaval);		
		LCD_SetCursor(sx,sy);	//设置光标位置
	}
} 
//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试! 
u16 LCD_getID()
{
	delay_ms(50); // delay 50 ms 
 	LCD_WriteRegData(0x0000,0x0001);
	delay_ms(50); // delay 50 ms 
  	lcddev.id = LCD_ReadRegData(0x0000);
	
	LCD_WR_REG(0XD3);				   
	lcddev.id=LCD_RD_DATA();	//dummy read 	
	lcddev.id=LCD_RD_DATA(); 	//读到0X00
	lcddev.id=LCD_RD_DATA();   	//读取93								   
	lcddev.id<<=8;
	lcddev.id|=LCD_RD_DATA();  	//读取41 
	return lcddev.id;
}
void LCD_Init(void)
{ 	
	vu32 i=0;
	
	//重新配置写时序控制寄存器的时序   	 							    
	FSMC_Bank1E->BWTR[6]&=~(0XF<<0);//地址建立时间（ADDSET）清零 	 
	FSMC_Bank1E->BWTR[6]&=~(0XF<<8);//数据保存时间清零
	FSMC_Bank1E->BWTR[6]|=3<<0;		//地址建立时间（ADDSET）为3个HCLK =18ns  	 
	FSMC_Bank1E->BWTR[6]|=2<<8; 	//数据保存时间为6ns*3个HCLK=18ns
	LCD_getID();
	printf(" LCD ID:%x\r\n",lcddev.id); //打印LCD ID  
	if(lcddev.id==0X9341)	//9341初始化
	{	 
		LCD_WR_REG(0xCF);  
		LCD_WR_DATA(0x00);
		i++;
		LCD_WR_DATA(0xC1); 
		LCD_WR_DATA(0X30); 
		LCD_WR_REG(0xED);  
		LCD_WR_DATA(0x64); 
		LCD_WR_DATA(0x03); 
		LCD_WR_DATA(0X12); 
		LCD_WR_DATA(0X81); 
		LCD_WR_REG(0xE8);  
		LCD_WR_DATA(0x85); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x7A); 
		LCD_WR_REG(0xCB);  
		LCD_WR_DATA(0x39); 
		LCD_WR_DATA(0x2C); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x34); 
		LCD_WR_DATA(0x02); 
		LCD_WR_REG(0xF7);  
		LCD_WR_DATA(0x20); 
		LCD_WR_REG(0xEA);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_REG(0xC0);    //Power control 
		LCD_WR_DATA(0x1B);   //VRH[5:0] 
		LCD_WR_REG(0xC1);    //Power control 
		LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
		LCD_WR_REG(0xC5);    //VCM control 
		LCD_WR_DATA(0x30); 	 //3F
		LCD_WR_DATA(0x30); 	 //3C
		LCD_WR_REG(0xC7);    //VCM control2 
		LCD_WR_DATA(0XB7); 
		LCD_WR_REG(0x36);    // Memory Access Control 
		LCD_WR_DATA(0x48); 
		LCD_WR_REG(0x3A);   
		LCD_WR_DATA(0x55); 
		LCD_WR_REG(0xB1);   
		LCD_WR_DATA(0x00);   
		LCD_WR_DATA(0x1A); 
		LCD_WR_REG(0xB6);    // Display Function Control 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0xA2); 
		LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
		LCD_WR_DATA(0x00); 
		LCD_WR_REG(0x26);    //Gamma curve selected 
		LCD_WR_DATA(0x01); 
		LCD_WR_REG(0xE0);    //Set Gamma 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x2A); 
		LCD_WR_DATA(0x28); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x0E); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x54); 
		LCD_WR_DATA(0XA9); 
		LCD_WR_DATA(0x43); 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 		 
		LCD_WR_REG(0XE1);    //Set Gamma 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x15); 
		LCD_WR_DATA(0x17); 
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x11); 
		LCD_WR_DATA(0x06); 
		LCD_WR_DATA(0x2B); 
		LCD_WR_DATA(0x56); 
		LCD_WR_DATA(0x3C); 
		LCD_WR_DATA(0x05); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3f);
		LCD_WR_REG(0x2A); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xef);	 
		LCD_WR_REG(0x11); //Exit Sleep
		delay_ms(120);
		LCD_WR_REG(0x29); //display on	
	}
	else if(lcddev.id==0x9325)//9325
	{
		LCD_WriteRegData(0x00E5,0x78F0); 
		LCD_WriteRegData(0x0001,0x0100); 
		LCD_WriteRegData(0x0002,0x0700); 
		LCD_WriteRegData(0x0003,0x1030); 
		LCD_WriteRegData(0x0004,0x0000); 
		LCD_WriteRegData(0x0008,0x0202);  
		LCD_WriteRegData(0x0009,0x0000);
		LCD_WriteRegData(0x000A,0x0000); 
		LCD_WriteRegData(0x000C,0x0000); 
		LCD_WriteRegData(0x000D,0x0000);
		LCD_WriteRegData(0x000F,0x0000);
		//power on sequence VGHVGL
		LCD_WriteRegData(0x0010,0x0000);   
		LCD_WriteRegData(0x0011,0x0007);  
		LCD_WriteRegData(0x0012,0x0000);  
		LCD_WriteRegData(0x0013,0x0000); 
		LCD_WriteRegData(0x0007,0x0000); 
		//vgh 
		LCD_WriteRegData(0x0010,0x1690);   
		LCD_WriteRegData(0x0011,0x0227);
		//delayms(100);
		//vregiout 
		LCD_WriteRegData(0x0012,0x009D); //0x001b
		//delayms(100); 
		//vom amplitude
		LCD_WriteRegData(0x0013,0x1900);
		//delayms(100); 
		//vom H
		LCD_WriteRegData(0x0029,0x0025); 
		LCD_WriteRegData(0x002B,0x000D); 
		//gamma
		LCD_WriteRegData(0x0030,0x0007);
		LCD_WriteRegData(0x0031,0x0303);
		LCD_WriteRegData(0x0032,0x0003);// 0006
		LCD_WriteRegData(0x0035,0x0206);
		LCD_WriteRegData(0x0036,0x0008);
		LCD_WriteRegData(0x0037,0x0406); 
		LCD_WriteRegData(0x0038,0x0304);//0200
		LCD_WriteRegData(0x0039,0x0007); 
		LCD_WriteRegData(0x003C,0x0602);// 0504
		LCD_WriteRegData(0x003D,0x0008); 
		//ram
		LCD_WriteRegData(0x0050,0x0000); 
		LCD_WriteRegData(0x0051,0x00EF);
		LCD_WriteRegData(0x0052,0x0000); 
		LCD_WriteRegData(0x0053,0x013F);  
		LCD_WriteRegData(0x0060,0xA700); 
		LCD_WriteRegData(0x0061,0x0001); 
		LCD_WriteRegData(0x006A,0x0000); 
		//
		LCD_WriteRegData(0x0080,0x0000); 
		LCD_WriteRegData(0x0081,0x0000); 
		LCD_WriteRegData(0x0082,0x0000); 
		LCD_WriteRegData(0x0083,0x0000); 
		LCD_WriteRegData(0x0084,0x0000); 
		LCD_WriteRegData(0x0085,0x0000); 
		//
		LCD_WriteRegData(0x0090,0x0010); 
		LCD_WriteRegData(0x0092,0x0600); 
		
		LCD_WriteRegData(0x0007,0x0133);
		LCD_WriteRegData(0x00,0x0022);//
	}else if(lcddev.id==0x9328)//ILI9328   OK  
	{
  		LCD_WriteRegData(0x00EC,0x108F);// internal timeing      
 		LCD_WriteRegData(0x00EF,0x1234);// ADD        
		//LCD_WriteRegData(0x00e7,0x0010);      
        //LCD_WriteRegData(0x0000,0x0001);//开启内部时钟
        LCD_WriteRegData(0x0001,0x0100);     
        LCD_WriteRegData(0x0002,0x0700);//电源开启                    
		//LCD_WriteRegData(0x0003,(1<<3)|(1<<4) ); 	//65K  RGB
		//DRIVE TABLE(寄存器 03H)
		//BIT3=AM BIT4:5=ID0:1
		//AM ID0 ID1   FUNCATION
		// 0  0   0	   R->L D->U
		// 1  0   0	   D->U	R->L
		// 0  1   0	   L->R D->U
		// 1  1   0    D->U	L->R
		// 0  0   1	   R->L U->D
		// 1  0   1    U->D	R->L
		// 0  1   1    L->R U->D 正常就用这个.
		// 1  1   1	   U->D	L->R
        LCD_WriteRegData(0x0003,(1<<12)|(3<<4)|(0<<3) );//65K    
        LCD_WriteRegData(0x0004,0x0000);                                   
        LCD_WriteRegData(0x0008,0x0202);	           
        LCD_WriteRegData(0x0009,0x0000);         
        LCD_WriteRegData(0x000a,0x0000);//display setting         
        LCD_WriteRegData(0x000c,0x0001);//display setting          
        LCD_WriteRegData(0x000d,0x0000);//0f3c          
        LCD_WriteRegData(0x000f,0x0000);
		//电源配置
        LCD_WriteRegData(0x0010,0x0000);   
        LCD_WriteRegData(0x0011,0x0007);
        LCD_WriteRegData(0x0012,0x0000);                                                                 
        LCD_WriteRegData(0x0013,0x0000);                 
     	LCD_WriteRegData(0x0007,0x0001);                 
       	delay_ms(50); 
        LCD_WriteRegData(0x0010,0x1490);   
        LCD_WriteRegData(0x0011,0x0227);
        delay_ms(50); 
        LCD_WriteRegData(0x0012,0x008A);                  
        delay_ms(50); 
        LCD_WriteRegData(0x0013,0x1a00);   
        LCD_WriteRegData(0x0029,0x0006);
        LCD_WriteRegData(0x002b,0x000d);
        delay_ms(50); 
        LCD_WriteRegData(0x0020,0x0000);                                                            
        LCD_WriteRegData(0x0021,0x0000);           
		delay_ms(50); 
		//伽马校正
        LCD_WriteRegData(0x0030,0x0000); 
        LCD_WriteRegData(0x0031,0x0604);   
        LCD_WriteRegData(0x0032,0x0305);
        LCD_WriteRegData(0x0035,0x0000);
        LCD_WriteRegData(0x0036,0x0C09); 
        LCD_WriteRegData(0x0037,0x0204);
        LCD_WriteRegData(0x0038,0x0301);        
        LCD_WriteRegData(0x0039,0x0707);     
        LCD_WriteRegData(0x003c,0x0000);
        LCD_WriteRegData(0x003d,0x0a0a);
        delay_ms(50); 
        LCD_WriteRegData(0x0050,0x0000); //水平GRAM起始位置 
        LCD_WriteRegData(0x0051,0x00ef); //水平GRAM终止位置                    
        LCD_WriteRegData(0x0052,0x0000); //垂直GRAM起始位置                    
        LCD_WriteRegData(0x0053,0x013f); //垂直GRAM终止位置  
 
         LCD_WriteRegData(0x0060,0xa700);        
        LCD_WriteRegData(0x0061,0x0001); 
        LCD_WriteRegData(0x006a,0x0000);
        LCD_WriteRegData(0x0080,0x0000);
        LCD_WriteRegData(0x0081,0x0000);
        LCD_WriteRegData(0x0082,0x0000);
        LCD_WriteRegData(0x0083,0x0000);
        LCD_WriteRegData(0x0084,0x0000);
        LCD_WriteRegData(0x0085,0x0000);
      
        LCD_WriteRegData(0x0090,0x0010);     
        LCD_WriteRegData(0x0092,0x0600);  
        //开启显示设置    
        LCD_WriteRegData(0x0007,0x0133); 
	}else if(lcddev.id==0x9320)//测试OK.
	{
		LCD_WriteRegData(0x00,0x0000);
		LCD_WriteRegData(0x01,0x0100);	//Driver Output Contral.
		LCD_WriteRegData(0x02,0x0700);	//LCD Driver Waveform Contral.
		LCD_WriteRegData(0x03,0x1030);//Entry Mode Set.
		//LCD_WriteRegData(0x03,0x1018);	//Entry Mode Set.
	
		LCD_WriteRegData(0x04,0x0000);	//Scalling Contral.
		LCD_WriteRegData(0x08,0x0202);	//Display Contral 2.(0x0207)
		LCD_WriteRegData(0x09,0x0000);	//Display Contral 3.(0x0000)
		LCD_WriteRegData(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		LCD_WriteRegData(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		LCD_WriteRegData(0x0d,0x0000);	//Frame Maker Position.
		LCD_WriteRegData(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
		delay_ms(50); 
		LCD_WriteRegData(0x07,0x0101);	//Display Contral.
		delay_ms(50); 								  
		LCD_WriteRegData(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		LCD_WriteRegData(0x11,0x0007);								//Power Control 2.(0x0001)
		LCD_WriteRegData(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
		LCD_WriteRegData(0x13,0x0b00);								//Power Control 4.
		LCD_WriteRegData(0x29,0x0000);								//Power Control 7.
	
		LCD_WriteRegData(0x2b,(1<<14)|(1<<4));	    
		LCD_WriteRegData(0x50,0);	//Set X Star
		//水平GRAM终止位置Set X End.
		LCD_WriteRegData(0x51,239);	//Set Y Star
		LCD_WriteRegData(0x52,0);	//Set Y End.t.
		LCD_WriteRegData(0x53,319);	//
	
		LCD_WriteRegData(0x60,0x2700);	//Driver Output Control.
		LCD_WriteRegData(0x61,0x0001);	//Driver Output Control.
		LCD_WriteRegData(0x6a,0x0000);	//Vertical Srcoll Control.
	
		LCD_WriteRegData(0x80,0x0000);	//Display Position? Partial Display 1.
		LCD_WriteRegData(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		LCD_WriteRegData(0x82,0x0000);	//RAM Address End-Partial Display 1.
		LCD_WriteRegData(0x83,0x0000);	//Displsy Position? Partial Display 2.
		LCD_WriteRegData(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		LCD_WriteRegData(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		LCD_WriteRegData(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		LCD_WriteRegData(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		LCD_WriteRegData(0x93,0x0001);	//Panel Interface Contral 3.
		LCD_WriteRegData(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		LCD_WriteRegData(0x97,(0<<8));	//
		LCD_WriteRegData(0x98,0x0000);	//Frame Cycle Contral.	   
		LCD_WriteRegData(0x07,0x0173);	//(0x0173)
	}else if(lcddev.id==0X9331)//OK |/|/|			 
	{
		LCD_WriteRegData(0x00E7, 0x1014);
		LCD_WriteRegData(0x0001, 0x0100); // set SS and SM bit
		LCD_WriteRegData(0x0002, 0x0200); // set 1 line inversion
        LCD_WriteRegData(0x0003,(1<<12)|(3<<4)|(1<<3));//65K    
		//LCD_WriteRegData(0x0003, 0x1030); // set GRAM write direction and BGR=1.
		LCD_WriteRegData(0x0008, 0x0202); // set the back porch and front porch
		LCD_WriteRegData(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
		LCD_WriteRegData(0x000A, 0x0000); // FMARK function
		LCD_WriteRegData(0x000C, 0x0000); // RGB interface setting
		LCD_WriteRegData(0x000D, 0x0000); // Frame marker Position
		LCD_WriteRegData(0x000F, 0x0000); // RGB interface polarity
		//*************Power On sequence ****************//
		LCD_WriteRegData(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
		LCD_WriteRegData(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
		LCD_WriteRegData(0x0012, 0x0000); // VREG1OUT voltage
		LCD_WriteRegData(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
		delay_ms(200); // Dis-charge capacitor power voltage
		LCD_WriteRegData(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
		LCD_WriteRegData(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
		delay_ms(50); // Delay 50ms
		LCD_WriteRegData(0x0012, 0x000C); // Internal reference voltage= Vci;
		delay_ms(50); // Delay 50ms
		LCD_WriteRegData(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
		LCD_WriteRegData(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
		LCD_WriteRegData(0x002B, 0x000B); // Set Frame Rate
		delay_ms(50); // Delay 50ms
		LCD_WriteRegData(0x0020, 0x0000); // GRAM horizontal Address
		LCD_WriteRegData(0x0021, 0x013f); // GRAM Vertical Address
		// ----------- Adjust the Gamma Curve ----------//
		LCD_WriteRegData(0x0030, 0x0000);
		LCD_WriteRegData(0x0031, 0x0106);
		LCD_WriteRegData(0x0032, 0x0000);
		LCD_WriteRegData(0x0035, 0x0204);
		LCD_WriteRegData(0x0036, 0x160A);
		LCD_WriteRegData(0x0037, 0x0707);
		LCD_WriteRegData(0x0038, 0x0106);
		LCD_WriteRegData(0x0039, 0x0707);
		LCD_WriteRegData(0x003C, 0x0402);
		LCD_WriteRegData(0x003D, 0x0C0F);
		//------------------ Set GRAM area ---------------//
		LCD_WriteRegData(0x0050, 0x0000); // Horizontal GRAM Start Address
		LCD_WriteRegData(0x0051, 0x00EF); // Horizontal GRAM End Address
		LCD_WriteRegData(0x0052, 0x0000); // Vertical GRAM Start Address
		LCD_WriteRegData(0x0053, 0x013F); // Vertical GRAM Start Address
		LCD_WriteRegData(0x0060, 0x2700); // Gate Scan Line
		LCD_WriteRegData(0x0061, 0x0001); // NDL,VLE, REV 
		LCD_WriteRegData(0x006A, 0x0000); // set scrolling line
		//-------------- Partial Display Control ---------//
		LCD_WriteRegData(0x0080, 0x0000);
		LCD_WriteRegData(0x0081, 0x0000);
		LCD_WriteRegData(0x0082, 0x0000);
		LCD_WriteRegData(0x0083, 0x0000);
		LCD_WriteRegData(0x0084, 0x0000);
		LCD_WriteRegData(0x0085, 0x0000);
		//-------------- Panel Control -------------------//
		LCD_WriteRegData(0x0090, 0x0010);
		LCD_WriteRegData(0x0092, 0x0600);
		LCD_WriteRegData(0x0007, 0x0133); // 262K color and display ON
	}	 
	LCD_Display_Dir(0);		 	//默认为竖屏
	//LCD_LED=1;					//点亮背光
	LCD_Clear(WHITE);
}  
//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	LCD_SetCursor(0x00,0x0000);	//设置光标位置 
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;	
	}
}  
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	u16 temp;
	if((lcddev.id==0X6804)&&(lcddev.dir==1))	//6804横屏的时候特殊处理  
	{
		temp=sx;
		sx=sy;
		sy=lcddev.width-ex-1;	  
		ex=ey;
		ey=lcddev.width-temp-1;
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		LCD_Fill(sx,sy,ex,ey,color);  
 		lcddev.dir=1;	 
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}else
	{
		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
		 	LCD_SetCursor(sx,i);      				//设置光标位置 
			LCD_WriteRAM_Prepare();     			//开始写入GRAM	  
			for(j=0;j<xlen;j++)LCD->LCD_RAM=color;	//显示颜色 	    
		}
	}	 
}  
//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j];//写入数据 
	}		  
}  
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{
		LCD_Fast_DrawPoint(uRow,uCol,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_Fast_DrawPoint(x0+a,y0-b,color);             //5
 		LCD_Fast_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_Fast_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_Fast_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_Fast_DrawPoint(x0-a,y0+b,color);             //1       
 		LCD_Fast_DrawPoint(x0-b,y0+a,color);             
		LCD_Fast_DrawPoint(x0-a,y0-b,color);             //2             
  		LCD_Fast_DrawPoint(x0-b,y0-a,color);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}



void Draw_Test(void)
{
	
	LCD_Clear(WHITE); //清屏

	LCD_DrawLine(20,64, 220,128,RED);//划线函数
	LCD_DrawLine(20,128, 220,64,RED);//划线函数

	LCD_DrawRectangle(20,64,220,128, BLUE);//绘制方形状

	LCD_Draw_Circle(120,96,81, BRED);//绘制圆形
	LCD_Draw_Circle(120,96,80, BRED);//绘制圆形
	LCD_Draw_Circle(120,96,79, BRED);//绘制圆形
		
	delay_ms(1000);//延时

}
void Color_Test(void)
{
	LCD_Clear(GRAY0); //清屏

	//LCD_ShowString(24,16,BLUE,"1: 颜色填充测试");

	LCD_Fill(5,5,lcddev.width-5,lcddev.height-5,RED);//设置一个窗口，写入指定区域颜色

	LCD_Fill(20,20,lcddev.width-20,lcddev.height-20,YELLOW);//设置一个窗口，写入指定区域颜色

	LCD_Fill(50,50,lcddev.width-50,lcddev.height-50,BLUE);//设置一个窗口，写入指定区域颜色

	LCD_Fill(80,100,lcddev.width-80,lcddev.height-80,MAGENTA);//设置一个窗口，写入指定区域颜色

	delay_ms(1000);

	LCD_Clear(WHITE);

	Draw_Test();//绘图函数测试

	delay_ms(500);//延时
	LCD_Clear(BLACK);
	delay_ms(500);//延时
	LCD_Clear(RED);
	delay_ms(500);//延时
	LCD_Clear(YELLOW);
	delay_ms(500);//延时
	LCD_Clear(GREEN);
	delay_ms(500);//延时
	LCD_Clear(BLUE);
	delay_ms(500);//延时
}

void Show_Picture(u8 *Pic)
{
	u32 i,j,k=0;	
	u16 picH,picL;
	
	//Draw_Font16B(24,16,BLUE,"3: 图片显示测试");		
	//delay_ms(100);    //延时显示	
	LCD_Clear(WHITE);  //清屏	
	#if USE_HORIZONTAL==1	//使用横屏	
	{
		
		LCD_Set_Window(0,0,320,240);//设置一个自动换行显示区域窗口
		LCD_WriteRAM_Prepare();     	//开始写入GRAM							
		for(i=0;i<240;i++)
		for(j=0;j<320;j++)
		{
			picH=Pic[k++];
			picL=Pic[k++];//			
//			
//			LCD_WR_DATA8(picH);  //写8位显示数据
//			LCD_WR_DATA8(picL);			
			LCD->LCD_RAM=(picH<<8)+picL; 			
		}			
	}			
	#else		
	{
		
		LCD_Set_Window(0,0,240,320);//设置一个自动换行显示区域窗口
		LCD_WriteRAM_Prepare();     	//开始写入GRAM							
		for(i=0;i<320;i++)
			for(j=0;j<240;j++)
			{
				picH=Pic[k++];
				picL=Pic[k++];
	//			
	//			LCD_WR_DATA8(picH);  //写8位显示数据
	//			LCD_WR_DATA8(picL);
				LCD->LCD_RAM=(picH<<8)+picL; 
			}	
	}

	
	#endif
	
} 

void LCD_test()
{
	while(1){
		LCD_Clear(WHITE);  //清屏	
		LCD_ShowString(50,50,40,40,16,"KGZ\n");
		delay_ms(1000);
		Color_Test();
		Show_Picture(gImage_LCD280);
		delay_ms(1000);
	}

}





























