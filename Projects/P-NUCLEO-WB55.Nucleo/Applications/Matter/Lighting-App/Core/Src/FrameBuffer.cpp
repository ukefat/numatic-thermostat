/*
 * FrameBuffer.cpp
 *
 *  Created on: Feb 25, 2025
 *      Author: kobru
 */


#include <FrameBuffer.hpp>

#include "DEV_Config.hpp"
#include "Debug.hpp"
#include <stdint.h>
#include <stdlib.h>
#include <string.h> //memset()
#include <memory>
#include "FreeRTOSMemory.hpp"


//#define Imagesize (((EPD_4in26_WIDTH % 8 == 0)? (EPD_4in26_WIDTH / 8 ): (EPD_4in26_WIDTH / 8 + 1)) * EPD_4in26_HEIGHT)

std::unique_ptr<FrameBuffer, FreeRTOSDeleter> FrameBuffer::instance = nullptr;
// !!!!!!!!!!!!!!!!! IMPORTANT -> this width and height dont correlate to the width and height of image it correlates to the
// width and height of the partial iamge you want to draw (usefull when you only want to draw one part of the screen
FrameBuffer::FrameBuffer(UWORD Width, UWORD Height, UWORD Rotate, UWORD Color){
	Paint_initImage(Width,Height,Rotate,Color);
}


/******************************************************************************
function: Create Image
parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/



void FrameBuffer::Paint_initImage(UWORD Width, UWORD Height, UWORD Rotate, UWORD Color){
	UWORD Imagesize  =(((Width % 8 == 0)? (Width / 8 ): (Width / 8 + 1)) * Height);


    this->Image = make_unique_freertos_array(Imagesize);
    if (!this->Image) {
		printf("FrameBuffer allocation failed!\n");
    }
	this->WidthMemory = Width;
    this->HeightMemory = Height;
    this->Color = Color;
	this->Scale = 2;

    this->WidthByte = (Width % 8 == 0)? (Width / 8 ): (Width / 8 + 1);
    this->HeightByte = Height;
//    printf("WidthByte = %d, HeightByte = %d\r\n", this->WidthByte, this->HeightByte);
//    printf(" EPD_WIDTH / 8 = %d\r\n",  122 / 8);

    this->Rotate = Rotate;
    this->Mirror = MIRROR_NONE;

    if(Rotate == ROTATE_0 || Rotate == ROTATE_180) {
        this->Width = Width;
        this->Height = Height;
    } else {
        this->Width = Height;
        this->Height = Width;
    }
}

void FrameBuffer::Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color)
{
//    this->Image = NULL;
//    this->Image = image;
//
//    this->WidthMemory = Width;
//    this->HeightMemory = Height;
//    this->Color = Color;
//	this->Scale = 2;
//
//    this->WidthByte = (Width % 8 == 0)? (Width / 8 ): (Width / 8 + 1);
//    this->HeightByte = Height;
////    printf("WidthByte = %d, HeightByte = %d\r\n", this->WidthByte, this->HeightByte);
////    printf(" EPD_WIDTH / 8 = %d\r\n",  122 / 8);
//
//    this->Rotate = Rotate;
//    this->Mirror = MIRROR_NONE;
//
//    if(Rotate == ROTATE_0 || Rotate == ROTATE_180) {
//        this->Width = Width;
//        this->Height = Height;
//    } else {
//        this->Width = Height;
//        this->Height = Width;
//    }
}

/******************************************************************************
function: Select Image
parameter:
    image : Pointer to the image cache
******************************************************************************/
//void FrameBuffer::Paint_SelectImage(UBYTE *image)
//{
//    this->Image = image;
//}

/******************************************************************************
function: Select Image Rotate
parameter:
    Rotate : 0,90,180,270
******************************************************************************/
void FrameBuffer::Paint_SetRotate(UWORD Rotate)
{
    if(Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180 || Rotate == ROTATE_270) {
        Debug("Set image Rotate %d\r\n", Rotate);
        this->Rotate = Rotate;
    } else {
        Debug("rotate = 0, 90, 180, 270\r\n");
    }
}

//void FrameBuffer::apply(FrameBuffer& fb){
//	Paint_SetPixel
//}


void FrameBuffer::Paint_SetScale(UBYTE scale)
{
    if(scale == 2){
        this->Scale = scale;
        this->WidthByte = (this->WidthMemory % 8 == 0)? (this->WidthMemory / 8 ): (this->WidthMemory / 8 + 1);
    }else if(scale == 4){
        this->Scale = scale;
        this->WidthByte = (this->WidthMemory % 4 == 0)? (this->WidthMemory / 4 ): (this->WidthMemory / 4 + 1);
    }else if(scale == 6 || scale == 7){//Only applicable with 5in65 e-Paper
				this->Scale = scale;
				this->WidthByte = (this->WidthMemory % 2 == 0)? (this->WidthMemory / 2 ): (this->WidthMemory / 2 + 1);;
		}else{
        Debug("Set Scale Input parameter error\r\n");
        Debug("Scale Only support: 2 4 7\r\n");
    }
}
/******************************************************************************
function:	Select Image mirror
parameter:
    mirror   :Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
******************************************************************************/
void FrameBuffer::Paint_SetMirroring(UBYTE mirror)
{
    if(mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL ||
        mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN) {
        Debug("mirror image x:%s, y:%s\r\n",(mirror & 0x01)? "mirror":"none", ((mirror >> 1) & 0x01)? "mirror":"none");
        this->Mirror = mirror;
    } else {
        Debug("mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
    }
}

/******************************************************************************
function: Draw Pixels
parameter:
    Xpoint : At point X
    Ypoint : At point Y
    Color  : Painted colors
******************************************************************************/
void FrameBuffer::Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
{
    if(Xpoint > this->Width || Ypoint > this->Height){
        Debug("Exceeding display boundaries\r\n");
        return;
    }
    UWORD X, Y;

    switch(this->Rotate) {
    case 0:
        X = Xpoint;
        Y = Ypoint;
        break;
    case 90:
        X = this->WidthMemory - Ypoint - 1;
        Y = Xpoint;
        break;
    case 180:
        X = this->WidthMemory - Xpoint - 1;
        Y = this->HeightMemory - Ypoint - 1;
        break;
    case 270:
        X = Ypoint;
        Y = this->HeightMemory - Xpoint - 1;
        break;
    default:
        return;
    }

    switch(this->Mirror) {
    case MIRROR_NONE:
        break;
    case MIRROR_HORIZONTAL:
        X = this->WidthMemory - X - 1;
        break;
    case MIRROR_VERTICAL:
        Y = this->HeightMemory - Y - 1;
        break;
    case MIRROR_ORIGIN:
        X = this->WidthMemory - X - 1;
        Y = this->HeightMemory - Y - 1;
        break;
    default:
        return;
    }

    if(X > this->WidthMemory || Y > this->HeightMemory){
        Debug("Exceeding display boundaries\r\n");
        return;
    }

    if(this->Scale == 2){
        UDOUBLE Addr = X / 8 + Y * this->WidthByte;
        UBYTE Rdata = this->Image[Addr];
        if(Color == BLACK)
            this->Image[Addr] = Rdata & ~(0x80 >> (X % 8));
        else
            this->Image[Addr] = Rdata | (0x80 >> (X % 8));
    }else if(this->Scale == 4){
        UDOUBLE Addr = X / 4 + Y * this->WidthByte;
        Color = Color % 4;//Guaranteed color scale is 4  --- 0~3
        UBYTE Rdata = this->Image[Addr];

        Rdata = Rdata & (~(0xC0 >> ((X % 4)*2)));
        this->Image[Addr] = Rdata | ((Color << 6) >> ((X % 4)*2));
    }else if(this->Scale == 6 || this->Scale == 7){
		UDOUBLE Addr = X / 2  + Y * this->WidthByte;
		UBYTE Rdata = this->Image[Addr];
		Rdata = Rdata & (~(0xF0 >> ((X % 2)*4)));//Clear first, then set value
		this->Image[Addr] = Rdata | ((Color << 4) >> ((X % 2)*4));
		//printf("Add =  %d ,data = %d\r\n",Addr,Rdata);
		}
}

/******************************************************************************
function: Clear the color of the picture
parameter:
    Color : Painted colors
******************************************************************************/
void FrameBuffer::Paint_Clear(UWORD Color)
{
	if(this->Scale == 2) {
		for (UWORD Y = 0; Y < this->HeightByte; Y++) {
			for (UWORD X = 0; X < this->WidthByte; X++ ) {//8 pixel =  1 byte
				UDOUBLE Addr = X + Y*this->WidthByte;
				this->Image[Addr] = Color;
			}
		}
    }else if(this->Scale == 4) {
        for (UWORD Y = 0; Y < this->HeightByte; Y++) {
			for (UWORD X = 0; X < this->WidthByte; X++ ) {
				UDOUBLE Addr = X + Y*this->WidthByte;
				this->Image[Addr] = (Color<<6)|(Color<<4)|(Color<<2)|Color;
			}
		}
	}else if(this->Scale == 6 || this->Scale == 7) {
		for (UWORD Y = 0; Y < this->HeightByte; Y++) {
			for (UWORD X = 0; X < this->WidthByte; X++ ) {
				UDOUBLE Addr = X + Y*this->WidthByte;
				this->Image[Addr] = (Color<<4)|Color;
			}
		}
	}
}

/******************************************************************************
function: Clear the color of a window
parameter:
    Xstart : x starting point
    Ystart : Y starting point
    Xend   : x end point
    Yend   : y end point
    Color  : Painted colors
******************************************************************************/
void FrameBuffer::Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color)
{
    UWORD X, Y;
    for (Y = Ystart; Y < Yend; Y++) {
        for (X = Xstart; X < Xend; X++) {//8 pixel =  1 byte
            Paint_SetPixel(X, Y, Color);
        }
    }
}

/******************************************************************************
function: Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		: The Xpoint coordinate of the point
    Ypoint		: The Ypoint coordinate of the point
    Color		: Painted color
    Dot_Pixel	: point size
    Dot_Style	: point Style
******************************************************************************/
void FrameBuffer::Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color,
                     DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_Style)
{
    if (Xpoint > this->Width || Ypoint > this->Height) {
//        Debug("Paint_DrawPoint Input exceeds the normal display range\r\n");
//				printf("Xpoint = %d , this->Width = %d  \r\n ",Xpoint,this->Width);
//				printf("Ypoint = %d , this->Height = %d  \r\n ",Ypoint,this->Height);
        return;
    }

    int16_t XDir_Num , YDir_Num;
    if (Dot_Style == DOT_FILL_AROUND) {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
                if(Xpoint + XDir_Num - Dot_Pixel < 0 || Ypoint + YDir_Num - Dot_Pixel < 0)
                    break;
                // printf("x = %d, y = %d\r\n", Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
                Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
            }
        }
    } else {
        for (XDir_Num = 0; XDir_Num <  Dot_Pixel; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num <  Dot_Pixel; YDir_Num++) {
                Paint_SetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color);
            }
        }
    }
}

/******************************************************************************
function: Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
    Line_width : Line width
    Line_Style: Solid and dotted lines
******************************************************************************/
void FrameBuffer::Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                    UWORD Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style)
{
    if (Xstart > this->Width || Ystart > this->Height ||
        Xend > this->Width || Yend > this->Height) {
        Debug("Paint_DrawLine Input exceeds the normal display range\r\n");
        return;
    }

    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;
    int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    int XAddway = Xstart < Xend ? 1 : -1;
    int YAddway = Ystart < Yend ? 1 : -1;

    //Cumulative error
    int Esp = dx + dy;
    char Dotted_Len = 0;

    for (;;) {
        Dotted_Len++;
        //Painted dotted line, 2 point is really virtual
        if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0) {
            //Debug("LINE_DOTTED\r\n");
            Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Line_width, DOT_STYLE_DFT);
            Dotted_Len = 0;
        } else {
            Paint_DrawPoint(Xpoint, Ypoint, Color, Line_width, DOT_STYLE_DFT);
        }
        if (2 * Esp >= dy) {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx) {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

/******************************************************************************
function: Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the rectangle
******************************************************************************/
void FrameBuffer::Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                         UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
    if (Xstart > this->Width || Ystart > this->Height ||
        Xend > this->Width || Yend > this->Height) {
        Debug("Input exceeds the normal display range\r\n");
        return;
    }

    if (Draw_Fill) {
        UWORD Ypoint;
        for(Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
            Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color , Line_width, LINE_STYLE_SOLID);
        }
    } else {
        Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xend, Ystart, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xstart, Yend, Color, Line_width, LINE_STYLE_SOLID);
    }
}

/******************************************************************************
function: Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the Circle
******************************************************************************/
void FrameBuffer::Paint_DrawCircle(UWORD X_Center, UWORD Y_Center, UWORD Radius,
                      UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
    if (X_Center > this->Width || Y_Center >= this->Height) {
        Debug("Paint_DrawCircle Input exceeds the normal display range\r\n");
        return;
    }

    //Draw a circle from(0, R) as a starting point
    int16_t XCurrent, YCurrent;
    XCurrent = 0;
    YCurrent = Radius;

    //Cumulative error,judge the next point of the logo
    int16_t Esp = 3 - (Radius << 1 );

    int16_t sCountY;
    if (Draw_Fill == DRAW_FILL_FULL) {
        while (XCurrent <= YCurrent ) { //Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) {
                Paint_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//1
                Paint_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//2
                Paint_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//3
                Paint_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//4
                Paint_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//5
                Paint_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//6
                Paint_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//7
                Paint_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            if (Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    } else { //Draw a hollow circle
        while (XCurrent <= YCurrent ) {
            Paint_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT);//1
            Paint_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT);//2
            Paint_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT);//3
            Paint_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT);//4
            Paint_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT);//5
            Paint_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT);//6
            Paint_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT);//7
            Paint_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT);//0

            if (Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    }
}

/******************************************************************************
function: Show English characters
parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void FrameBuffer::Paint_DrawChar(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char,
                    sFONT* Font, UWORD Color_Foreground, UWORD Color_Background)
{
    UWORD Page, Column;

    if (Xpoint > this->Width || Ypoint > this->Height) {
        Debug("Paint_DrawChar Input exceeds the normal display range\r\n");
        return;
    }

    uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &Font->table[Char_Offset];

    for (Page = 0; Page < Font->Height; Page ++ ) {
        for (Column = 0; Column < Font->Width; Column ++ ) {

            //To determine whether the font background color and screen background color is consistent
            if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
                if (*ptr & (0x80 >> (Column % 8)))
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            } else {
                if (*ptr & (0x80 >> (Column % 8))) {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                } else {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Background);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
            }
            //One pixel is 8 bits
            if (Column % 8 == 7)
                ptr++;
        }// Write a line
        if (Font->Width % 8 != 0)
            ptr++;
    }// Write all
}

static void pixel_callback(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state)
{
    state_t *s = (state_t*)state;
    uint32_t pos;
    int16_t value;

    if (y < 0 || y >= s->height) return;
    if (x < 0 || x + count >= s->width) return;
    if (alpha < 1) return;

    while (count--)
    {
    	s->fb->Paint_SetPixel(x, y, !s->color); // black = 1 now
        x++;
    }
}

static uint8_t character_callback(int16_t x, int16_t y, mf_char character, void *state)
{
    state_t *s = (state_t*)state;
    return mf_render_character(s->font, x, y, character, pixel_callback, state);
}

void FrameBuffer::Paint_DrawString(UWORD Xstart, UWORD Ystart, const char * pString, const char* Font, UWORD Color_Foreground, UWORD Color_Background)
{

	  state_t state = {this, EPD_4in26_WIDTH, EPD_4in26_HEIGHT, 0, nullptr, (bool)Color_Foreground};

	  state.font = mf_find_font(Font);
	  if(!state.font){
		  this->Paint_DrawString_EN(Xstart, Ystart, "Font missing", &Font16, WHITE, BLACK); // white text
		  return;
	  }
	  mf_render_aligned(state.font, Xstart, Ystart,
		  MF_ALIGN_LEFT, pString, strlen(pString),
		  character_callback, (void*)&state);

}

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the English string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void FrameBuffer::Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString,
                         sFONT* Font, UWORD Color_Foreground, UWORD Color_Background)
{
    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;

    if (Xstart > this->Width || Ystart > this->Height) {
        Debug("Paint_DrawString_EN Input exceeds the normal display range\r\n");
        return;
    }

    while (* pString != '\0') {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
        if ((Xpoint + Font->Width ) > this->Width ) {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint  + Font->Height ) > this->Height ) {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }
        Paint_DrawChar(Xpoint, Ypoint, * pString, Font, Color_Background, Color_Foreground);

        //The next character of the address
        pString ++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}


/******************************************************************************
function: Display the string
parameter:
    Xstart  ：X coordinate
    Ystart  ：Y coordinate
    pString ：The first address of the Chinese string and English
              string to be displayed
    Font    ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
//void FrameBuffer::Paint_DrawString_CN(UWORD Xstart, UWORD Ystart, const char * pString, cFONT* font,
//                        UWORD Color_Foreground, UWORD Color_Background)
//{
//    const char* p_text = pString;
//    int x = Xstart, y = Ystart;
//    int i, j,Num;
//
//    /* Send the string character by character on EPD */
//    while (*p_text != 0) {
//        if(*p_text <= 0x7F) {  //ASCII < 126
//            for(Num = 0; Num < font->size; Num++) {
//                if(*p_text== font->table[Num].index[0]) {
//                    const char* ptr = &font->table[Num].matrix[0];
//
//                    for (j = 0; j < font->Height; j++) {
//                        for (i = 0; i < font->Width; i++) {
//                            if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
//                                if (*ptr & (0x80 >> (i % 8))) {
//                                    Paint_SetPixel(x + i, y + j, Color_Foreground);
//                                    // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                                }
//                            } else {
//                                if (*ptr & (0x80 >> (i % 8))) {
//                                    Paint_SetPixel(x + i, y + j, Color_Foreground);
//                                    // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                                } else {
//                                    Paint_SetPixel(x + i, y + j, Color_Background);
//                                    // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                                }
//                            }
//                            if (i % 8 == 7) {
//                                ptr++;
//                            }
//                        }
//                        if (font->Width % 8 != 0) {
//                            ptr++;
//                        }
//                    }
//                    break;
//                }
//            }
//            /* Point on the next character */
//            p_text += 1;
//            /* Decrement the column position by 16 */
//            x += font->ASCII_Width;
//        } else {        //Chinese
//            for(Num = 0; Num < font->size; Num++) {
//                if((*p_text== font->table[Num].index[0]) && (*(p_text+1) == font->table[Num].index[1])) {
//                    const char* ptr = &font->table[Num].matrix[0];
//
//                    for (j = 0; j < font->Height; j++) {
//                        for (i = 0; i < font->Width; i++) {
//                            if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
//                                if (*ptr & (0x80 >> (i % 8))) {
//                                    Paint_SetPixel(x + i, y + j, Color_Foreground);
//                                    // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                                }
//                            } else {
//                                if (*ptr & (0x80 >> (i % 8))) {
//                                    Paint_SetPixel(x + i, y + j, Color_Foreground);
//                                    // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                                } else {
//                                    Paint_SetPixel(x + i, y + j, Color_Background);
//                                    // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                                }
//                            }
//                            if (i % 8 == 7) {
//                                ptr++;
//                            }
//                        }
//                        if (font->Width % 8 != 0) {
//                            ptr++;
//                        }
//                    }
//                    break;
//                }
//            }
//            /* Point on the next character */
//            p_text += 2;
//            /* Decrement the column position by 16 */
//            x += font->Width;
//        }
//    }
//}

/******************************************************************************
function:	Display nummber
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The number displayed
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
#define  ARRAY_LEN 255
void FrameBuffer::Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, int32_t Nummber,
                   sFONT* Font, UWORD Color_Foreground, UWORD Color_Background)
{

    int16_t Num_Bit = 0, Str_Bit = 0;
    uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
    uint8_t *pStr = Str_Array;

    if (Xpoint > this->Width || Ypoint > this->Height) {
        Debug("Paint_DisNum Input exceeds the normal display range\r\n");
        return;
    }

    //Converts a number to a string
    do {
        Num_Array[Num_Bit] = Nummber % 10 + '0';
        Num_Bit++;
        Nummber /= 10;
    } while(Nummber);


    //The string is inverted
    while (Num_Bit > 0) {
        Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
        Str_Bit ++;
        Num_Bit --;
    }

    //show
    Paint_DrawString_EN(Xpoint, Ypoint, (const char*)pStr, Font, Color_Background, Color_Foreground);
}

/******************************************************************************
function:	Display nummber (Able to display decimals)
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The number displayed
    Font             ：A structure pointer that displays a character size
    Digit            : Fractional width
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void FrameBuffer::Paint_DrawNumDecimals(UWORD Xpoint, UWORD Ypoint, double Nummber,
                    const char* Font, UWORD Digit, UWORD Color_Foreground, UWORD Color_Background)
{
    int16_t Num_Bit = 0, Str_Bit = 0;
    uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
    uint8_t *pStr = Str_Array;
	int temp = Nummber;
	float decimals;
	uint8_t i;
    if (Xpoint > this->Width || Ypoint > this->Height) {
        Debug("Paint_DisNum Input exceeds the normal display range\r\n");
        return;
    }

	if(Digit > 0) {
		decimals = Nummber - temp;
		for(i=Digit; i > 0; i--) {
			decimals*=10;
		}
		temp = decimals;
		//Converts a number to a string
		for(i=Digit; i>0; i--) {
			Num_Array[Num_Bit] = temp % 10 + '0';
			Num_Bit++;
			temp /= 10;
		}
		Num_Array[Num_Bit] = '.';
		Num_Bit++;
	}

	temp = Nummber;
    //Converts a number to a string
    do {
        Num_Array[Num_Bit] = temp % 10 + '0';
        Num_Bit++;
        temp /= 10;
    } while(temp);

    //The string is inverted
    while (Num_Bit > 0) {
        Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
        Str_Bit ++;
        Num_Bit --;
    }

    //show
    Paint_DrawString(Xpoint, Ypoint, (const char*)pStr, Font, Color_Foreground, Color_Background);
}

/******************************************************************************
function:	Display time
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    pTime            : Time-related structures
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void FrameBuffer::Paint_DrawTime(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, const char* Font,
                    UWORD Color_Foreground, UWORD Color_Background)
{
    char value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
//    LOG_WARN("time seconds %d\r\n/r/n",pTime->Sec);
//    UWORD Dx = Font->Width;
    char timeStr[10];
    snprintf(timeStr, 10, "%c%c:%c%c %cM", &value[pTime->Hour / 10], &value[pTime->Hour % 10],
    																 &value[pTime->Min / 10],
																	 &value[pTime->Min % 10],
																	 pTime->isPM ? 'P' : 'A' );
    Paint_DrawString(Xstart, Ystart, timeStr, Font, Color_Background, Color_Foreground);


    //Write data into the cache
//    Paint_DrawString(Xstart                           , Ystart, &value[pTime->Hour / 10], Font, Color_Background, Color_Foreground);
//    Paint_DrawString(Xstart + Dx                      , Ystart, &value[pTime->Hour % 10], Font, Color_Background, Color_Foreground);
//    Paint_DrawString(Xstart + Dx  + Dx / 4 + Dx / 2   , Ystart, ":"                    , Font, Color_Background, Color_Foreground);
//    Paint_DrawString(Xstart + Dx * 2 + Dx / 2         , Ystart, &value[pTime->Min / 10] , Font, Color_Background, Color_Foreground);
//    Paint_DrawString(Xstart + Dx * 3 + Dx / 2         , Ystart, &value[pTime->Min % 10] , Font, Color_Background, Color_Foreground);

//    if(pTime->hasSeconds){
//    	Paint_DrawString(Xstart + Dx * 4 + Dx / 2 - Dx / 4, Ystart, ":"                    , Font, Color_Background, Color_Foreground);
//    	Paint_DrawString(Xstart + Dx * 5                  , Ystart, &value[pTime->Sec / 10] , Font, Color_Background, Color_Foreground);
//    	Paint_DrawString(Xstart + Dx * 6                  , Ystart, &value[pTime->Sec % 10] , Font, Color_Background, Color_Foreground);
//    }
//
//    UWORD additionalDX = 0;
//    if(pTime->hasSeconds){
//    	additionalDX =3;
//    }
//	Paint_DrawString(Xstart + Dx * 5 + additionalDX       , Ystart, pTime->isPM ? "P" : "A" , Font, Color_Background, Color_Foreground);
//	Paint_DrawString(Xstart + Dx * 6 + additionalDX       , Ystart, "M" , Font, Color_Background, Color_Foreground);

}

/******************************************************************************
function:	Display monochrome bitmap
parameter:
    image_buffer ：A picture data converted to a bitmap
info:
    Use a computer to convert the image into a corresponding array,
    and then embed the array directly into Imagedata.cpp as a .c file.
******************************************************************************/
void FrameBuffer::Paint_DrawBitMap(const unsigned char* image_buffer)
{
    UWORD x, y;
    UDOUBLE Addr = 0;

    for (y = 0; y < this->HeightByte; y++) {
        for (x = 0; x < this->WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * this->WidthByte;
            this->Image[Addr] = (unsigned char)image_buffer[Addr];
        }
    }
}

/******************************************************************************
function:	paste monochrome bitmap to a frame buff
parameter:
    image_buffer ：A picture data converted to a bitmap
    xStart: The starting x coordinate
    yStart: The starting y coordinate
    imageWidth: Original image width
    imageHeight: Original image height
    flipColor: Whether the color is reversed
info:
    Use this function to paste image data into a buffer
******************************************************************************/
void FrameBuffer::Paint_DrawBitMap_Paste(const unsigned char* image_buffer, UWORD xStart, UWORD yStart, UWORD imageWidth, UWORD imageHeight, UBYTE flipColor)
{
    UBYTE color, srcImage;
    UWORD x, y;
    UWORD width = (imageWidth%8==0 ? imageWidth/8 : imageWidth/8+1);

    for (y = 0; y < imageHeight; y++) {
        for (x = 0; x < imageWidth; x++) {
            srcImage = image_buffer[y*width + x/8];
            if(flipColor)
                color = (((srcImage<<(x%8) & 0x80) == 0) ? 1 : 0);
            else
                color = (((srcImage<<(x%8) & 0x80) == 0) ? 0 : 1);
            Paint_SetPixel(x+xStart, y+yStart, color);
        }
    }
}

void FrameBuffer::Paint_DrawBitMap_Block(const unsigned char* image_buffer, UBYTE Region)
{
    UWORD x, y;
    UDOUBLE Addr = 0;
		for (y = 0; y < this->HeightByte; y++) {
				for (x = 0; x < this->WidthByte; x++) {//8 pixel =  1 byte
						Addr = x + y * this->WidthByte ;
						this->Image[Addr] = \
						(unsigned char)image_buffer[Addr+ (this->HeightByte)*this->WidthByte*(Region - 1)];
				}
		}
}

