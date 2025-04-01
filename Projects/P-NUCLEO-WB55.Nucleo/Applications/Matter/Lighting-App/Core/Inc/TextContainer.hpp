///*
// * TextContainer.hpp
// *
// *  Created on: Mar 21, 2025
// *      Author: kobru
// */
//
//#ifndef INC_TEXTCONTAINER_HPP_
//#define INC_TEXTCONTAINER_HPP_
//
#include "Drawable.hpp"
#include "Container.hpp"
#include "Rectangle.hpp"
#include "DrawText.hpp"
#include "main.h"


class TextContainer : public Container{
public:
	TextContainer() = delete;
	TextContainer(UWORD xStart, UWORD yStart, UWORD xEnd, UWORD yEnd, const char * str = "",Callback cb = nullptr):
		Container( xStart,  yStart,  xEnd,  yEnd,  cb ),
		rectangle(0, 0, xEnd, yEnd,
		BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY),
		drawtext(0,0,str, "Century60", WHITE, BLACK)
		{
			addDrawable(&rectangle);
			addDrawable(&drawtext);
		}

	void setText(const char * str){


	    // Force update of text pointer if your drawtext caches it
		resetClear();
	    drawtext.setString(str); // Assuming your HighlightableDrawText supports this

	}
private:

	char buffer[20];
	HighLightOnInteractRectangle rectangle;
	HighlightableDrawText drawtext;
};

class InteractableTextContainer : public Container{
public:
	InteractableTextContainer() = delete;
	InteractableTextContainer(UWORD xStart, UWORD yStart, UWORD xEnd, UWORD yEnd, const char * str = "",Callback cb = nullptr):
		Container( xStart,  yStart,  xEnd,  yEnd,  cb ),
		rectangle(0, 0, xEnd, yEnd,
		BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY),
		drawtext(0,0,str, "Century60", WHITE, BLACK)
		{
			addDrawable(&rectangle);
			addDrawable(&drawtext);
		}

	void setText(const char * str){


	    // Force update of text pointer if your drawtext caches it
	    drawtext.setString(str); // Assuming your HighlightableDrawText supports this
	}
private:

	char buffer[20];
	HighLightOnInteractRectangle rectangle;
	HighlightableDrawText drawtext;
};

//
//
//
//#endif /* INC_TEXTCONTAINER_HPP_ */
