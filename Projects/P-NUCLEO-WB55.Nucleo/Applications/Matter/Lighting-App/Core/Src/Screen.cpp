/*
 * Screen.cpp
 *
 *  Created on: Feb 24, 2025
 *      Author: kobru
 */

#include "Screen.hpp"

Screen::Screen(Callback cb , UWORD Width_ , UWORD Height_  , UWORD colour , UWORD Rotate):
	currentSelection(-1),
	elementCount(0),
	buttonCb(cb),
	Colour(colour)
{

}

Screen::~Screen() {
}

void Screen::removeDrawable(Drawable* obj) {
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



void Screen::addDrawable(Drawable* obj) {
	if (elementCount < MAX_SCREEN_DRAWABLES) {

		if(obj->interactable() && currentSelection == -1){
			currentSelection = elementCount;
		}

		elements[elementCount++] = obj;
	}
}

UBYTE* Screen::getImage() {
	FrameBuffer& fb = FrameBuffer::getInstance();

	return fb.getImage();
}

//    void renderAll(){
//
//    }

void Screen::optimizedRender() {


	FrameBuffer& fb = FrameBuffer::getInstance();

	for (int i = 0; i < elementCount; i++) {
		elements[i]->draw(fb);
	}
}
void Screen::cleanRender() {
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

void Screen::clear(UWORD colour){
	FrameBuffer& fb = FrameBuffer::getInstance();

	fb.Paint_Clear(colour);
}

void Screen::selectNext() {
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

void Screen::selectPrev() {
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

void Screen::interact() {
	if (currentSelection > -1)
	{
		elements[currentSelection]->triggerAction();
	}

}


void Screen::buttonPress(Button bt){
	if(buttonCb){
		buttonCb(bt);
	}
}
void Screen::setButtonCallback(Callback cb){
	buttonCb = cb;
}

void Screen::increaseValue(){}
void Screen::decreaseValue(){}
void Screen::cancel(){}
void Screen::save(){}
void Screen::deleteSetPoint(){}

