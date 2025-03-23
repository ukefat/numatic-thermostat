/*
 * TextContainer.hpp
 *
 *  Created on: Mar 21, 2025
 *      Author: kobru
 */

#ifndef INC_TEXTCONTAINER_HPP_
#define INC_TEXTCONTAINER_HPP_

#include "Drawable.h"
#include "Container.h"
#include "Rectangle.h"
#include "DrawText.hpp"
#include "main.h"

class TextContainer : Container{
public:
	TextContainer() = delete;
	TextContainer(UWORD xStart, UWORD yStart, UWORD xEnd, UWORD yEnd, Callback cb = nullptr):
		Container( xStart,  yStart,  xEnd,  yEnd,  cb ),
		rectangle(xStart, yStart, xEnd, yEnd,
		WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY,
		nullptr,
		Rotate_ = ROTATE_0,  Mirror_ = MIRROR_NONE, layer_ = 0),
		drawtext(0,0,buffer, &Font24, WHITE, BLACK)
		{
			addDrawable(&rectangle);
			addDrawable(&drawtext);
		}

private:

	char buffer[50];
	HighLightOnInteractRectangle rectangle;
	HighlightableDrawText drawtext;
};




#endif /* INC_TEXTCONTAINER_HPP_ */
