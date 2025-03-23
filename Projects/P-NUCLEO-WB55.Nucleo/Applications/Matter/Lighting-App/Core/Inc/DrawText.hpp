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

	sFONT* Font;
	UWORD Color_Foreground;
	UWORD Color_Background;
  /* data */
public:

    void draw(FrameBuffer& fb, UWORD xMove = 0, UWORD yMove = 0) override{
    	if(updated){
			if(pTime == nullptr)
				fb.Paint_DrawString_EN( Xstart + xMove,  Ystart + yMove,  pString, Font,  Color_Foreground,  Color_Background); // we want to pass in a array
			else{
				fb.Paint_DrawTime(Xstart + xMove, Ystart + yMove, pTime, Font, Color_Foreground, Color_Background);
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
		Xstart(Xstart), Ystart(Ystart), pString(pString), pTime(nullptr), Font(Font), Color_Foreground(Color_Foreground), Color_Background(Color_Background) {}

    DrawText(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT* Font,UWORD Color_Foreground, UWORD Color_Background):
		Drawable(),
		Xstart(Xstart), Ystart(Ystart), pString(nullptr), pTime(pTime), Font(Font), Color_Foreground(Color_Foreground), Color_Background(Color_Background) {}


    ~DrawText(){}
};


class HighlightableDrawText: public DrawText {
public:
	HighlightableDrawText(UWORD Xstart, UWORD Ystart, const char * pString, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background):
		DrawText( Xstart,  Ystart,  pString, Font, Color_Foreground, Color_Background) {}

	HighlightableDrawText(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT* Font,UWORD Color_Foreground, UWORD Color_Background):
		DrawText( Xstart,  Ystart, pTime,  Font, Color_Foreground,  Color_Background) {}

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
