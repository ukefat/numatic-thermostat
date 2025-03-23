/*
 * Circle.h
 *
 *  Created on: Feb 25, 2025
 *      Author: kobru
 */

#ifndef INC_CIRCLE_H_
#define INC_CIRCLE_H_

#include "Drawable.h"

class Circle: public Drawable
{
private:
	// TODO
	// appropriate manipulation techniques
	UWORD xCenter,yCenter, radius, color;
	DOT_PIXEL lineWidth;
	DRAW_FILL fill;
  /* data */
public:

    void draw(FrameBuffer& fb, UWORD xDifference = 0, UWORD yDifference=0) override{
    	fb.Paint_DrawCircle(xCenter + xDifference, yCenter + yDifference, radius, color, lineWidth, fill); // we want to pass in a array
    }

    void highlight(bool isSelected) override{
    	if(isSelected){
    		fill = DRAW_FILL_FULL;
    	}else {
    		fill = DRAW_FILL_EMPTY;
    	}
    }

    Circle(UWORD xCenter, UWORD yCenter, UWORD radius, UWORD color, DOT_PIXEL lineWidth, DRAW_FILL fill):
    	Drawable({}),
		xCenter(xCenter), yCenter(yCenter), radius(radius), color(color), lineWidth(lineWidth), fill(fill) {}
    ~Circle(){}
};



#endif /* INC_CIRCLE_H_ */
