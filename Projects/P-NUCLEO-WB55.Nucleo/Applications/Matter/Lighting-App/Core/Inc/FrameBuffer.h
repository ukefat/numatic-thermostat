/*
 * FrameBuffer.h
 *
 *  Created on: Feb 25, 2025
 *      Author: kobru
 */

#ifndef INC_FRAMEBUFFER_H_
#define INC_FRAMEBUFFER_H_



#include "DEV_Config.h"
#include "../../Fonts/fonts.h"
#include <vector>
#include <memory>
#include "FreeRTOSMemory.hpp"
#include "Debug.h"
/**
 * Image attributes
**/


/**
 * Display rotate
**/
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270

/**
 * Display Flip
**/
typedef enum {
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
} MIRROR_IMAGE;
#define MIRROR_IMAGE_DFT MIRROR_NONE

/**
 * image color
**/
#define WHITE          0xFF
#define BLACK          0x00
#define RED            BLACK

#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE

#define TRUE 1
#define FALSE 0

//4 Gray level
#define  GRAY1 0x03 //Blackest
#define  GRAY2 0x02
#define  GRAY3 0x01 //gray
#define  GRAY4 0x00 //white

/**
 * The size of the point
**/
typedef enum {
    DOT_PIXEL_1X1  = 1,		// 1 x 1
    DOT_PIXEL_2X2  , 		// 2 X 2
    DOT_PIXEL_3X3  ,		// 3 X 3
    DOT_PIXEL_4X4  ,		// 4 X 4
    DOT_PIXEL_5X5  , 		// 5 X 5
    DOT_PIXEL_6X6  , 		// 6 X 6
    DOT_PIXEL_7X7  , 		// 7 X 7
    DOT_PIXEL_8X8  , 		// 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

/**
 * Point size fill style
**/
typedef enum {
    DOT_FILL_AROUND  = 1,		// dot pixel 1 x 1
    DOT_FILL_RIGHTUP  , 		// dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

/**
 * Line style, solid or dashed
**/
typedef enum {
    LINE_STYLE_SOLID = 0,
    LINE_STYLE_DOTTED,
} LINE_STYLE;

/**
 * Whether the graphic is filled
**/
typedef enum {
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL,
} DRAW_FILL;

/**
 * Custom structure of a time attribute
**/
typedef struct {
    UWORD Year;  //0000
    UBYTE  Month; //1 - 12
    UBYTE  Day;   //1 - 30
    UBYTE  Hour;  //0 - 23
    UBYTE  Min;   //0 - 59
    UBYTE  Sec;   //0 - 59
    bool hasSeconds;
    bool isPM;
} PAINT_TIME;
extern PAINT_TIME sPaint_time;


//init and Clear
class FrameBuffer {
private:
    static std::unique_ptr<FrameBuffer, FreeRTOSDeleter> instance; // Unique pointer for singleton
    std::unique_ptr<UBYTE[], FreeRTOSDeleter> Image;  // 48KB heap-allocated buffer	UWORD Width;
//	std::unique_ptr<UBYTE[], FreeRTOSDeleter> Image;
//	UBYTE* Image;
	UWORD Width;
	UWORD Height;
	UWORD WidthMemory;
	UWORD HeightMemory;
	UWORD Color;
	UWORD Rotate;
	UWORD Mirror;
	UWORD WidthByte;
	UWORD HeightByte;
	UWORD Scale;


	FrameBuffer(UWORD Width = EPD_4in26_WIDTH, UWORD Height= EPD_4in26_HEIGHT, UWORD Rotate = ROTATE_0, UWORD Color = WHITE);

public:

	FrameBuffer() = delete;

    static FrameBuffer& getInstance(UWORD Width = EPD_4in26_WIDTH, UWORD Height= EPD_4in26_HEIGHT, UWORD Rotate = ROTATE_0, UWORD Color = WHITE) {
        if (!instance) {
        	void* mem = pvPortMalloc(sizeof(FrameBuffer));  // Allocate the singleton instance
//            instance = std::unique_ptr<FrameBuffer>(new FrameBuffer(Width, Height, Rotate, Color)); // Use `new` since the constructor is private
        	if (mem) {
				instance.reset(new (mem) FrameBuffer(Width, Height, Rotate, Color)); // Placement new
			} else {
				LOG_ERROR("FreeRTOS allocation failed for FrameBuffer instance!");
			}
        }
        return *instance;
    }

	UBYTE* getImage() {
		return Image.get();
	}


	void Paint_initImage(UWORD Width, UWORD Height, UWORD Rotate, UWORD Color);
	void Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color);
//	void Paint_SelectImage(UBYTE *image);
	void Paint_SetRotate(UWORD Rotate);
	void Paint_SetMirroring(UBYTE mirror);
	void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color);
	void Paint_SetScale(UBYTE scale);

	void Paint_Clear(UWORD Color);
	void Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color);

	//Drawing
	void Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color, DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay);
	void Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style);
	void Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill);
	void Paint_DrawCircle(UWORD X_Center, UWORD Y_Center, UWORD Radius, UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill);

	//Display string
	void Paint_DrawChar(UWORD Xstart, UWORD Ystart, const char Acsii_Char, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);
	void Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);
//	void Paint_DrawString_CN(UWORD Xstart, UWORD Ystart, const char * pString, cFONT* font, UWORD Color_Foreground, UWORD Color_Background);
	void Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, int32_t Nummber, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);
	void Paint_DrawNumDecimals(UWORD Xpoint, UWORD Ypoint, double Nummber, sFONT* Font, UWORD Digit, UWORD Color_Foreground, UWORD Color_Background); // Able to display decimals
	void Paint_DrawTime(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);

	//pic
	void Paint_DrawBitMap(const unsigned char* image_buffer);
	void Paint_DrawBitMap_Paste(const unsigned char* image_buffer, UWORD Xstart, UWORD Ystart, UWORD imageWidth, UWORD imageHeight, UBYTE flipColor);
	//void Paint_DrawBitMap_Half(const unsigned char* image_buffer, UBYTE Region);
	//void Paint_DrawBitMap_OneQuarter(const unsigned char* image_buffer, UBYTE Region);
	//void Paint_DrawBitMap_OneEighth(const unsigned char* image_buffer, UBYTE Region);
	void Paint_DrawBitMap_Block(const unsigned char* image_buffer, UBYTE Region);

	UWORD getColor() const {
		return Color;
	}

	UWORD getHeight() const {
		return Height;
	}

	UWORD getHeightByte() const {
		return HeightByte;
	}

	UWORD getHeightMemory() const {
		return HeightMemory;
	}

	UWORD getMirror() const {
		return Mirror;
	}

	UWORD getRotate() const {
		return Rotate;
	}

	UWORD getScale() const {
		return Scale;
	}

	UWORD getWidth() const {
		return Width;
	}

	UWORD getWidthByte() const {
		return WidthByte;
	}

	UWORD getWidthMemory() const {
		return WidthMemory;
	}
};

#endif


