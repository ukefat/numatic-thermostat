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

    void removeDrawable(Drawable* obj) {
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



    void addDrawable(Drawable* obj) {
    	if (elementCount < MAX_SCREEN_DRAWABLES) {

			if(obj->interactable() && currentSelection == -1){
				currentSelection = elementCount;
			}

			elements[elementCount++] = obj;
		}
    }

    inline UBYTE* getImage() {
    	FrameBuffer& fb = FrameBuffer::getInstance();

    	return fb.getImage();
    }

    void optimizedRender() {


    	FrameBuffer& fb = FrameBuffer::getInstance();

        for (int i = 0; i < elementCount; i++) {
            elements[i]->draw(fb);
        }
    }
    void cleanRender() {
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

    void clear(UWORD colour){
    	FrameBuffer& fb = FrameBuffer::getInstance();

    	fb.Paint_Clear(colour);
    }

    void selectNext() {
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

    void selectPrev() {
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

    void interact() {
    	if (currentSelection > -1)
		{
    		elements[currentSelection]->triggerAction();
		}

    }


    void buttonPress(Button bt){
    	if(buttonCb){
    		buttonCb(bt);
    	}
    }

    void setButtonCallback(Callback cb){
    	buttonCb = cb;
    }



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

//
//class SetPointScreen: public Screen {
//
//private:
//	std::unique_ptr<Drawable, FreeRTOSDeleter> dynamicDrawables;
//};



#endif /* INC_SCREEN_H_ */
