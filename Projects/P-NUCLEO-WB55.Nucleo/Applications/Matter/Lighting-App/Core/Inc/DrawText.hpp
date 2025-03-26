/*
 * DrawText.hpp
 *
 *  Created on: Mar 5, 2025
 *      Author: kobru
 */

#ifndef INC_DRAWTEXT_HPP_
#define INC_DRAWTEXT_HPP_


#include "Drawable.h"

class DrawText: public Drawable
{
protected:

	UWORD Xstart;
	UWORD Ystart;

	const char * pString;
	PAINT_TIME* pTime ;
	double Number;
	UWORD digit;

	sFONT* Font;
	UWORD Color_Foreground;
	UWORD Color_Background;
  /* data */
public:

	void setNumber(double N){
//		if(N )
		if(N != Number){
			updated = true;
		}
		Number = N;

	}

	void setString(const char* string){
		pString = string;
		updated = true;
	}
	void setDigit(UWORD D){
		if(digit != D){
			updated = true;
		}
		digit = D;

	}
    void draw(FrameBuffer& fb, UWORD xMove = 0, UWORD yMove = 0) override{
    	if(updated){
             if(pTime != nullptr)
				fb.Paint_DrawTime(Xstart + xMove, Ystart + yMove, pTime, Font, Color_Foreground, Color_Background);
			else if (pString != nullptr){
				fb.Paint_DrawString_EN( Xstart + xMove,  Ystart + yMove,  pString, Font,  Color_Foreground,  Color_Background); // we want to pass in a array
			} else{
				fb.Paint_DrawNumDecimals(Xstart+ xMove , Ystart+ yMove, Number, Font, 1, Color_Foreground, Color_Background);
			}
    	}
    	updated = false;
    }

    void updatedText(){
    	updated = true;
    }
    DrawText() = delete;

    DrawText(UWORD Xstart, UWORD Ystart, const char * pString, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background):
    	Drawable(),
		Xstart(Xstart), Ystart(Ystart), pString(pString), pTime(nullptr), Number(-1), digit(0), Font(Font), Color_Foreground(Color_Foreground), Color_Background(Color_Background) {}

    DrawText(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT* Font,UWORD Color_Foreground, UWORD Color_Background):
		Drawable(),
		Xstart(Xstart), Ystart(Ystart), pString(nullptr), pTime(pTime), Number(-1), digit(0), Font(Font), Color_Foreground(Color_Foreground), Color_Background(Color_Background) {}

    DrawText(UWORD Xstart, UWORD Ystart, double drawNum, sFONT* Font, UWORD digit, UWORD Color_Foreground, UWORD Color_Background):
		Drawable(),
		Xstart(Xstart), Ystart(Ystart), pString(nullptr), pTime(nullptr), Number(drawNum), digit(digit), Font(Font), Color_Foreground(Color_Foreground), Color_Background(Color_Background) {}


    ~DrawText(){}
};


class HighlightableDrawText: public DrawText {
public:
	HighlightableDrawText(UWORD Xstart, UWORD Ystart, const char * pString, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background):
		DrawText( Xstart,  Ystart,  pString, Font, Color_Foreground, Color_Background) {}

	HighlightableDrawText(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT* Font,UWORD Color_Foreground, UWORD Color_Background):
		DrawText( Xstart,  Ystart, pTime,  Font, Color_Foreground,  Color_Background) {}
	HighlightableDrawText(UWORD Xstart, UWORD Ystart, double drawNum, sFONT* Font, UWORD digit, UWORD Color_Foreground, UWORD Color_Background):
		DrawText( Xstart,  Ystart, drawNum,  Font,digit, Color_Foreground,  Color_Background) {}
	HighlightableDrawText() = delete;

    void highlight(bool isSelected) override{

		if(isSelected){
			this->Color_Foreground = BLACK;
			this->Color_Background = WHITE;

		}else {
			this->Color_Foreground = WHITE;
			this->Color_Background = BLACK;

		}
		updated = true;
    }

};



#endif /* INC_DRAWTEXT_HPP_ */
