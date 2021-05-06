/*****************************************************************************
* | File      	:   OLED_0in91.c
* | Author      :   Waveshare team
* | Function    :   0.91inch OLED Module Drive function
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2020-08-17
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "OLED_0in91.h"
#include "stdio.h"
/*******************************************************************************
function:
			Hardware reset
*******************************************************************************/
static void OLED_Reset(void)
{
    OLED_RST_1;
    delay_ms(100);
    OLED_RST_0;
    delay_ms(100);
    OLED_RST_1;
    delay_ms(100);
}

/*******************************************************************************
function:
			Write register address and data
*******************************************************************************/
static void OLED_WriteReg(uint8_t Reg)
{
#ifdef USE_IIC
    I2C_Write_Byte(Reg,IIC_CMD);
#endif
}

static void OLED_WriteData(uint8_t Data)
{	
#ifdef USE_IIC
    I2C_Write_Byte(Data,IIC_RAM);
#endif
}

/*******************************************************************************
function:
			Common register initialization
*******************************************************************************/
static void OLED_InitReg(void)
{
    OLED_WriteReg(0xAE);

    OLED_WriteReg(0x40);//---set low column address
    OLED_WriteReg(0xB0);//---set high column address

    OLED_WriteReg(0xC8);//-not offset

    OLED_WriteReg(0x81);
    OLED_WriteReg(0xff);

    OLED_WriteReg(0xa1);

    OLED_WriteReg(0xa6);

    OLED_WriteReg(0xa8);
    OLED_WriteReg(0x1f);

    OLED_WriteReg(0xd3);
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xd5);
    OLED_WriteReg(0xf0);

    OLED_WriteReg(0xd9);
    OLED_WriteReg(0x22);

    OLED_WriteReg(0xda);
    OLED_WriteReg(0x02);

    OLED_WriteReg(0xdb);
    OLED_WriteReg(0x49);

    OLED_WriteReg(0x8d);
    OLED_WriteReg(0x14);
}


/********************************************************************************
function:
			initialization
********************************************************************************/
void OLED_0in91_Init()
{
    //Hardware reset
    OLED_Reset();

    //Set the initialization register
    OLED_InitReg();
    delay_ms(200);

    //Turn on the OLED display
    OLED_WriteReg(0xaf);
}


/********************************************************************************
function:
			Clear screen
********************************************************************************/
void OLED_0in91_Clear()
{
    UBYTE Column,Page;
    for(Page = 0; Page < OLED_0in91_HEIGHT/8; Page++) {
        OLED_WriteReg(0xb0 + Page);    //Set page address
        OLED_WriteReg(0x00);           //Set display position - column low address
        OLED_WriteReg(0x10);           //Set display position - column high address
        for(Column = 0; Column < OLED_0in91_WIDTH; Column++)
            OLED_WriteData(0x00);
    }
}

/********************************************************************************
function:	
			Update all memory to OLED
********************************************************************************/
void OLED_0in91_Display(const UBYTE *Image)
{		
    UBYTE Column,Page;
    UWORD temp;
    for(Page = 0; Page < OLED_0in91_HEIGHT/8; Page++) {
        OLED_WriteReg(0xb0 + Page);
        OLED_WriteReg(0x00);
        OLED_WriteReg(0x10);
        for(Column = 0; Column < OLED_0in91_WIDTH; Column++) {
            temp = Image[(3-Page) + Column*4];
            OLED_WriteData(temp);
        }
    }   
}



int OLED_0in91_test(void)
{
	printf("0.91inch OLED test demo\n");
	printf("USE_IIC\r\n");
	OLED_CS_0;
	OLED_DC_0;//DC = 0,1 >> Address = 0x3c,0x3d 
	
#ifdef USE_SPI_4W
	printf("Only USE_IIC, Please revise DEV_Config.h !!!\r\n");
	return -1;
#endif
	
	printf("OLED Init...\r\n");
	OLED_0in91_Init();
	delay_ms(500);	
	OLED_0in91_Clear();

	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((OLED_0in91_WIDTH%8==0)? (OLED_0in91_WIDTH/8): (OLED_0in91_WIDTH/8+1)) * OLED_0in91_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
			printf("Failed to apply for black memory...\r\n");
			return -1;
	}
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, OLED_0in91_HEIGHT, OLED_0in91_WIDTH, 90, BLACK);	
	
	printf("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	delay_ms(500);
	Paint_Clear(BLACK);
	while(1) {
		// 2.Drawing on the image		
		printf("Drawing:page 1\r\n");
		Paint_DrawPoint(15, 10, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
		Paint_DrawPoint(25, 10, WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
		Paint_DrawPoint(35, 10, WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
		Paint_DrawLine(10, 10, 10, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(20, 10, 20, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(30, 10, 30, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawLine(40, 10, 40, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawCircle(70, 16, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawCircle(110, 16, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);			
		Paint_DrawRectangle(60, 6, 80, 26, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawRectangle(100, 6, 120, 26, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);		
		// 3.Show image on page1
		OLED_0in91_Display(BlackImage);
		delay_ms(2000);			
		Paint_Clear(BLACK);
		
		// Drawing on the image
		printf("Drawing:page 2\r\n");			
		Paint_DrawString_EN(10, 0, "waveshare", &Font16, WHITE, WHITE);
		Paint_DrawNum(10, 18, 123.456789, &Font12, 4, WHITE, WHITE);
		// Show image on page2
		OLED_0in91_Display(BlackImage);
		delay_ms(2000);	
		Paint_Clear(BLACK);		

		// Drawing on the image
		printf("Drawing:page 3\r\n");			
        Paint_DrawString_CN(0, 0,"你好Ab", &Font12CN, WHITE, WHITE);
		// Show image on page3
		OLED_0in91_Display(BlackImage);
		delay_ms(2000);	
		Paint_Clear(BLACK);	

		// Drawing on the image
		printf("Drawing:page 4\r\n");			
		OLED_0in91_Display(gImage_0in91);
		delay_ms(2000);	
		Paint_Clear(BLACK);		
		

	}
}


