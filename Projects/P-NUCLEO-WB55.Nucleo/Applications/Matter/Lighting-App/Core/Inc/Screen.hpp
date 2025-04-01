/*
 * Screen.h
 *
 *  Created on: Feb 24, 2025
 *      Author: kobru
 */

#ifndef INC_SCREEN_HPP_
#define INC_SCREEN_HPP_

//#include <vector>
#include <Container.hpp>
#include <Drawable.hpp>
#include <EPD_4in26.hpp>
#include <EPD_test.hpp>
#include <FrameBuffer.hpp>
#include "TextContainer.hpp"
#include "Error.hpp"
#include "ButtonContainer.hpp"
#define MAX_SCREEN_DRAWABLES 10 // Define a reasonable limit for the array

#include "DrawableObjects.hpp"
//#include

extern DaySchedule schedule[];
extern UBYTE currentDay;
extern UBYTE currentSetPointSelection;
class Screen {
public:
	using Callback = void (*)(Button button);

    Screen(Callback cb = nullptr, UWORD Width_ = EPD_4in26_WIDTH, UWORD Height_  = EPD_4in26_HEIGHT, UWORD colour = WHITE, UWORD Rotate= ROTATE_0);

    ~Screen();

    virtual void removeDrawable(Drawable* obj);



    virtual void addDrawable(Drawable* obj);

    virtual inline UBYTE* getImage();

    virtual void optimizedRender();
    virtual void cleanRender();

    virtual void clear(UWORD colour);

    virtual void selectNext();

    virtual void selectPrev();

    virtual void interact();


    virtual void buttonPress(Button bt);

    virtual void setButtonCallback(Callback cb);

    virtual void increaseValue();
    virtual void decreaseValue();
    virtual void cancel();
    virtual void save();
    virtual void deleteSetPoint();

    virtual void deleteSelection(){};

protected:

    signed int currentSelection = -1;

    Drawable* elements[MAX_SCREEN_DRAWABLES];
    int elementCount;
    Callback buttonCb;
    UWORD Colour;


};

class SetPointScreen: public Screen {
public:
	SetPointScreen(Callback cb = nullptr);

    virtual void optimizedRender() ;
    virtual void cleanRender();

    virtual void interact() override;


    void setPointUpdate();

    void setPointUpdateElements();

    virtual void selectPrev() override;

    virtual void selectNext() override;



    virtual void addDrawable(Drawable* obj) ;

    void setSetPoint(bool setSetPoint);

    void addSetpoint();
    void editCurrentSetPoint();

    virtual void increaseValue();
    virtual void decreaseValue();
    virtual void cancel() override;

    void remove();

    virtual void save();
    virtual void deleteSetPoint();

private:


    char hoursBuffer[10];
    char isAmBuffer[10];
    char temperatureBuffer[10];
    char minutesBuffer[10];

    int minutes;
	int hours;
    float temperature;
    bool isAm;

    Container setPointHoursContainer = Container(360,60,120,130);
    HighLightOnInteractRectangle setPointHoursRectangle = HighLightOnInteractRectangle(0, 0, 160, 131, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);;
    HighlightableDrawText setPointHoursText = HighlightableDrawText(23,30,"","Century90limited", WHITE, BLACK);

//    char minutesBuffer[50];
    Container setPointMinutesContainer = Container(520,60,120,130);
    HighLightOnInteractRectangle setPointMinutesRectangle = HighLightOnInteractRectangle(0, 0, 160, 131, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);;
    HighlightableDrawText setPointMinutesText = HighlightableDrawText(25,30,"","Century90limited", WHITE, BLACK);

//    char tempBuffer[50];
    Container setPointTempContainer = Container(480,240,120,131);
    HighLightOnInteractRectangle setPointTempRectangle = HighLightOnInteractRectangle(0, 0, 200, 131, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);;
    HighlightableDrawText setPointTempText = HighlightableDrawText(13,30,"","Century90limited", WHITE, BLACK);

//    Container setPointTempContainer = Container(502,240,120,131);
//    HighLightOnInteractRectangle setPointTempRectangle = HighLightOnInteractRectangle(0, 0, 120, 131, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);;
    HighlightableDrawText setPointAMText = HighlightableDrawText(20,20,"","Century90limited", WHITE, BLACK);

    bool isNewSetPoint;

    int currentSetPointElement;
	Drawable* setPointElements[MAX_SCREEN_DRAWABLES];
	int setPointElementsCount;
//	bool isNewSetPoint;
};


//struct


class WarningScreen: public Screen {
public:
	WarningScreen(Callback cb = nullptr):
		Screen(cb), dismiss(331,415,138,65,"Dismiss", nullptr), back(169,415,138,65, "Back", nullptr),
		warningMessages{
		InteractableTextContainer(0, 50 , 300, 50, ""),
		InteractableTextContainer(0, 100 , 300, 50, ""),
		InteractableTextContainer(0, 150 , 300, 50, ""),
		InteractableTextContainer(0, 200 , 300, 50, ""),
		InteractableTextContainer(0, 250 , 300, 50, "")
	},
	warningHeader(329,43,400,50,"Error 1"),
	warningText(324,107,400,50,"Error description")
	{



	   // Get reference to the error model singleton
		ErrorModel& model = ErrorModel::getInstance();
		uint8_t errorCount = model.getErrorCount();

		for (int i = 0; i < MAX_ERRORS; ++i) {
			if (i < errorCount) {
				const Error* err = model.getError(i);
				if (err) {
					warningMessages[i].setInteractability(true);
					warningMessages[i].setText(err->getHeader());
				}
			} else {
				warningMessages[i].setText(""); // Clear unused slots
				warningMessages[i].setInteractability(false);

			}
			addDrawable(&warningMessages[i]);


		}
		addDrawable(&dismiss);
		addDrawable(&back);

		if(Screen::currentSelection > -1) {
			const Error* err = model.getError(Screen::currentSelection);

			warningHeader.setText(err->getHeader());
			warningText.setText(err->getText());
		}
		addDrawable(&warningHeader);
		addDrawable(&warningText);

	}


	void selectNext(){

		Screen::selectNext();

		ErrorModel& model = ErrorModel::getInstance();

		if(Screen::currentSelection > -1) {
			const Error* err = model.getError(Screen::currentSelection);

			warningHeader.setText(err->getHeader());
			warningText.setText(err->getText());
		}
	}
	void selectPrev(){

		Screen::selectPrev();

		ErrorModel& model = ErrorModel::getInstance();

		if(Screen::currentSelection > -1) {
			const Error* err = model.getError(Screen::currentSelection);

			warningHeader.setText(err->getHeader());
			warningText.setText(err->getText());
		}
	}


	void deleteSelection() override {
		ErrorModel& model = ErrorModel::getInstance();
//		uint8_t errorCount = model.getErrorCount();


		if(Screen::currentSelection > -1) {
			model.removeErrorAt(Screen::currentSelection);

			uint8_t errorCount = model.getErrorCount();

			for (int i = currentSelection; i < MAX_ERRORS; ++i) {
				if (i < errorCount) {
					const Error* err = model.getError(i);
					if (err) {
						warningMessages[i].setInteractability(true);
						warningMessages[i].setText(err->getHeader());
						warningMessages[i].resetClear();
					}
				} else {
					warningMessages[i].setText(""); // Clear unused slots
					warningMessages[i].resetClear();
					warningMessages[i].setInteractability(false);

				}

			}

			if (errorCount  == currentSelection){
				currentSelection -= 1;
				elements[currentSelection +1]->highlight(false);
				if(currentSelection > -1){
					elements[currentSelection ]->highlight(true);
				}
			}


			if(Screen::currentSelection > -1) {
				const Error* err = model.getError(Screen::currentSelection);

				warningHeader.setText(err->getHeader());
				warningText.setText(err->getText());
			}else {
				warningHeader.setText("");
				warningText.setText("");
			}

		}
	}
private:
	ButtonContainer dismiss;
	ButtonContainer back;

	InteractableTextContainer warningMessages[5];


	TextContainer warningHeader;
	TextContainer warningText;
};

#endif /* INC_SCREEN_HPP_ */
