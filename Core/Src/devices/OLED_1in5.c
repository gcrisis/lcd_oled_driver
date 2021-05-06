/*****************************************************************************
* | File      	:   OLED_1in5.c
* | Author      :   Waveshare team
* | Function    :   1.5inch OLED Module Drive function
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2020-08-15
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
#include "OLED_1in5.h"
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
#ifdef USE_SPI_4W
	OLED_DC_0;
    OLED_CS_0;
    SPI4W_Write_Byte(Reg);
    OLED_CS_1;
#endif
#ifdef USE_IIC
    I2C_Write_Byte(Reg,IIC_CMD);
#endif
}

static void OLED_WriteData(uint8_t Data)
{   
#ifdef USE_SPI_4W
    OLED_DC_1;
    OLED_CS_0;
    SPI4W_Write_Byte(Data);
    OLED_CS_1;
#endif
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
    OLED_WriteReg(0xae);//--turn off oled panel

    OLED_WriteReg(0x15);    //   set column address
    OLED_WriteReg(0x00);    //  start column   0
    OLED_WriteReg(0x7f);    //  end column   127

    OLED_WriteReg(0x75);    //   set row address
    OLED_WriteReg(0x00);    //  start row   0
    OLED_WriteReg(0x7f);    //  end row   127

    OLED_WriteReg(0x81);  // set contrast control
    OLED_WriteReg(0x80);

    OLED_WriteReg(0xa0);    // gment remap
    OLED_WriteReg(0x51);   //51

    OLED_WriteReg(0xa1);  // start line
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xa2);  // display offset
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xa4);    // rmal display
    OLED_WriteReg(0xa8);    // set multiplex ratio
    OLED_WriteReg(0x7f);

    OLED_WriteReg(0xb1);  // set phase leghth
    OLED_WriteReg(0xf1);

    OLED_WriteReg(0xb3);  // set dclk
    OLED_WriteReg(0x00);  //80Hz:0xc1 90Hz:0xe1   100Hz:0x00   110Hz:0x30 120Hz:0x50   130Hz:0x70     01

    OLED_WriteReg(0xab);  //
    OLED_WriteReg(0x01);  //

    OLED_WriteReg(0xb6);  // set phase leghth
    OLED_WriteReg(0x0f);

    OLED_WriteReg(0xbe);
    OLED_WriteReg(0x0f);

    OLED_WriteReg(0xbc);
    OLED_WriteReg(0x08);

    OLED_WriteReg(0xd5);
    OLED_WriteReg(0x62);

    OLED_WriteReg(0xfd);
    OLED_WriteReg(0x12);

}

/********************************************************************************
function:
            initialization
********************************************************************************/
void OLED_1in5_Init(void)
{
    //Hardware reset
    OLED_Reset();

    //Set the initialization register
    OLED_InitReg();
    delay_ms(200);

    //Turn on the OLED display
    OLED_WriteReg(0xAF);
}

/********************************************************************************
function:   Set the display Window(Xstart, Ystart, Xend, Yend)
parameter:
        xStart :   X direction Start coordinates
        Ystart :   Y direction Start coordinates
        Xend   :   X direction end coordinates
        Yend   :   Y direction end coordinates
********************************************************************************/
static void OLED_SetWindow(UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend)
{
    if((Xstart > OLED_1in5_WIDTH) || (Ystart > OLED_1in5_HEIGHT) ||
       (Xend > OLED_1in5_WIDTH) || (Yend > OLED_1in5_HEIGHT))
        return;

    OLED_WriteReg(0x15);
    OLED_WriteReg(Xstart/2);
    OLED_WriteReg(Xend/2 - 1);

    OLED_WriteReg(0x75);
    OLED_WriteReg(Ystart);
    OLED_WriteReg(Yend - 1);
}

/********************************************************************************
function:
            Clear screen
********************************************************************************/
void OLED_1in5_Clear(void)
{
    UWORD i;
    OLED_SetWindow(0, 0, 128, 128);
    for(i=0; i<OLED_1in5_WIDTH*OLED_1in5_HEIGHT/2; i++){
        OLED_WriteData(0x00);
    }
}

/********************************************************************************
function:   Update all memory to OLED
********************************************************************************/
void OLED_1in5_Display(const UBYTE *Image)
{
    UWORD i, j, temp;
    OLED_SetWindow(0, 0, 128, 128);
    for(i=0; i<OLED_1in5_HEIGHT; i++)
        for(j=0; j<OLED_1in5_WIDTH/2; j++)
        {
            temp = Image[j + i*64];
            OLED_WriteData(temp);
        }
}


int OLED_1in5_test(void)
{
	printf("1.5inch OLED test demo\n");
#ifdef USE_SPI_4W
	printf("USE_SPI_4W\r\n");
#endif
#ifdef USE_IIC
	printf("USE_IIC\r\n");
	OLED_CS_0;
	OLED_DC_0;//DC = 0,1 >> Address = 0x3c,0x3d 
#endif
	  
	printf("OLED Init...\r\n");
	OLED_1in5_Init();
	delay_ms(500);
	OLED_1in5_Clear();
	
	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((OLED_1in5_WIDTH%2==0)? (OLED_1in5_WIDTH/2): (OLED_1in5_WIDTH/2+1)) * OLED_1in5_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
			printf("Failed to apply for black memory...\r\n");
			return -1;
	}
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, OLED_1in5_WIDTH, OLED_1in5_HEIGHT, 270, BLACK);	
	Paint_SetScale(16);
	printf("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	delay_ms(500);
	Paint_Clear(BLACK);
	while(1) {
		
		// 2.Drawing on the image		
		printf("Drawing:page 1\r\n");
		Paint_DrawPoint(20, 10, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
		Paint_DrawPoint(30, 10, WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
		Paint_DrawPoint(40, 10, WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
		Paint_DrawLine(10, 10, 10, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(20, 20, 20, 30, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(30, 30, 30, 40, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawLine(40, 40, 40, 50, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawCircle(60, 30, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawCircle(100, 40, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);			
		Paint_DrawRectangle(50, 30, 60, 40, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawRectangle(90, 30, 110, 50, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);		
		// 3.Show image on page1
		OLED_1in5_Display(BlackImage);
		delay_ms(2000);			
		Paint_Clear(BLACK);

		// Drawing on the image
		printf("Drawing:page 2\r\n");
		for(UBYTE i=0; i<16; i++){
			Paint_DrawRectangle(0, 8*i, 127, 8*(i+1), i, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		}			
		// Show image on page2
		OLED_1in5_Display(BlackImage);
		delay_ms(2000);	
		Paint_Clear(BLACK);	
		
		// Drawing on the image
		printf("Drawing:page 3\r\n");			
		Paint_DrawString_EN(10, 0, "waveshare", &Font16, 0x1, 0xb);
		Paint_DrawString_EN(10, 17, "hello world", &Font8, 0x2, 0xc);
		Paint_DrawNum(10, 30, 123.456789, &Font8, 4, 0x3, 0xd);
		Paint_DrawNum(10, 43, 987654, &Font12, 5, 0x4, 0xe);
		// Show image on page2
		OLED_1in5_Display(BlackImage);
		delay_ms(2000);	
		Paint_Clear(BLACK);		
		
		// Drawing on the image
		printf("Drawing:page 4\r\n");
		Paint_DrawString_CN(10, 0,"你好Abc", &Font12CN, WHITE, WHITE);
		Paint_DrawString_CN(0, 20, "微雪电子", &Font24CN, WHITE, WHITE);
		// Show image on page3
		OLED_1in5_Display(BlackImage);
		delay_ms(2000);		
		Paint_Clear(BLACK);		

		// show the array image
		printf("Drawing:page 5\r\n");
		OLED_1in5_Display(gImage_1in5);
		delay_ms(2000);		
		Paint_Clear(BLACK);	

	}
}

