/*****************************************************************************
* | File      	:   OLED_0in95_rgb.c
* | Author      :   Waveshare team
* | Function    :   0.95inch RGB OLED Module Drive function
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
#include "OLED_0in95_rgb.h"
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
    OLED_WriteReg(DISPLAY_OFF);          //Display Off
    OLED_WriteReg(SET_CONTRAST_A);       //Set contrast for color A
    OLED_WriteReg(0xFF);                     //145 0x91
    OLED_WriteReg(SET_CONTRAST_B);       //Set contrast for color B
    OLED_WriteReg(0xFF);                     //80 0x50
    OLED_WriteReg(SET_CONTRAST_C);       //Set contrast for color C
    OLED_WriteReg(0xFF);                     //125 0x7D
    OLED_WriteReg(MASTER_CURRENT_CONTROL);//master current control
    OLED_WriteReg(0x06);                     //6
    OLED_WriteReg(SET_PRECHARGE_SPEED_A);//Set Second Pre-change Speed For ColorA
    OLED_WriteReg(0x64);                     //100
    OLED_WriteReg(SET_PRECHARGE_SPEED_B);//Set Second Pre-change Speed For ColorB
    OLED_WriteReg(0x78);                     //120
    OLED_WriteReg(SET_PRECHARGE_SPEED_C);//Set Second Pre-change Speed For ColorC
    OLED_WriteReg(0x64);                     //100
    OLED_WriteReg(SET_REMAP);            //set remap & data format
    OLED_WriteReg(0x72);                      //normal 0x72   180 0x60 
    OLED_WriteReg(SET_DISPLAY_START_LINE);//Set display Start Line
    OLED_WriteReg(0x0);
    OLED_WriteReg(SET_DISPLAY_OFFSET);   //Set display offset
    OLED_WriteReg(0x0);
    OLED_WriteReg(NORMAL_DISPLAY);       //Set display mode
    OLED_WriteReg(SET_MULTIPLEX_RATIO);  //Set multiplex ratio
    OLED_WriteReg(0x3F);
    OLED_WriteReg(SET_MASTER_CONFIGURE); //Set master configuration
    OLED_WriteReg(0x8E);
    OLED_WriteReg(POWER_SAVE_MODE);      //Set Power Save Mode
    OLED_WriteReg(0x00);                     //0x00
    OLED_WriteReg(PHASE_PERIOD_ADJUSTMENT);//phase 1 and 2 period adjustment
    OLED_WriteReg(0x31);                     //0x31
    OLED_WriteReg(DISPLAY_CLOCK_DIV);    //display clock divider/oscillator frequency
    OLED_WriteReg(0xF0);
    OLED_WriteReg(SET_PRECHARGE_VOLTAGE);//Set Pre-Change Level
    OLED_WriteReg(0x3A);
    OLED_WriteReg(SET_V_VOLTAGE);        //Set vcomH
    OLED_WriteReg(0x3E);
    OLED_WriteReg(DEACTIVE_SCROLLING);   //disable scrolling
}

/********************************************************************************
function:
            initialization
********************************************************************************/
void OLED_0in95_rgb_Init(void)
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
void OLED_0in95_rgb_Clear(void)
{
    UWORD i;

    OLED_WriteReg(SET_COLUMN_ADDRESS);
    OLED_WriteReg(0);         //cloumn start address
    OLED_WriteReg(OLED_0in95_RGB_WIDTH - 1); //cloumn end address
    OLED_WriteReg(SET_ROW_ADDRESS);
    OLED_WriteReg(0);         //page atart address
    OLED_WriteReg(OLED_0in95_RGB_HEIGHT - 1); //page end address  

    for(i=0; i<OLED_0in95_RGB_WIDTH*OLED_0in95_RGB_HEIGHT*2; i++){
        OLED_WriteData(0x00);
    }
}

/********************************************************************************
function:   Update all memory to OLED
********************************************************************************/
void OLED_0in95_rgb_Display(const UBYTE *Image)
{
    UWORD i, j, temp;

    OLED_WriteReg(SET_COLUMN_ADDRESS);
    OLED_WriteReg(0);         //cloumn start address
    OLED_WriteReg(OLED_0in95_RGB_WIDTH - 1); //cloumn end address
    OLED_WriteReg(SET_ROW_ADDRESS);
    OLED_WriteReg(0);         //page atart address
    OLED_WriteReg(OLED_0in95_RGB_HEIGHT - 1); //page end address  
     
    for(i=0; i<OLED_0in95_RGB_HEIGHT; i++)
        for(j=0; j<OLED_0in95_RGB_WIDTH*2; j++)
        {
            temp = Image[j + i*OLED_0in95_RGB_WIDTH*2];
            OLED_WriteData(temp);
        }
}



int OLED_0in95_rgb_test(void)
{
	printf("0.95inch RGB OLED test demo\n");
#ifdef USE_IIC|USE_IIC_SOFT
	printf("Only USE_SPI_4W, Please revise DEV_Config.h !!!\r\n");
	return -1;
#endif
	printf("USE_SPI_4W\r\n");
	printf("OLED Init...\r\n");
	OLED_0in95_rgb_Init();
	delay_ms(500);	
	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = (OLED_0in95_RGB_WIDTH*2) * OLED_0in95_RGB_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
			printf("Failed to apply for black memory...\r\n");
			return -1;
	}
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, OLED_0in95_RGB_WIDTH, OLED_0in95_RGB_HEIGHT, 0, BLACK);	
	Paint_SetScale(65);
	printf("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	delay_ms(500);
	Paint_Clear(BLACK);
	while(1) {
		
		// 2.Drawing on the image		
		printf("Drawing:page 1\r\n");
		Paint_DrawPoint(20, 10, BLUE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
		Paint_DrawPoint(30, 10, BRED, DOT_PIXEL_2X2, DOT_STYLE_DFT);
		Paint_DrawPoint(40, 10, GRED, DOT_PIXEL_3X3, DOT_STYLE_DFT);
		Paint_DrawLine(10, 10, 10, 20, GBLUE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(20, 20, 20, 30, RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(30, 30, 30, 40, MAGENTA, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawLine(40, 40, 40, 50, GREEN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawCircle(60, 30, 15, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);	
		Paint_DrawRectangle(50, 30, 60, 40, BROWN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		// 3.Show image on page1
		OLED_0in95_rgb_Display(BlackImage);
		delay_ms(2000);			
		Paint_Clear(BLACK);

		// Drawing on the image
		printf("Drawing:page 2\r\n");
		for(UBYTE i=0; i<16; i++){
			Paint_DrawRectangle(0, 4*i, 95, 4*(i+1), i*4095, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		}			
		// Show image on page2
		OLED_0in95_rgb_Display(BlackImage);
		delay_ms(2000);	
		Paint_Clear(BLACK);	
		
		// Drawing on the image
		printf("Drawing:page 3\r\n");			
		Paint_DrawString_EN(10, 0, "waveshare", &Font12, BLACK, BLUE);
		Paint_DrawString_EN(10, 17, "hello world", &Font8, BLACK, MAGENTA);
		Paint_DrawNum(10, 30, 123.456789, &Font8, 5, RED, BLACK);
		Paint_DrawNum(10, 43, 987654, &Font12, 4, YELLOW, BLACK);
		// Show image on page3
		OLED_0in95_rgb_Display(BlackImage);
		delay_ms(2000);	
		Paint_Clear(BLACK);		
		
		// Drawing on the image
		printf("Drawing:page 4\r\n");
		Paint_DrawString_CN(20, 0,"���Abc", &Font12CN, BROWN, BLACK);
		Paint_DrawString_CN(20, 20, "΢ѩ", &Font24CN, BRED, BLACK);
		// Show image on page4
		OLED_0in95_rgb_Display(BlackImage);
		delay_ms(2000);		
		Paint_Clear(BLACK);	

		// Drawing on the image
		printf("Drawing:page 5\r\n");
		Paint_DrawString_CN(20, 20, "����", &Font24CN, BRED, BLACK);
		// Show image on page5
		OLED_0in95_rgb_Display(BlackImage);
		delay_ms(2000);		
		Paint_Clear(BLACK);			

		// Drawing on the image
		printf("Drawing:page 6\r\n");
		OLED_0in95_rgb_Display(gImage_0in95_rgb);
		delay_ms(2000);		
		Paint_Clear(BLACK);	
		
		OLED_0in95_rgb_Clear();
	}
}

