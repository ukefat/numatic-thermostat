/*
 * Rectangle.h
 *
 *  Created on: Feb 25, 2025
 *      Author: kobru
 */

#ifndef INC_RECTANGLE_H_
#define INC_RECTANGLE_H_


#include "Drawable.h"

class Rectangle: public Drawable
{
protected:
	// TODO
	// appropriate manipulation techniques
	UWORD xStart,yStart, xEnd, yEnd, color;
	DOT_PIXEL lineWidth;
	DRAW_FILL fill;
  /* data */
public:

	void draw(FrameBuffer& fb, UWORD xStart = 0, UWORD yStart =0) override {

		if (updated || firstDraw ){
			fb.Paint_DrawRectangle(this->xStart + xStart, this->yStart+ yStart, xEnd+ xStart, yEnd+ yStart, color, lineWidth, fill);
		}
		firstDraw = false;
		updated = false;
    }

    void clearArea(FrameBuffer& fb, UWORD xStart = 0, UWORD yStart =0) override {
//    	updated = true;
    	fb.Paint_ClearWindows(this->xStart + xStart, this->yStart+ yStart, xEnd+ xStart, yEnd+ yStart, color==BLACK?WHITE:BLACK);
    }


    Rectangle(UWORD xStart, UWORD yStart, UWORD xEnd, UWORD yEnd, UWORD color, DOT_PIXEL lineWidth, DRAW_FILL fill,
    		Callback onSelect = nullptr,
    		UWORD Rotate_ = ROTATE_0, UWORD Mirror_ = MIRROR_NONE, UWORD layer_ = 0 ):
    	Drawable(onSelect, Rotate_, Mirror_,  layer_),
    	xStart(xStart), yStart(yStart), xEnd(xEnd), yEnd(yEnd), color(color), lineWidth(lineWidth), fill(fill) {}
    ~Rectangle(){}

	UWORD getColor() const {
		return color;
	}

	void setColor( UWORD color) {
		this->color = color;
	}

	DRAW_FILL getFill() const {
		return fill;
	}

	void setFill(DRAW_FILL fill) {
		this->fill = fill;
	}

	DOT_PIXEL getLineWidth() const {
		return lineWidth;
	}

	void setLineWidth(DOT_PIXEL lineWidth) {
		this->lineWidth = lineWidth;
	}

	void updateDimensions(UWORD xStart, UWORD yStart, UWORD xEnd, UWORD yEnd) {
		this->yStart = yStart;
		this->yEnd = yEnd;
		this->xStart = xStart;
		this->xEnd = xEnd;
	}

	UWORD getXEnd() const
	{
		return xEnd;
	}

	UWORD getXStart() const
	{
		return xStart;
	}


	UWORD getYEnd() const
	{
		return yEnd;
	}

	UWORD getYStart() const
	{
		return yStart;
	}

};

class HighLightOnInteractRectangle: public Rectangle {
public:
	HighLightOnInteractRectangle(
			UWORD xStart, UWORD yStart, UWORD xEnd, UWORD yEnd,
			UWORD color, DOT_PIXEL lineWidth, DRAW_FILL fill,
			Callback onSelect = nullptr,
			UWORD Rotate_ = ROTATE_0, UWORD Mirror_ = MIRROR_NONE, UWORD layer_ = 0 ):
		Rectangle(xStart, yStart, xEnd, yEnd, color, lineWidth, fill, onSelect, Rotate_, Mirror_,  layer_)
	    {}
private:
	virtual void highlight(bool isSelected) override{
		if(isSelected){
			this->fill = DRAW_FILL_FULL;
		}else {
			this->fill = DRAW_FILL_EMPTY;
		}
		updated = true;
	}
};



#endif /* INC_RECTANGLE_H_ */
