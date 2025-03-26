/*
 * Screen.h
 *
 *  Created on: Feb 24, 2025
 *      Author: kobru
 */

#ifndef INC_SCREEN_H_
#define INC_SCREEN_H_

//#include <vector>
#include <EPD_4in26.hpp>
#include <EPD_test.hpp>
#include "Drawable.h"
#include "FrameBuffer.h"
#include "Container.h"
#include <memory>

#define MAX_SCREEN_DRAWABLES 10 // Define a reasonable limit for the array

#include "DrawableObjects.hpp"
//#include

extern DaySchedule schedule[];
extern UBYTE currentDay;
extern UBYTE currentSetPointSelection;
class Screen {
public:
	using Callback = void (*)(Button button);

    Screen(Callback cb = nullptr, UWORD Width_ = EPD_4in26_WIDTH, UWORD Height_  = EPD_4in26_HEIGHT, UWORD colour = WHITE, UWORD Rotate= ROTATE_0):
    	currentSelection(-1),
		elementCount(0),
//		selectableCount(0),
		buttonCb(cb),
		Colour(colour)
    {

    }

    ~Screen() {
    }

    virtual void removeDrawable(Drawable* obj) {
        for (int i = 0; i < elementCount; ++i) {
            if (elements[i] == obj) {
                // Found the object; shift all remaining elements left
                for (int j = i; j < elementCount - 1; ++j) {
                    elements[j] = elements[j + 1];
                }

                elements[elementCount - 1] = nullptr;  // Clear last slot
                elementCount--;

                // Adjust selection if needed
                if (currentSelection == i) {
                    currentSelection = -1;
                } else if (currentSelection > i) {
                    currentSelection--;
                }

                break;
            }
        }
    }



    virtual void addDrawable(Drawable* obj) {
    	if (elementCount < MAX_SCREEN_DRAWABLES) {

			if(obj->interactable() && currentSelection == -1){
				currentSelection = elementCount;
			}

			elements[elementCount++] = obj;
		}
    }

    virtual inline UBYTE* getImage() {
    	FrameBuffer& fb = FrameBuffer::getInstance();

    	return fb.getImage();
    }

//    void renderAll(){
//
//    }

    virtual void optimizedRender() {


    	FrameBuffer& fb = FrameBuffer::getInstance();

        for (int i = 0; i < elementCount; i++) {
            elements[i]->draw(fb);
        }
    }
    virtual void cleanRender() {
    	clear(Colour);

    	//safety highlight
        if (currentSelection > -1) {
        	elements[currentSelection]->highlight(true);
        }

    	FrameBuffer& fb = FrameBuffer::getInstance();

//        }
        for (int i = 0; i < elementCount; i++) {
        	elements[i]->resetUpdated();
            elements[i]->draw(fb);
        }
    }

    virtual void clear(UWORD colour){
    	FrameBuffer& fb = FrameBuffer::getInstance();

    	fb.Paint_Clear(colour);
    }

    virtual void selectNext() {
    	if (currentSelection > -1){
    		elements[currentSelection]->highlight(false);
            int previousSelection = currentSelection;
            do{
                currentSelection = (currentSelection + 1) % elementCount;
                if(elements[currentSelection]->interactable()){
                	break;
                }
            }while(currentSelection !=  previousSelection);
            elements[currentSelection]->highlight(true);
    	}
    }

    virtual void selectPrev() {
    	if (currentSelection > -1)
		{
    		elements[currentSelection]->highlight(false);

            int previousSelection = currentSelection;
            do{
                currentSelection = (currentSelection == 0) ? elementCount - 1 : currentSelection - 1;
                if(elements[currentSelection]->interactable()){
                	break;
                }
            }while(currentSelection !=  previousSelection);

//            currentSelection = (currentSelection == 0) ? elementCount - 1 : currentSelection - 1;
            elements[currentSelection]->highlight(true);
		}
    }

    virtual void interact() {
    	if (currentSelection > -1)
		{
    		elements[currentSelection]->triggerAction();
		}

    }


    virtual void buttonPress(Button bt){
    	if(buttonCb){
    		buttonCb(bt);
    	}
    }

    virtual void setButtonCallback(Callback cb){
    	buttonCb = cb;
    }

    virtual void increaseValue(){}
    virtual void decreaseValue(){}
    virtual void cancel(){}
    virtual void save(){}
    virtual void deleteSetPoint(){}

protected:


//    subset of elements
    signed int currentSelection = -1;

    Drawable* elements[MAX_SCREEN_DRAWABLES];
    int elementCount;

//    Drawable* selectableElements[MAX_SCREEN_DRAWABLES];
//    int selectableCount;

    Callback buttonCb;
    UWORD Colour;

//    std::vector<Container *> container;

};

class SetPointScreen: public Screen {
public:
	SetPointScreen(Callback cb = nullptr):
		Screen(cb), currentSetPointElement(0), setPointElementsCount (0),isNewSetPoint(false) {
		setPointHoursContainer.addDrawable(&setPointHoursRectangle);
		setPointHoursContainer.addDrawable(&setPointHoursText);

		setPointMinutesContainer.addDrawable(&setPointMinutesRectangle);
		setPointMinutesContainer.addDrawable(&setPointMinutesText);

		setPointTempContainer.addDrawable(&setPointTempRectangle);
		setPointTempContainer.addDrawable(&setPointTempText);

		isNewSetPoint = true;
		addDrawable(&setPointHoursContainer);
		addDrawable(&setPointMinutesContainer);
		addDrawable(&setPointTempContainer);

		isNewSetPoint = false;
//		setPointScreen.addDrawable(&setPointHoursContainer);
//		setPointScreen.addDrawable(&setPointMinutesContainer);
//		setPointScreen.addDrawable(&setPointTempContainer);
//		setPointScreen.addDrawable(&SetPointC);	}
	}

    virtual void optimizedRender() {
    	FrameBuffer& fb = FrameBuffer::getInstance();

        for (int i = 0; i < setPointElementsCount; i++) {
        	setPointElements[i]->draw(fb);
        }
    	Screen::optimizedRender();
    }
    virtual void cleanRender() {
    	Screen::cleanRender();

    	FrameBuffer& fb = FrameBuffer::getInstance();
    	setPointUpdate();

        for (int i = 0; i < setPointElementsCount; i++) {
        	setPointElements[i]->resetUpdated();
        	setPointElements[i]->draw(fb);
        }
    }

    virtual void interact() override{
    	if(isNewSetPoint){
//    		if( setPointSelection > -1){
    			setPointElements[currentSetPointElement]->triggerAction();
//    		}
    	}
    	else{
    		Screen::interact();
    	}

    }

    virtual void highlightCurrentElement(bool highlight){

    }

    void setPointUpdate() {
    	int currSelection = Screen::currentSelection;
    	DaySchedule day = schedule[currentDay];
    	if(currSelection < day.setpointCount){
    		SetPointData data = day.setpoints[currSelection];
    		minutes = data.minute;
    		temperature = data.temperature;
    		hours = data.hour;



    	}else{
    		minutes = 0;
    		temperature = 22;
    		hours = 11;

    	}
    	setPointUpdateElements();
    }

    void setPointUpdateElements(){
    	snprintf(hoursBuffer, sizeof(hoursBuffer), "%d", hours);
    	setPointHoursContainer.resetClear();
		setPointHoursText.setString(hoursBuffer);

		lukeisfuckingretardisthehoelessmostincooherenceioadnawdnowa(temperatureBuffer, sizeof(temperatureBuffer), temperature);

//    	snprintf(temperatureBuffer, sizeof(temperatureBuffer), "%d", (int)temperature);
    	setPointTempContainer.resetClear();
    	setPointTempText.setString(temperatureBuffer);

		snprintf(minutesBuffer, sizeof(minutesBuffer), "%d", minutes);
    	setPointMinutesContainer.resetClear();
		setPointMinutesText.setString(minutesBuffer);


		setPointAMText.setString(isAm ? "AM":"PM");
    }

    virtual void selectPrev() override{
    	if(isNewSetPoint){
				setPointElements[currentSetPointElement]->highlight(false);

				if(currentSetPointElement == 0){
					isNewSetPoint = false;
					return; //?
				}

				currentSetPointElement = currentSetPointElement - 1;
				setPointElements[currentSetPointElement]->highlight(true);
    	}
    	else {
    		Screen::selectPrev();
    	}
    	currentSetPointSelection = currentSelection;
    	setPointUpdate();
    }

    virtual void selectNext() override {
    	if(isNewSetPoint){
			setPointElements[currentSetPointElement]->highlight(false);

			currentSetPointElement = (currentSetPointElement + 1) % setPointElementsCount;

			setPointElements[currentSetPointElement]->highlight(true);

    	}
    	else {
    		Screen::selectNext();
    	}
    	currentSetPointSelection = currentSelection;

    	setPointUpdate();
    }

    void refreshBoxes(){
    	setPointTempRectangle.resetClear();
    	setPointHoursRectangle.resetClear();
    	setPointMinutesRectangle.resetClear();
    	setPointUpdate();

    }

    virtual void addDrawable(Drawable* obj) {
    	if(isNewSetPoint){
    		if (setPointElementsCount < MAX_SCREEN_DRAWABLES) {

				if(obj->interactable() && currentSetPointElement == -1){
					currentSetPointElement = setPointElementsCount;
			    	currentSetPointSelection = currentSelection;

				}

				setPointElements[setPointElementsCount++] = obj;
			}
    	}
    	else{
    		Screen::addDrawable(obj);
    	}

    }

    void setSetPoint(bool setSetPoint){
    	isNewSetPoint = setSetPoint;
    }

    void addSetpoint(){
    	currentSetPointElement = 0;
    	isNewSetPoint = true;
    }
    void editCurrentSetPoint(){
    	currentSetPointElement = 0;
    	isNewSetPoint = true;
//    	setPoin
		setPointElements[currentSetPointElement]->highlight(true);
    	setPointUpdate();
    }

    virtual void increaseValue(){
    	if(currentSetPointElement ==0){
    		hours = (hours+1) % 24;
    	}
    	else if(currentSetPointElement ==1){
    		minutes = (minutes+5) % 60;
//    		hours+=5;
    	}
    	else if(currentSetPointElement ==2){
    		isAm = !isAm;
    	}
    	else{
    		temperature = (temperature + 0.5 > 30.0 ? 15.0: temperature+0.5);
    	}
    	setPointUpdateElements();
    }
    virtual void decreaseValue(){
    	if(currentSetPointElement ==0){
    		hours = (hours < 0 ? 23 : hours -1);
    	}
    	else if(currentSetPointElement ==1){
    		minutes = (minutes < 0 ? 55 : minutes -5);
    	}
    	else if(currentSetPointElement ==2){
    		isAm = !isAm;
    	}
    	else{
    		temperature  = (temperature < 7 ? 30 : temperature - 0.5);
    	}
    	setPointUpdateElements();
    }
    virtual void cancel(){
    	setPointUpdate();
		setPointElements[currentSetPointElement]->highlight(false);
    	isNewSetPoint = false;

    }

    void remove(){
		setPointElements[currentSetPointElement]->highlight(false);
    	isNewSetPoint = false;
    }

    virtual void save(){
		if (currentDay < 0 || currentDay >= DAYS_IN_WEEK)
			return ;  // out of bounds

		DaySchedule& day = schedule[currentDay];

		if (day.setpointCount >= MAX_SETPOINTS_PER_DAY)
			return ;  // no room left

		day.setpoints[currentSetPointSelection] = {temperature,hours,minutes,isAm};
//		return true;
//		if(currentSetPointSelection +1
		if(currentSetPointSelection == day.setpointCount){
			day.setpointCount++;
		}
//		}
		updateSetPointDynamicElements(currentDay);
//		day.setpointCount++;


    }
    virtual void deleteSetPoint(){}

private:


    char hoursBuffer[10];
    char isAmBuffer[10];
    char temperatureBuffer[10];
    char minutesBuffer[10];

    int minutes;
	int hours;
    float temperature;
    bool isAm;

    Container setPointHoursContainer = Container(340,60,120,130);
    HighLightOnInteractRectangle setPointHoursRectangle = HighLightOnInteractRectangle(0, 0, 120, 131, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);;
    HighlightableDrawText setPointHoursText = HighlightableDrawText(20,20,"",&Font24, WHITE, BLACK);

//    char minutesBuffer[50];
    Container setPointMinutesContainer = Container(502,60,120,130);
    HighLightOnInteractRectangle setPointMinutesRectangle = HighLightOnInteractRectangle(0, 0, 120, 131, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);;
    HighlightableDrawText setPointMinutesText = HighlightableDrawText(20,20,"",&Font24, WHITE, BLACK);

//    char tempBuffer[50];
    Container setPointTempContainer = Container(502,240,120,131);
    HighLightOnInteractRectangle setPointTempRectangle = HighLightOnInteractRectangle(0, 0, 120, 131, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);;
    HighlightableDrawText setPointTempText = HighlightableDrawText(20,20,"",&Font24, WHITE, BLACK);

//    Container setPointTempContainer = Container(502,240,120,131);
//    HighLightOnInteractRectangle setPointTempRectangle = HighLightOnInteractRectangle(0, 0, 120, 131, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);;
    HighlightableDrawText setPointAMText = HighlightableDrawText(20,20,"",&Font24, WHITE, BLACK);

    bool isNewSetPoint;

    int currentSetPointElement;
	Drawable* setPointElements[MAX_SCREEN_DRAWABLES];
	int setPointElementsCount;
//	bool isNewSetPoint;
};

#endif /* INC_SCREEN_H_ */
