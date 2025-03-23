/*
 * Container.h
 *
 *  Created on: Feb 25, 2025
 *      Author: kobru
 */

#ifndef INC_CONTAINER_H_
#define INC_CONTAINER_H_

#include "Drawable.h"
//#include <vector>

#define MAX_CONTAINER_DRAWABLES 10  // Set a reasonable limit for contained drawables

class Container: public Drawable {
public:
	Container(UWORD xStart, UWORD yStart, UWORD xEnd, UWORD yEnd, Callback cb = nullptr):
		Drawable(cb),
		xStart(xStart), yStart(yStart), xEnd(xEnd), yEnd(yEnd), drawableCount(0)
	{

	}
    void draw(FrameBuffer& fb, UWORD xMove = 0, UWORD yMove =0) override{
    	//TODO displace draws in the container?

    	UWORD x = xStart + xMove;
    	UWORD y = yStart + yMove;

        for (int i = 0; i < drawableCount; i++) {
			if(updated){
				drawables[i]->clearArea(fb, x, y);
			}
			if(!hide){
				drawables[i]->draw(fb, x, y);
			}
        }
        updated = false;

    }

    void resetUpdated() override{
        for (int i = 0; i < drawableCount; i++) {
            drawables[i]->resetUpdated();
        }
    }

    void highlight(bool isSelected) override{
    	updated = true;
    	for (int i = 0; i < drawableCount; i++) {
            drawables[i]->highlight(isSelected);
        }
    }

    void addDrawable(Drawable * draw){
        if (drawableCount < MAX_CONTAINER_DRAWABLES) {
            drawables[drawableCount++] = draw;
        }
    }

	UWORD getXEnd() const {
		return xEnd;
	}

	void setXEnd(UWORD xEnd) {
		this->xEnd = xEnd;
	}

	UWORD getXStart() const {
		return xStart;
	}

	void setXStart(UWORD xStart) {
		this->xStart = xStart;
	}

	UWORD getYEnd() const {
		return yEnd;
	}

	void setYEnd(UWORD yEnd) {
		this->yEnd = yEnd;
	}

	UWORD getYStart() const {
		return yStart;
	}

	void setYStart(UWORD yStart) {
		this->yStart = yStart;
	}

private:
    UWORD xStart;
    UWORD yStart;
    UWORD xEnd;
    UWORD yEnd;

    Drawable* drawables[MAX_CONTAINER_DRAWABLES]; // Fixed-size array
    int drawableCount;
};



#endif /* INC_CONTAINER_H_ */
