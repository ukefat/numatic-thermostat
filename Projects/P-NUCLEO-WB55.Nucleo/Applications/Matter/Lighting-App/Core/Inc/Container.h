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
//			if(updated){
//				drawables[i]->clearArea(fb, x, y);
//			}
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

    void resetClear() override {
        for (int i = 0; i < drawableCount; i++) {
            drawables[i]->resetClear();
        }
    }

    void highlight(bool isSelected) override{
//    	updated = true;
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


//
//void drawAlertScreen(Screen& alertScreen) {
//	UWORD containerWidth = 138;
//	UWORD containerHeight = 65;
//
//	UWORD scheduleContainerHeightIndent = 15;
//	UWORD scheduleContainerWidth = 307;
//	UWORD scheduleContainerHeight = 56;
//
//	Container BackContainer = Container(169,415,containerWidth,containerHeight);
//	Rectangle backRectangle =  Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
//	DrawText backText =  DrawText(20,20,"Back",&Font16, WHITE, BLACK);
//
//	BackContainer.addDrawable(&backRectangle);
//	BackContainer.addDrawable(&backText);
////
//	Container homeContainer = Container(331,415,containerWidth,containerHeight);
////		Rectangle* homeRectangle = new Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
//	DrawText homeText = DrawText(20,20,"Home",&Font16, WHITE, BLACK);
//
//	homeContainer.addDrawable(&backRectangle);
//	homeContainer.addDrawable(&homeText);
//
//	Container dismissContainer = Container(493,415,containerWidth,containerHeight);
////		Rectangle* selectRectangle = new Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
//	DrawText selectText = DrawText(20,20,"Select",&Font16, WHITE, BLACK);
//
//	dismissContainer.addDrawable(&backRectangle);
//	dismissContainer.addDrawable(&selectText);
//
//	UWORD mainMenuWidth = 747;
//	UWORD mainMenuHeight = 93;
//
//
//	Container Alert1 = Container(0, scheduleContainerHeightIndent, scheduleContainerWidth, scheduleContainerHeight, [](){
//
//	} );
//	HighLightOnInteractRectangle Alert1Rectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//	HighlightableDrawText Alert1Text = HighlightableDrawText(20,20,"Heating Warning",&Font24, WHITE, BLACK);
//
//	Alert1.addDrawable(&Alert1Rectangle);
//	Alert1.addDrawable(&Alert1Text);
//
//	Container Alert2Container = Container(0, scheduleContainerHeightIndent + scheduleContainerHeight * 1, scheduleContainerWidth, scheduleContainerHeight, [](){
//
//	}  );
//	HighLightOnInteractRectangle Alert2Rectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//	HighlightableDrawText Alert2Text = HighlightableDrawText(20,20,"Leaking Pipes",&Font24, WHITE, BLACK);
//
//	Alert2Container.addDrawable(&Alert2Rectangle);
//	Alert2Container.addDrawable(&Alert2Text);
//
//
//
//
//
//	alertScreen.addDrawable(&BackContainer);
//	alertScreen.addDrawable(&homeContainer);
//	alertScreen.addDrawable(&dismissContainer);
//
//	alertScreen.addDrawable(&Alert1);
//	alertScreen.addDrawable(&Alert2Container);
//
//
//
//}
//void drawClockDateScreen(Screen& clockDateScreen){
//	UWORD clockContainerWidth = 138;
//	UWORD ClockContainerHeight = 65;
//
//
//	Container clockLeftContainer = Container(169,415,clockContainerWidth,ClockContainerHeight);
//	Rectangle clockButtonRectangle =  Rectangle(0, 0, clockContainerWidth, ClockContainerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
//	DrawText clockLeftText =  DrawText(20,20,"Back",&Font16, WHITE, BLACK);
//
//	Container clockMiddleContainer = Container(331,415,clockContainerWidth,ClockContainerHeight);
//	DrawText clockMiddleText= DrawText(20,20,"Home",&Font16, WHITE, BLACK);
//
//	Container clockRightContainer = Container(493,415,clockContainerWidth,ClockContainerHeight);
//	DrawText clockRightContainerText = DrawText(20,20,"Select",&Font16, WHITE, BLACK);
//
//
//	clockDateScreen.addDrawable(&clockLeftContainer);
//	clockDateScreen.addDrawable(&clockRightContainer);
//	clockDateScreen.addDrawable(&clockMiddleContainer);
//
//}

#endif /* INC_CONTAINER_H_ */
