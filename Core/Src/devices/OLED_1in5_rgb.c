/*****************************************************************************
* | File      	:   OLED_1in5_rgb.c
* | Author      :   Waveshare team
* | Function    :   1.5inch OLED Module Drive function
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
#include "OLED_1in5_rgb.h"
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
}

static void OLED_WriteData(uint8_t Data)
{   
#ifdef USE_SPI_4W
    OLED_DC_1;
    OLED_CS_0;
    SPI4W_Write_Byte(Data);
    OLED_CS_1;
#endif
}

/*******************************************************************************
function:
        Common register initialization
*******************************************************************************/
static void OLED_InitReg(void)
{
    OLED_WriteReg(0xfd);  // command lock
    OLED_WriteData(0x12);
    OLED_WriteReg(0xfd);  // command lock
    OLED_WriteData(0xB1);

    OLED_WriteReg(0xae);  // display off
    OLED_WriteReg(0xa4);  // Normal Display mode

    OLED_WriteReg(0x15);  //set column address
    OLED_WriteData(0x00);     //column address start 00
    OLED_WriteData(0x7f);     //column address end 127
    OLED_WriteReg(0x75);  //set row address
    OLED_WriteData(0x00);     //row address start 00
    OLED_WriteData(0x7f);     //row address end 127    

    OLED_WriteReg(0xB3);
    OLED_WriteData(0xF1);

    OLED_WriteReg(0xCA);  
    OLED_WriteData(0x7F);

    OLED_WriteReg(0xa0);  //set re-map & data format
    OLED_WriteData(0x74);     //Horizontal address increment

    OLED_WriteReg(0xa1);  //set display start line
    OLED_WriteData(0x00);     //start 00 line

    OLED_WriteReg(0xa2);  //set display offset
    OLED_WriteData(0x00);

    OLED_WriteReg(0xAB);  
    OLED_WriteReg(0x01);  

    OLED_WriteReg(0xB4);  
    OLED_WriteData(0xA0);   
    OLED_WriteData(0xB5);  
    OLED_WriteData(0x55);    

    OLED_WriteReg(0xC1);  
    OLED_WriteData(0xC8); 
    OLED_WriteData(0x80);
    OLED_WriteData(0xC0);

    OLED_WriteReg(0xC7);  
    OLED_WriteData(0x0F);

    OLED_WriteReg(0xB1);  
    OLED_WriteData(0x32);

    OLED_WriteReg(0xB2);  
    OLED_WriteData(0xA4);
    OLED_WriteData(0x00);
    OLED_WriteData(0x00);

    OLED_WriteReg(0xBB);  
    OLED_WriteData(0x17);

    OLED_WriteReg(0xB6);
    OLED_WriteData(0x01);

    OLED_WriteReg(0xBE);
    OLED_WriteData(0x05);

    OLED_WriteReg(0xA6);
}

/********************************************************************************
function:
            initialization
********************************************************************************/
void OLED_1in5_rgb_Init(void)
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
function:
            Clear screen
********************************************************************************/
void OLED_1in5_rgb_Clear(void)
{
    UWORD i;

    OLED_WriteReg(0x15);
    OLED_WriteData(0);
    OLED_WriteData(127);
    OLED_WriteReg(0x75);
    OLED_WriteData(0);
    OLED_WriteData(127);
    // fill!
    OLED_WriteReg(0x5C);

    for(i=0; i<OLED_1in5_RGB_WIDTH*OLED_1in5_RGB_HEIGHT*2; i++){
        OLED_WriteData(0x00);
    }
}

/********************************************************************************
function:   Update all memory to OLED
********************************************************************************/
void OLED_1in5_rgb_Display(const UBYTE *Image)
{
    UWORD i, j, temp;

    OLED_WriteReg(0x15);
    OLED_WriteData(0);
    OLED_WriteData(127);
    OLED_WriteReg(0x75);
    OLED_WriteData(0);
    OLED_WriteData(127);
    // fill!
    OLED_WriteReg(0x5C);   
    
    for(i=0; i<OLED_1in5_RGB_HEIGHT; i++)
        for(j=0; j<OLED_1in5_RGB_WIDTH*2; j++)
        {
            temp = Image[j + i*256];
            OLED_WriteData(temp);
			
        }
}

/********************************************************************************
function:   Update all memory to OLED
********************************************************************************/
void OLED_1in5_rgb_Display_Part(const UBYTE *Image, UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend)
{
    UWORD i, j, temp;

    OLED_WriteReg(0x15);
    OLED_WriteData(Xstart);
    OLED_WriteData(Xend-1);
    OLED_WriteReg(0x75);
    OLED_WriteData(Ystart);
    OLED_WriteData(Yend-1);
    // fill!
    OLED_WriteReg(0x5C);   
    
    for(i=0; i<Yend-Ystart; i++)
        for(j=0; j<(Xend-Xstart)*2; j++)
        {
            temp = Image[j + i*(Xend-Xstart)*2];
            OLED_WriteData(temp);
        }
}


int OLED_1in5_rgb_test(void)
{
	printf("1.5inch RGB OLED test demo\n");
#if defined (USE_IIC) || defined (USE_IIC_SOFT) 
	printf("Only USE_SPI_4W, Please revise DEV_Config.h !!!\r\n");
	return -1;
#endif
	
	printf("USE_SPI_4W\r\n");
	
	printf("OLED Init...\r\n");
	OLED_1in5_rgb_Init();
	delay_ms(500);	
	OLED_1in5_rgb_Clear();
	
	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = (OLED_1in5_RGB_WIDTH*2) * OLED_1in5_RGB_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize/4)) == NULL) {
			printf("Failed to apply for black memory...\r\n");
			return -1;
	}
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, OLED_1in5_RGB_WIDTH/2, OLED_1in5_RGB_HEIGHT/2, 270, BLACK);	
	Paint_SetScale(65);
	printf("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	delay_ms(500);
	Paint_Clear(BLACK);
	while(1) {
		
		// 2.Drawing on the image		
		printf("Drawing:page 1\r\n");
		Paint_DrawPoint(5, 5, BLUE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
		Paint_DrawPoint(15, 5, BRED, DOT_PIXEL_2X2, DOT_STYLE_DFT);
		Paint_DrawPoint(25, 5, GRED, DOT_PIXEL_3X3, DOT_STYLE_DFT);
		Paint_DrawLine(5, 10, 5, 20, GBLUE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(10, 20, 10, 30, RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(15, 30, 15, 40, MAGENTA, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawLine(20, 40, 20, 50, GREEN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawCircle(40, 20, 15, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawCircle(50, 50, 12, YELLOW, DOT_PIXEL_1X1, DRAW_FILL_FULL);			
		Paint_DrawRectangle(35, 15, 45, 25, BROWN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawRectangle(45, 45, 55, 55, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);		
		// 3.Show image on page1
		OLED_1in5_rgb_Display_Part(BlackImage, 0, 0, 64, 64);
		delay_ms(2000);			
		Paint_Clear(BLACK);

		// Drawing on the image
		printf("Drawing:page 2\r\n");
		for(UBYTE i=0; i<16; i++){
			Paint_DrawRectangle(0, 4*i, 63, 4*(i+1), i*4095, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		}			
		// Show image on page2
		OLED_1in5_rgb_Display_Part(BlackImage, 0, 0, 64, 64);
		delay_ms(2000);	
		Paint_Clear(BLACK);	
		
		// Drawing on the image
		printf("Drawing:page 3\r\n");			
		Paint_DrawString_EN(0, 0, "waveshare", &Font12, BLACK, BLUE);
		Paint_DrawString_EN(0, 14, "hello world", &Font8, BLACK, MAGENTA);
		Paint_DrawNum(10, 24, 123.459, &Font8, 2, RED, BLACK);
		Paint_DrawNum(10, 34, 9854, &Font12, 1, YELLOW, BLACK);
		// Show image on page2
		OLED_1in5_rgb_Display_Part(BlackImage, 0, 0, 64, 64);
		delay_ms(2000);	
		Paint_Clear(BLACK);		
		
		// Drawing on the image
		printf("Drawing:page 4\r\n");
		Paint_DrawString_CN(0, 0,"你好Ab", &Font12CN, BROWN, BLACK);
		// Show image on page4
		OLED_1in5_rgb_Display_Part(BlackImage, 0, 0, 64, 64);
		delay_ms(2000);		
		Paint_Clear(BLACK);		
		
		// show the array image
		printf("Drawing:page 5\r\n");
		OLED_1in5_rgb_Display(gImage_1in5_rgb);
		delay_ms(2000);		
		Paint_Clear(BLACK);		

		OLED_1in5_rgb_Clear();
	}
}

