/*
 * ButtonContainer.hpp
 *
 *  Created on: Mar 30, 2025
 *      Author: kobru
 */

#ifndef SRC_BUTTONCONTAINER_HPP_
#define SRC_BUTTONCONTAINER_HPP_

#include "Container.hpp"
#include "Rectangle.hpp"
#include "DrawText.hpp"
#include "../Fonts/fonts.h"
class ButtonContainer: public Container {
public:
	ButtonContainer(UWORD xStart, UWORD yStart, UWORD xEnd, UWORD yEnd, const char * strVal , Callback cb = nullptr):
		Container( xStart,  yStart,  xEnd,  yEnd,  cb ),
		containerRectagle(0, 0, xEnd, yEnd,
		BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY),
		text(10,10,strVal, "Century60", WHITE, BLACK)
		{
			addDrawable(&containerRectagle);
			addDrawable(&text);
		}
	ButtonContainer() = delete;
private:
	Rectangle containerRectagle;
	DrawText text;
};


#endif /* SRC_BUTTONCONTAINER_HPP_ */
