#include <EPD_4in26.hpp>
#include "FrameBuffer.h"
#include "Screen.h"
#include "Rectangle.h"
#include "Circle.h"
#include "main.h"
#include "ScreenManager.h"
#include <queue>
#include "Debug.h"
#include "Container.h"
#include "BitMap.hpp"
#include "image_data.hpp"
#include "DrawText.hpp"
#include <stdio.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "EPD_test.hpp"
//#include "TextContainer.h"

#define Imagesize (((EPD_4in26_WIDTH % 8 == 0)? (EPD_4in26_WIDTH / 8 ): (EPD_4in26_WIDTH / 8 + 1)) * EPD_4in26_HEIGHT)
//static UBYTE BlackImage[Imagesize];

//static UBYTE screen1img[Imagesize];
//static UBYTE frameBufferScreen[Imagesize];

extern SPI_HandleTypeDef hspi1;
extern QueueHandle_t buttonQueue;
extern QueueHandle_t stateQueue;
extern QueueHandle_t dataSetPointOperationQueue;


#define DAYS_IN_WEEK 7
#define MAX_SETPOINTS_PER_DAY 3
//#define MAX


UWORD setPointContainerHeightIndent = 15;
UWORD setPointContainerWidth = 307;
UWORD setPointContainerHeight = 78;



typedef struct {
	char setPointBuffer[50];
	Container container = Container(0,0,0,0);
	HighLightOnInteractRectangle rectangle = HighLightOnInteractRectangle(0, 0, setPointContainerWidth, setPointContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);;
	HighlightableDrawText text = HighlightableDrawText(20,20,setPointBuffer,&Font24, WHITE, BLACK);
} SetPointContainer;


typedef struct {
	SetPointData setpoints[MAX_SETPOINTS_PER_DAY];
    uint8_t setpointCount;
} DaySchedule;

static DaySchedule schedule[DAYS_IN_WEEK] = {
    {{ {22.0, 6, 30}, {24.0, 18, 0} }, 2},  // Monday
    {{ {21.5, 7, 0} }, 1},  // Tuesday
    {{ {22.5, 6, 45}, {24.5, 17, 30} }, 2},  // Wednesday
    {{ {20.0, 5, 30}, {23.0, 16, 30} }, 2},  // Thursday
    {{ {22.0, 6, 0}, {24.0, 20, 0} }, 2},  // Friday
    {{ {21.0, 8, 0}, {23.0, 21, 0} }, 2},  // Saturday
    {{ {20.5, 9, 0}, {22.5, 22, 0} }, 2}   // Sunday
};

static SetPointContainer setPointOptions[MAX_SETPOINTS_PER_DAY];
static UBYTE currentDay = 99;
static UBYTE currentSetPointSelection = 0;

//typedef struct SetPointData{
//    float temperature;
//    uint8_t hour;
//    uint8_t minute;
//    bool AM;
//} NewSetPointData;


PAINT_TIME time = {9999, 12,30,23,59,59,false,false};


float setPoint = 23.5;
char buffer[50]; // Buffer for formatted string

float temparature = 24.5;
char temparatureBuffer[50]; // Buffer for formatted string

UWORD containerYLength = 100;
UWORD containerYStart = EPD_4in26_HEIGHT - containerYLength;

UWORD ContainerxLength = 150;
UWORD ContainerxStart = 325;



DrawText timeText = DrawText(338,36,&time,&Font24, WHITE, BLACK);

BitMap battery = BitMap(gImage_battery, 0, 0, 64, 64, WHITE);

BitMap wifi = BitMap(gImage_wifi,730,0, 64,64,WHITE);

Screen setPointScreen = Screen();


EPD_4in26 ePaperGlobal(RST_GPIO_Port, RST_Pin,
						    		DC_GPIO_Port, DC_Pin,
									SPI_CS_GPIO_Port, SPI_CS_Pin,
						    		BUSY_GPIO_Port, BUSY_Pin,
						    		PWR_GPIO_Port, PWR_Pin,
									&hspi1
									);


void updateSetPointMainArea(Screen scr){

}

void updateSetPointDynamicElements(UBYTE index){
	currentDay = index;
	DaySchedule day = schedule[index];
	for(int i =0; i < MAX_SETPOINTS_PER_DAY; i ++){

		SetPointContainer& container = setPointOptions[i];
		SetPointData data = day.setpoints[i];

		if(i < day.setpointCount){
	        //snprintf(container.setPointBuffer, sizeof(container.setPointBuffer), "%02d:%02d - %.1fC", data.hour, data.minute, data.temperature);
	        container.text.updatedText();
	        container.container.addCallback([](){

//	    		xQueueSend(dataSetPointOperationQueue, &data, portMAX_DELAY);

	        });
		}

		else if(i == day.setpointCount ){
	        //snprintf(container.setPointBuffer, sizeof(container.setPointBuffer), "Add + SetPoint");
	        container.text.updatedText();
	        container.container.addCallback([](){

	        });
		} else {
	        //snprintf(container.setPointBuffer, sizeof(container.setPointBuffer), "\0");
	        container.text.updatedText();
		}


	}
}





void setPointInitializeConnections(Screen & setPointScreen){
//	DaySchedule day = schedule[index];
	UWORD setPointContainerHeightIndent = 15;
	UWORD setPointContainerWidth = 307;
	UWORD setPointContainerHeight = 78;



	for(int i =0; i < MAX_SETPOINTS_PER_DAY; i++){

		SetPointContainer& setPointContaienr = setPointOptions[i];
		setPointContaienr.container.setXStart(0);
		setPointContaienr.container.setYStart(setPointContainerHeightIndent + setPointContainerHeight *i);
		setPointContaienr.container.setXEnd(setPointContainerWidth);
		setPointContaienr.container.setYEnd(setPointContainerHeight);


		setPointContaienr.container.addDrawable(&setPointContaienr.rectangle);
		setPointContaienr.container.addDrawable(&setPointContaienr.text);

		setPointScreen.addDrawable(&setPointContaienr.container);
	}

//	setPointScreen

}



void drawAlertScreen(Screen& alertScreen) {
	UWORD containerWidth = 138;
	UWORD containerHeight = 65;

	UWORD scheduleContainerHeightIndent = 15;
	UWORD scheduleContainerWidth = 307;
	UWORD scheduleContainerHeight = 56;

	Container BackContainer = Container(169,415,containerWidth,containerHeight);
	Rectangle backRectangle =  Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText backText =  DrawText(20,20,"Back",&Font16, WHITE, BLACK);

	BackContainer.addDrawable(&backRectangle);
	BackContainer.addDrawable(&backText);
//
	Container homeContainer = Container(331,415,containerWidth,containerHeight);
//		Rectangle* homeRectangle = new Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText homeText = DrawText(20,20,"Home",&Font16, WHITE, BLACK);

	homeContainer.addDrawable(&backRectangle);
	homeContainer.addDrawable(&homeText);

	Container dismissContainer = Container(493,415,containerWidth,containerHeight);
//		Rectangle* selectRectangle = new Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText selectText = DrawText(20,20,"Select",&Font16, WHITE, BLACK);

	dismissContainer.addDrawable(&backRectangle);
	dismissContainer.addDrawable(&selectText);

	UWORD mainMenuWidth = 747;
	UWORD mainMenuHeight = 93;


	Container Alert1 = Container(0, scheduleContainerHeightIndent, scheduleContainerWidth, scheduleContainerHeight, [](){

	} );
	HighLightOnInteractRectangle Alert1Rectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText Alert1Text = HighlightableDrawText(20,20,"Heating Warning",&Font24, WHITE, BLACK);

	Alert1.addDrawable(&Alert1Rectangle);
	Alert1.addDrawable(&Alert1Text);

	Container Alert2Container = Container(0, scheduleContainerHeightIndent + scheduleContainerHeight * 1, scheduleContainerWidth, scheduleContainerHeight, [](){

	}  );
	HighLightOnInteractRectangle Alert2Rectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText Alert2Text = HighlightableDrawText(20,20,"Leaking Pipes",&Font24, WHITE, BLACK);

	Alert2Container.addDrawable(&Alert2Rectangle);
	Alert2Container.addDrawable(&Alert2Text);





	alertScreen.addDrawable(&BackContainer);
	alertScreen.addDrawable(&homeContainer);
	alertScreen.addDrawable(&dismissContainer);

	alertScreen.addDrawable(&Alert1);
	alertScreen.addDrawable(&Alert2Container);



}
void drawClockDateScreen(Screen& clockDateScreen){
	UWORD clockContainerWidth = 138;
	UWORD ClockContainerHeight = 65;


	Container clockLeftContainer = Container(169,415,clockContainerWidth,ClockContainerHeight);
	Rectangle clockButtonRectangle =  Rectangle(0, 0, clockContainerWidth, ClockContainerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText clockLeftText =  DrawText(20,20,"Back",&Font16, WHITE, BLACK);

	Container clockMiddleContainer = Container(331,415,clockContainerWidth,ClockContainerHeight);
	DrawText clockMiddleText= DrawText(20,20,"Home",&Font16, WHITE, BLACK);

	Container clockRightContainer = Container(493,415,clockContainerWidth,ClockContainerHeight);
	DrawText clockRightContainerText = DrawText(20,20,"Select",&Font16, WHITE, BLACK);


	clockDateScreen.addDrawable(&clockLeftContainer);
	clockDateScreen.addDrawable(&clockRightContainer);
	clockDateScreen.addDrawable(&clockMiddleContainer);

}


void setPointScreenCallback1(Button bt){
	State currState = State::Error;
	switch (bt){
	case Button::Middle:
		currState = State::Delete;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		break;
	case Button::Right:
		currState = State::Interact;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		break;
	case Button::Left:
		currState = State::PrevScreen;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
	    break;
	case Button::Up:
		currState = State::PrevElement;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
	    break;
	case Button::Down:
		currState = State::NextElement;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
	    break;
	default:
		break;
	}
}


void setPointScreenCallback2(Button bt){
	State currState = State::Error;
	switch (bt){
	case Button::Middle:
		currState = State::Delete;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		break;
	case Button::Right:
		currState = State::Interact;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		break;
	case Button::Left:
		currState = State::PrevScreen;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
	    break;
	case Button::Up:
		currState = State::PrevElement;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
	    break;
	case Button::Down:
		currState = State::NextElement;
	    xQueueSend(stateQueue, &currState, portMAX_DELAY);
	    break;
	default:
		break;
	}
}
void EPD_MainMenuWithQueue(){




	ScreenManager screenManager(&ePaperGlobal);

	Screen ClockDateScreen = Screen();

	Screen alertScreen = Screen();

	Screen openingScreen = Screen();


	Container container1 = Container(ContainerxStart,containerYStart,138,65, []() {
			State state = State::MainMenuScreen;
		    xQueueSend(stateQueue, &state, portMAX_DELAY);
		});

	Rectangle rect1 = Rectangle(0, 0, ContainerxLength, containerYLength, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

	BitMap bitmap = BitMap(gImage_icon, 27, 2, 96, 96, WHITE);

	//snprintf(buffer, sizeof(buffer), "Setpoint: %.1fC", setPoint);
	//snprintf(temparatureBuffer, sizeof(temparatureBuffer), "Temperature: %.1fC", temparature);

	DrawText setPointText = DrawText(358,121,buffer,&Font16, WHITE, BLACK);
	DrawText actualTemperature = DrawText(302,174,temparatureBuffer,&Font24, WHITE, BLACK);


	container1.addDrawable(&rect1);
	container1.addDrawable(&bitmap);
	openingScreen.addDrawable(&container1);
	openingScreen.addDrawable(&setPointText);
	openingScreen.addDrawable(&actualTemperature);
	openingScreen.addDrawable(&timeText);
	openingScreen.addDrawable(&battery);
	openingScreen.addDrawable(&wifi);







	Screen mainMenu = Screen();



	UWORD containerWidth = 138;
	UWORD containerHeight = 65;

	Container BackContainer = Container(169,415,containerWidth,containerHeight);
	Rectangle backRectangle =  Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText backText =  DrawText(20,20,"Back",&Font16, WHITE, BLACK);

	BackContainer.addDrawable(&backRectangle);
	BackContainer.addDrawable(&backText);
//
	Container homeContainer = Container(331,415,containerWidth,containerHeight);
//		Rectangle* homeRectangle = new Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText homeText = DrawText(20,20,"Home",&Font16, WHITE, BLACK);

	homeContainer.addDrawable(&backRectangle);
	homeContainer.addDrawable(&homeText);

	Container selectContainer = Container(493,415,containerWidth,containerHeight);
//		Rectangle* selectRectangle = new Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText selectText = DrawText(20,20,"Select",&Font16, WHITE, BLACK);

	selectContainer.addDrawable(&backRectangle);
	selectContainer.addDrawable(&selectText);

	UWORD mainMenuWidth = 747;
	UWORD mainMenuHeight = 93;


	Container scheduleContainer = Container(26,87,mainMenuWidth,mainMenuHeight, [](){
		State state = State::ScheduleScreen;
		xQueueSend(stateQueue, &state, portMAX_DELAY);
	});
	HighLightOnInteractRectangle scheduleRectangle = HighLightOnInteractRectangle(0, 0, mainMenuWidth, mainMenuHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText scheduleText = HighlightableDrawText(20,20,"Schedule",&Font24, WHITE, BLACK);

	scheduleContainer.addDrawable(&scheduleRectangle);
	scheduleContainer.addDrawable(&scheduleText);

	Container ClockContainer = Container(26,180,mainMenuWidth,mainMenuHeight, [](){

	});
	HighLightOnInteractRectangle ClockRectangle = HighLightOnInteractRectangle(0, 0, mainMenuWidth, mainMenuHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText ClockText = HighlightableDrawText(20,20,"Clock/Date",&Font24, WHITE, BLACK);

	ClockContainer.addDrawable(&ClockRectangle);
	ClockContainer.addDrawable(&ClockText);

	Container AlertContainer = Container(26,273,mainMenuWidth,mainMenuHeight, [](){

	});
	HighLightOnInteractRectangle AlertRectangle = HighLightOnInteractRectangle(0, 0, mainMenuWidth, mainMenuHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText AlertText = HighlightableDrawText(20,20,"Alerts",&Font24, WHITE, BLACK);

	AlertContainer.addDrawable(&AlertRectangle);
	AlertContainer.addDrawable(&AlertText);

	DrawText timeTextMainMenu = DrawText(338,20,&time,&Font24, WHITE, BLACK);



	mainMenu.addDrawable(&BackContainer);
	mainMenu.addDrawable(&homeContainer);
	mainMenu.addDrawable(&selectContainer);

	mainMenu.addDrawable(&scheduleContainer);
	mainMenu.addDrawable(&ClockContainer);
	mainMenu.addDrawable(&AlertContainer);


	mainMenu.addDrawable(&timeText);
	mainMenu.addDrawable(&battery);
	mainMenu.addDrawable(&wifi);


	Screen scheduleScreen = Screen();

	Container BackContainerScheduleScreen = Container(169,415,containerWidth,containerHeight);

	BackContainerScheduleScreen.addDrawable(&backRectangle);
	BackContainerScheduleScreen.addDrawable(&backText);

	Container homeContainerScheduleScreen = Container(331,415,containerWidth,containerHeight);

	homeContainerScheduleScreen.addDrawable(&backRectangle);
	homeContainerScheduleScreen.addDrawable(&homeText);

	Container selectContainerScheduleScreen = Container(493,415,containerWidth,containerHeight);

	selectContainerScheduleScreen.addDrawable(&backRectangle);
	selectContainerScheduleScreen.addDrawable(&selectText);


	UWORD scheduleContainerHeightIndent = 15;
	UWORD scheduleContainerWidth = 307;
	UWORD scheduleContainerHeight = 56;



	Container mondayContainer = Container(0, scheduleContainerHeightIndent, scheduleContainerWidth, scheduleContainerHeight, [](){
		updateSetPointDynamicElements(0);
		State state = State::SetPointScreen;
		xQueueSend(stateQueue, &state, portMAX_DELAY);
	} );
	HighLightOnInteractRectangle mondayRectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText mondayText = HighlightableDrawText(20,20,"Monday",&Font24, WHITE, BLACK);

	mondayContainer.addDrawable(&mondayRectangle);
	mondayContainer.addDrawable(&mondayText);

	Container tuesdayContainer = Container(0, scheduleContainerHeightIndent + scheduleContainerHeight * 1, scheduleContainerWidth, scheduleContainerHeight, [](){
		updateSetPointDynamicElements(1);
		State state = State::SetPointScreen;
		xQueueSend(stateQueue, &state, portMAX_DELAY);
	}  );
	HighLightOnInteractRectangle tuesdayRectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText tuesdayText = HighlightableDrawText(20,20,"Tuesday",&Font24, WHITE, BLACK);

	tuesdayContainer.addDrawable(&tuesdayRectangle);
	tuesdayContainer.addDrawable(&tuesdayText);

	Container wednesdayContainer = Container(0, scheduleContainerHeightIndent + scheduleContainerHeight * 2, scheduleContainerWidth, scheduleContainerHeight, [](){
		updateSetPointDynamicElements(2);
		State state = State::SetPointScreen;
		xQueueSend(stateQueue, &state, portMAX_DELAY);
	}  );
	HighLightOnInteractRectangle wednesdayRectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText wednesdayText = HighlightableDrawText(20,20,"Wednesday",&Font24, WHITE, BLACK);

	wednesdayContainer.addDrawable(&wednesdayRectangle);
	wednesdayContainer.addDrawable(&wednesdayText);

	Container thursdayContainer = Container(0, scheduleContainerHeightIndent + scheduleContainerHeight * 3, scheduleContainerWidth, scheduleContainerHeight, [](){
		updateSetPointDynamicElements(3);
		State state = State::SetPointScreen;
		xQueueSend(stateQueue, &state, portMAX_DELAY);
	}  );
	HighLightOnInteractRectangle thursdayRectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText thursdayText = HighlightableDrawText(20,20,"Thursday",&Font24, WHITE, BLACK);

	thursdayContainer.addDrawable(&thursdayRectangle);
	thursdayContainer.addDrawable(&thursdayText);

	Container fridayContainer = Container(0, scheduleContainerHeightIndent + scheduleContainerHeight * 4, scheduleContainerWidth, scheduleContainerHeight, [](){
		updateSetPointDynamicElements(4);
		State state = State::SetPointScreen;
		xQueueSend(stateQueue, &state, portMAX_DELAY);
	}  );
	HighLightOnInteractRectangle fridayRectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText fridayText = HighlightableDrawText(20,20,"Friday",&Font24, WHITE, BLACK);

	fridayContainer.addDrawable(&fridayRectangle);
	fridayContainer.addDrawable(&fridayText);

	Container saturdayContainer = Container(0, scheduleContainerHeightIndent + scheduleContainerHeight * 5, scheduleContainerWidth, scheduleContainerHeight, [](){
		updateSetPointDynamicElements(5);
		State state = State::SetPointScreen;
		xQueueSend(stateQueue, &state, portMAX_DELAY);
	}  );
	HighLightOnInteractRectangle saturdayRectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText saturdayText = HighlightableDrawText(20,20,"Saturday",&Font24, WHITE, BLACK);

	saturdayContainer.addDrawable(&saturdayRectangle);
	saturdayContainer.addDrawable(&saturdayText);

	Container sundayContainer = Container(0, scheduleContainerHeightIndent + scheduleContainerHeight * 6, scheduleContainerWidth, scheduleContainerHeight, [](){
		updateSetPointDynamicElements(6);
		State state = State::SetPointScreen;
		xQueueSend(stateQueue, &state, portMAX_DELAY);
	}  );
	HighLightOnInteractRectangle sundayRectangle = HighLightOnInteractRectangle(0, 0, scheduleContainerWidth, scheduleContainerHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	HighlightableDrawText sundayText = HighlightableDrawText(20,20,"Sunday",&Font24, WHITE, BLACK);

	sundayContainer.addDrawable(&sundayRectangle);
	sundayContainer.addDrawable(&sundayText);

	scheduleScreen.addDrawable(&BackContainerScheduleScreen);
	scheduleScreen.addDrawable(&homeContainerScheduleScreen);
	scheduleScreen.addDrawable(&selectContainerScheduleScreen);

	scheduleScreen.addDrawable(&mondayContainer);
	scheduleScreen.addDrawable(&tuesdayContainer);
	scheduleScreen.addDrawable(&wednesdayContainer);
	scheduleScreen.addDrawable(&thursdayContainer);
	scheduleScreen.addDrawable(&fridayContainer);
	scheduleScreen.addDrawable(&saturdayContainer);
	scheduleScreen.addDrawable(&sundayContainer);

	setPointInitializeConnections(setPointScreen);



	Container BackContainerSetPoint = Container(169,415,containerWidth,containerHeight);

	BackContainerSetPoint.addDrawable(&backRectangle);
	BackContainerSetPoint.addDrawable(&backText);

	Container deleteSetPoint = Container(331,415,containerWidth,containerHeight);
	DrawText deleteTextSetPoint = DrawText(20,20,"Delete",&Font16, WHITE, BLACK);

	deleteSetPoint.addDrawable(&backRectangle);
	deleteSetPoint.addDrawable(&deleteTextSetPoint);

	Container editContainerSetPoint = Container(493,415,containerWidth,containerHeight);
	DrawText editTextSetPoint = DrawText(20,20,"Edit",&Font16, WHITE, BLACK);

	editContainerSetPoint.addDrawable(&backRectangle);
	editContainerSetPoint.addDrawable(&editTextSetPoint);

//	setPointScreen.addDrawable(&setPointDemoContainer);

	setPointScreen.addDrawable(&BackContainerSetPoint);
	setPointScreen.addDrawable(&deleteSetPoint);
	setPointScreen.addDrawable(&editContainerSetPoint);

	screenManager.addScreen(ScreenType::HomeScreen, &openingScreen);
	screenManager.addScreen(ScreenType::MainMenuScreen, &mainMenu);
	screenManager.addScreen(ScreenType::ScheduleScreen, &scheduleScreen);
	screenManager.addScreen(ScreenType::SetPointScreen, &setPointScreen);
	screenManager.addScreen(ScreenType::AlertScreen, &alertScreen);

	screenManager.initFirstTime();
//
	screenManager.setNewActiveScreen(ScreenType::ScheduleScreen);
//	screenManager.displayActiveScreen();





	Button buttonReceivedState;

	State state;

	openingScreen.setButtonCallback([](Button bt){
		State currState = State::Error;
		switch(bt){
		case Button::Middle:
			currState = State::Interact;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
			break;
		default:
			break;
		}
	});



	mainMenu.setButtonCallback([](Button bt){
		State currState = State::Error;
		switch (bt){
		case Button::Middle:
			currState = State::HomeScreen;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
			break;
		case Button::Right:
			currState = State::Interact;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
			break;
		case Button::Left:
			currState = State::PrevScreen;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		    break;
		case Button::Up:
			currState = State::PrevElement;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		    break;
		case Button::Down:
			currState = State::NextElement;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		    break;
		default:
			break;
		}
	});

	scheduleScreen.setButtonCallback([](Button bt){
		State currState = State::Error;
		switch (bt){
		case Button::Middle:
			currState = State::HomeScreen;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
			break;
		case Button::Right:
			currState = State::Interact;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
			break;
		case Button::Left:
			currState = State::PrevScreen;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		    break;
		case Button::Up:
			currState = State::PrevElement;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		    break;
		case Button::Down:
			currState = State::NextElement;
		    xQueueSend(stateQueue, &currState, portMAX_DELAY);
		    break;
		default:
			break;
		}
	});

	SetPointData setPointData;
	while(1){
//		if (xQueueReceive(daySelectionQueue, &selectedDayIndex, pdMS_TO_TICKS(100)) == pdPASS) {
//		    LOG_INFO("Day Selected: %d", selectedDayIndex);
//		    updateSetPointScreen(setPointScreen, selectedDayIndex);
//		}
		if (xQueueReceive(dataSetPointOperationQueue, &setPointData, 10 ) == pdPASS){
//			case read

		}
		if (xQueueReceive(buttonQueue, &buttonReceivedState, 10) == pdPASS) {
			screenManager.buttonPress(buttonReceivedState);
		}
		if (xQueueReceive(stateQueue, &state, 10) == pdPASS) {
			switch(state){
			case State::HomeScreen:
				LOG_INFO("Opening Screen");
				screenManager.setNewActiveScreen(ScreenType::HomeScreen);
				break;
			case State::MainMenuScreen:
				LOG_INFO("Main Menu Screen");
				screenManager.setNewActiveScreen(ScreenType::MainMenuScreen);
				break;
			case State::ScheduleScreen:
				LOG_INFO("Main Menu Screen");
				screenManager.setNewActiveScreen(ScreenType::ScheduleScreen);
				break;
			case State::SetPointScreen:
				LOG_INFO("Main Menu Screen");
				screenManager.setNewActiveScreen(ScreenType::SetPointScreen);
				setPointScreen.setButtonCallback(setPointScreenCallback1);
				break;
			case State::PrevElement:
				LOG_INFO("Prev Element");

				screenManager.prevInteractable();
				screenManager.updateActiveScreen();
				break;
			case State::NextElement:
				LOG_INFO("Next Element");

				screenManager.nextInteractable();
				screenManager.updateActiveScreen();
				break;
			case State::Interact:
				LOG_INFO("Interact");

				screenManager.onInteract();
				break;



			case State::PrevScreen:

				LOG_INFO("Prev Screen");
				screenManager.setPrevScreen();
				break;
			default:
				break;
			}
		}
	}
}

void EPD_MainMenu() {
	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
					    		DC_GPIO_Port, DC_Pin,
								SPI_CS_GPIO_Port, SPI_CS_Pin,
					    		BUSY_GPIO_Port, BUSY_Pin,
					    		PWR_GPIO_Port, PWR_Pin,
								&hspi1
								);

	ScreenManager screenManager(&ePaper);

	Screen* screen1 = new Screen();



	UWORD containerWidth = 138;
	UWORD containerHeight = 65;

	Container* BackContainer = new Container(169,415,containerWidth,containerHeight);
	Rectangle* rect1 = new Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText* backText = new DrawText(20,20,"Back",&Font16, WHITE, BLACK);

	BackContainer->addDrawable(rect1);
	BackContainer->addDrawable(backText);
//
	Container* homeContainer = new Container(331,415,containerWidth,containerHeight);
	Rectangle* homeRectangle = new Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText* homeText = new DrawText(20,20,"Home",&Font16, WHITE, BLACK);

	homeContainer->addDrawable(homeRectangle);
	homeContainer->addDrawable(homeText);

	Container* selectContainer = new Container(493,415,containerWidth,containerHeight);
	Rectangle* selectRectangle = new Rectangle(0, 0, containerWidth, containerHeight, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
	DrawText* selectText = new DrawText(20,20,"Select",&Font16, WHITE, BLACK);

	selectContainer->addDrawable(selectRectangle);
	selectContainer->addDrawable(selectText);

	UWORD mainMenuWidth = 747;
	UWORD mainMenuHeight = 93;
	Container* scheduleContainer = new Container(26,87,mainMenuWidth,mainMenuHeight);
	Rectangle* scheduleRectangle = new Rectangle(0, 0, mainMenuWidth, mainMenuHeight, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	DrawText* scheduleText = new DrawText(20,20,"Schedule",&Font24, WHITE, BLACK);

	scheduleContainer->addDrawable(scheduleRectangle);
	scheduleContainer->addDrawable(scheduleText);


	PAINT_TIME time;

	time.Hour = 10;
	time.Min = 50;
	time.hasSeconds = false;
	time.isPM = false;

	DrawText* timeText = new DrawText(338,20,&time,&Font24, WHITE, BLACK);




	BitMap* battery = new BitMap(gImage_battery, 0, 0, 64, 64, WHITE);

	BitMap* wifi = new BitMap(gImage_wifi,730,0, 64,64,WHITE);
//
	screen1->addDrawable(BackContainer);
	screen1->addDrawable(homeContainer);
	screen1->addDrawable(selectContainer);

	screen1->addDrawable(scheduleContainer);

	screen1->addDrawable(timeText);
	screen1->addDrawable(battery);
	screen1->addDrawable(wifi);




	screenManager.addScreen(ScreenType::HomeScreen, screen1);

	screenManager.initFirstTime();
//
	screenManager.displayActiveScreen();

	while(1){

	}

}

void EPD_MainScreen() {
	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
						    		DC_GPIO_Port, DC_Pin,
									SPI_CS_GPIO_Port, SPI_CS_Pin,
						    		BUSY_GPIO_Port, BUSY_Pin,
						    		PWR_GPIO_Port, PWR_Pin,
									&hspi1
									);
	ScreenManager screenManager(&ePaper);

	Screen* screen1 = new Screen(nullptr, EPD_4in26_WIDTH, EPD_4in26_HEIGHT, WHITE);

	UWORD containerYLength = 100;
	UWORD containerYStart = EPD_4in26_HEIGHT - containerYLength;

	UWORD ContainerxLength = 150;
	UWORD ContainerxStart = 325;
	Container * container1 = new Container(ContainerxStart,containerYStart,138,65);

	Rectangle* rect1 = new Rectangle(0, 0, ContainerxLength, containerYLength, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

	BitMap* bitmap = new BitMap(gImage_icon, 27, 2, 96, 96, WHITE);

//	container1->addDrawable(circle1);
	container1->addDrawable(rect1);
	container1->addDrawable(bitmap);

	float setPoint = 23.5;
	char buffer[50]; // Buffer for formatted string

	//snprintf(buffer, sizeof(buffer), "Setpoint: %.1fC", setPoint);

	DrawText* setPointText = new DrawText(358,121,buffer,&Font16, WHITE, BLACK);

	float temparature = 24.5;
	char temparatureBuffer[50]; // Buffer for formatted string

	//snprintf(temparatureBuffer, sizeof(temparatureBuffer), "Temperature: %.1fC", temparature);
	DrawText* actualTemperature = new DrawText(302,174,temparatureBuffer,&Font24, WHITE, BLACK);


	PAINT_TIME time;

	time.Hour = 10;
	time.Min = 50;
	time.hasSeconds = false;

	DrawText* timeText = new DrawText(338,36,&time,&Font24, WHITE, BLACK);


	BitMap* battery = new BitMap(gImage_battery, 0, 0, 64, 64, WHITE);

	BitMap* wifi = new BitMap(gImage_wifi,730,0, 64,64,WHITE);

	screen1->addDrawable(container1);
	screen1->addDrawable(setPointText);
	screen1->addDrawable(actualTemperature);
	screen1->addDrawable(timeText);
	screen1->addDrawable(battery);
	screen1->addDrawable(wifi);


	screenManager.addScreen(ScreenType::HomeScreen,screen1);

	screenManager.initFirstTime();

//	ePaper.EPD_4in26_Display(fb.getImage());
	screenManager.displayActiveScreen();

	while(1){

	}


}


void EPD_Clear(){
	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
					    		DC_GPIO_Port, DC_Pin,
								SPI_CS_GPIO_Port, SPI_CS_Pin,
					    		BUSY_GPIO_Port, BUSY_Pin,
					    		PWR_GPIO_Port, PWR_Pin,
								&hspi1
								);
	ePaper.pinInit();
	ePaper.EPD_4in26_Init();

	ePaper.EPD_4in26_Clear();
}

void EPD_EventHandling(){

	std::queue<State> q;
//	q.push(State::Interact);

	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
				    		DC_GPIO_Port, DC_Pin,
							SPI_CS_GPIO_Port, SPI_CS_Pin,
				    		BUSY_GPIO_Port, BUSY_Pin,
				    		PWR_GPIO_Port, PWR_Pin,
							&hspi1
							);

	Screen* screen1 = new Screen(nullptr, EPD_4in26_WIDTH, EPD_4in26_HEIGHT, WHITE);

	Rectangle* rect1 = new Rectangle(200, 70, 300, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY,
			[]() {

//		q.push(State::Next);
//
//		q.push(State::Interact);

//		LOG_INFO("interacted!");
	});

	Rectangle* rect2 = new Rectangle(400, 70, 500, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY
//			,[&]() {
//
//		q.push(State::Next);
//		q.push(State::Interact);
//
////		LOG_INFO("interacted!");
//	}
	);

	Rectangle* rect3 = new Rectangle(600, 70, 700, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY,
			[]() {

//		q.push(State::Next);
////		q.push(State::Interact);

//		LOG_INFO("interacted!");
	});
//	Rectangle* rect2 = new Rectangle(400, 300, 600, 350, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

//	Circle* circle1 = new Circle(80, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//	Circle* circle2 = new Circle(150, 200, 240, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

//	screen1->addDrawable(circle1);
	screen1->addDrawable(rect1);
	screen1->addDrawable(rect2);
	screen1->addDrawable(rect3);

//	screen1->addDrawable(circle2);

	ScreenManager screenManager(&ePaper);

	screenManager.addScreen(ScreenType::HomeScreen,screen1);

	screenManager.initFirstTime();
	screenManager.displayActiveScreen();

	for (; !q.empty(); q.pop()){
		State myState = q.front();
//
//		if(myState == State::Interact){
//			screenManager.onInteract();
//		}
//
//		if(myState == State::Next){
//			screenManager.nextInteractable();
//			screenManager.updateActiveScreen();
////			LOG_INFO("MESSAGE WAS RECIEVED");
//		}


//		vTaskDelay(pdMS_TO_TICKS(1000);
	}
}

void EPD_ScreenManager_PartialRefresh(){
	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
				    		DC_GPIO_Port, DC_Pin,
							SPI_CS_GPIO_Port, SPI_CS_Pin,
				    		BUSY_GPIO_Port, BUSY_Pin,
				    		PWR_GPIO_Port, PWR_Pin,
							&hspi1
							);


		Screen* screen1 = new Screen(nullptr, EPD_4in26_WIDTH, EPD_4in26_HEIGHT, WHITE);
//		Screen screen2(EPD_4in26_WIDTH, EPD_4in26_HEIGHT, WHITE);
//		Screen screen3(EPD_4in26_WIDTH, EPD_4in26_HEIGHT, WHITE);

		Rectangle* rect1 = new Rectangle(400, 70, 500, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		Rectangle* rect2 = new Rectangle(400, 300, 500, 350, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

		Circle* circle1 = new Circle(80, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		Circle* circle2 = new Circle(150, 200, 240, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

		screen1->addDrawable(circle1);
		screen1->addDrawable(rect1);
		screen1->addDrawable(circle2);

		ScreenManager screenManager(&ePaper);

		screenManager.addScreen(ScreenType::HomeScreen,screen1);


		screenManager.initFirstTime();
		screenManager.displayActiveScreen();

		vTaskDelay(pdMS_TO_TICKS(1000));

		rect1->setColor(WHITE);
		screen1->addDrawable(rect2);

		screenManager.updateActiveScreen();

		vTaskDelay(pdMS_TO_TICKS(1000));


		rect1->setColor(BLACK);
		rect2->setColor(WHITE);

		screenManager.updateActiveScreen();

		vTaskDelay(pdMS_TO_TICKS(1000));
//		screen1.render();
//
//		ePaper.EPD_4in26_WriteToBuffer(screen1.getImage(), 0, EPD_4in26_HEIGHT-1, EPD_4in26_WIDTH-1,0);
//		ePaper.EPD_4in26_WriteToBuffer(fb.getImage(), 0,50, width,height);
//		ePaper.EPD_4in26_TurnOnDisplay_Part();
//		for(int i = 0; i < 9; i++){
//
//			if(i==1){
//				screen1.addDrawable(rect2);
//				ePaper.EPD_4in26_WriteToBuffer(screen1.getImage(), 0, EPD_4in26_HEIGHT-1, EPD_4in26_WIDTH-1,0);
//				ePaper.EPD_4in26_TurnOnDisplay_Part();
//				vTaskDelay(pdMS_TO_TICKS(1000);
//
//			}
//			screenManager.updateActiveScreen();
//			vTaskDelay(pdMS_TO_TICKS(1000);
//
//		}
}

void EPD_ScreenManager_PartialRefreshAndScreenSwitching (){
	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
			    		DC_GPIO_Port, DC_Pin,
						SPI_CS_GPIO_Port, SPI_CS_Pin,
			    		BUSY_GPIO_Port, BUSY_Pin,
			    		PWR_GPIO_Port, PWR_Pin,
						&hspi1
						);


	Screen screen1 = Screen();
	Screen screen2 = Screen();
	Screen screen3 = Screen();

	Rectangle* rect1 = new Rectangle(400, 70, 500, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	Rectangle* rect2 = new Rectangle(150, 200, 220, 240, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

	Circle* circle1 = new Circle(80, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	Circle* circle2 = new Circle(150, 200, 240, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

	screen1.addDrawable(circle1);
	screen1.addDrawable(rect1);
	screen2.addDrawable(circle2);
	screen3.addDrawable(rect1);
	screen3.addDrawable(rect2);





	ScreenManager screenManager(&ePaper);

	screenManager.addScreen(ScreenType::HomeScreen,&screen1);
	screenManager.addScreen(ScreenType::HomeScreen,&screen2);
	screenManager.addScreen(ScreenType::HomeScreen,&screen3);

//	screenManager.renderAll();

	screenManager.initFirstTime();
//	for(int i = 0; i < 9; i++){
//		int n = i%3;
//		screenManager.setNewActiveScreen(n);
//		vTaskDelay(pdMS_TO_TICKS(1000));
//
//		if(i%3 == 0){
////			screen1.clear(i%2==0?BLACK:WHITE);
//			rect1->setFill(i%2==0?DRAW_FILL_EMPTY:DRAW_FILL_FULL);
//			screenManager.updateActiveScreen();
//			vTaskDelay(pdMS_TO_TICKS(1000));
////
//		}
//
//	}

}


void EPD_partial_refresh(){
	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
		    		DC_GPIO_Port, DC_Pin,
					SPI_CS_GPIO_Port, SPI_CS_Pin,
		    		BUSY_GPIO_Port, BUSY_Pin,
		    		PWR_GPIO_Port, PWR_Pin,
					&hspi1
					);
    ePaper.pinInit();

    ePaper.EPD_4in26_Init();
    ePaper.EPD_4in26_Clear();
    vTaskDelay(pdMS_TO_TICKS(500));

	Screen screen1 = Screen();
//	screen1.clear(BLACK);
	Rectangle* rect1 = new Rectangle(0, 0, EPD_4in26_WIDTH, EPD_4in26_HEIGHT, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);


//	UWORD height =50;
//	UWORD width = 50;
//	UWORD testImageSize = (((width % 8 == 0)? (width / 8 ): (width / 8 + 1)) * height);
//	UBYTE testIMAGE[testImageSize];
//    FrameBuffer fb(testIMAGE,width, height,ROTATE_0,WHITE);
//    fb.Paint_DrawRectangle(0, 0, width, height, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

	screen1.addDrawable(rect1);
	screen1.cleanRender();


	ePaper.EPD_4in26_Display_Base(screen1.getImage());

	vTaskDelay(pdMS_TO_TICKS(1000));

	rect1->setColor(WHITE);
	screen1.cleanRender();

	ePaper.EPD_4in26_WriteToBuffer(screen1.getImage(), 0, 0, EPD_4in26_WIDTH,EPD_4in26_HEIGHT);
//	ePaper.EPD_4in26_WriteToBuffer(fb.getImage(), 0,50, width,height);
	ePaper.EPD_4in26_TurnOnDisplay_Part();

	vTaskDelay(pdMS_TO_TICKS(1000));


	rect1->setColor(BLACK);
	screen1.cleanRender();

	ePaper.EPD_4in26_WriteToBuffer(screen1.getImage(), 0, 0, EPD_4in26_WIDTH,EPD_4in26_HEIGHT );
	ePaper.EPD_4in26_TurnOnDisplay_Part();

	vTaskDelay(pdMS_TO_TICKS(1000));

	rect1->setColor(WHITE);
	screen1.cleanRender();

	ePaper.EPD_4in26_WriteToBuffer(screen1.getImage(), 0, 0, EPD_4in26_WIDTH,EPD_4in26_HEIGHT );
	ePaper.EPD_4in26_TurnOnDisplay_Part();

	vTaskDelay(pdMS_TO_TICKS(1000));

	printf("Clear...\r\n");
	ePaper.EPD_4in26_Init();
	ePaper.EPD_4in26_Clear();

    printf("Goto Sleep...\r\n");
	ePaper.EPD_4in26_Sleep();

	vTaskDelay(pdMS_TO_TICKS(2000));//important, at least 2s

	printf("close 5V, Module enters 0 power consumption ...\r\n");
	ePaper.pinExit();
}

//void EPD_screen_manager(){
//	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
//		    		DC_GPIO_Port, DC_Pin,
//					SPI_CS_GPIO_Port, SPI_CS_Pin,
//		    		BUSY_GPIO_Port, BUSY_Pin,
//		    		PWR_GPIO_Port, PWR_Pin,
//					&hspi1
//					);
//
//    //finish init
//
//	Screen screen1(EPD_4in26_WIDTH, EPD_4in26_HEIGHT, WHITE);
//	Screen screen2(EPD_4in26_WIDTH, EPD_4in26_HEIGHT, WHITE);
//	Screen screen3(EPD_4in26_WIDTH, EPD_4in26_HEIGHT, WHITE);
//
//    Rectangle* rect1 = new Rectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//    Rectangle* rect2 = new Rectangle(150, 200, 220, 240, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//
//    Circle* circle1 = new Circle(80, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//    Circle* circle2 = new Circle(150, 200, 240, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//
//    screen1.addDrawable(circle1);
//    screen2.addDrawable(circle2);
//    screen3.addDrawable(rect1);
//    screen3.addDrawable(rect2);
//
//
//
//
//
//	ScreenManager screenManager(&ePaper);
//
//	screenManager.addScreen(&screen1);
//	screenManager.addScreen(&screen2);
//	screenManager.addScreen(&screen3);
//
//	screenManager.renderAll();
//
//	screenManager.init();
//
//	for(int i = 0; i < 9; i++){
//		int n = i%3;
//		screenManager.setActiveScreen(n);
//		screenManager.display();
//
//		vTaskDelay(pdMS_TO_TICKS(1000);
//	}
//
//
//
//}

void EPD_multiple_Screens(){
	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
	    		DC_GPIO_Port, DC_Pin,
				SPI_CS_GPIO_Port, SPI_CS_Pin,
	    		BUSY_GPIO_Port, BUSY_Pin,
	    		PWR_GPIO_Port, PWR_Pin,
				&hspi1
				);
	    ePaper.pinInit();

	    ePaper.EPD_4in26_Init();
	    ePaper.EPD_4in26_Clear();
	    vTaskDelay(pdMS_TO_TICKS(500));
	    //finish init

	    //start FRAMEBUFFER INIT
	    Screen screen1 =  Screen();
	    Screen screen2 = Screen();

	    screen1.clear(WHITE);
	    screen2.clear(BLACK);

	    Rectangle* rect1 = new Rectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	    Rectangle* rect2 = new Rectangle(150, 200, 220, 240, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

	    screen1.addDrawable(rect1);
	    screen1.addDrawable(rect2);

	    screen1.cleanRender();


	    Circle* circle1 = new Circle(80, 70, 120, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	    Circle* circle2 = new Circle(150, 200, 240, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

	    screen2.addDrawable(circle1);
	    screen2.addDrawable(circle2);

	    screen2.cleanRender();


	    int loop = 0;
	    while (loop != 3){
		    ePaper.EPD_4in26_Display_Base(screen1.getImage());
			vTaskDelay(pdMS_TO_TICKS(2000));

		    ePaper.EPD_4in26_Display_Base(screen2.getImage());
			vTaskDelay(pdMS_TO_TICKS(2000));

			loop++;
	    }
//		vTaskDelay(pdMS_TO_TICKS(2000);
	    //work / testing




	    // finish testing

		printf("Clear...\r\n");
		ePaper.EPD_4in26_Init();
		ePaper.EPD_4in26_Clear();

	    printf("Goto Sleep...\r\n");
		ePaper.EPD_4in26_Sleep();

		vTaskDelay(pdMS_TO_TICKS(2000));//important, at least 2s

		printf("close 5V, Module enters 0 power consumption ...\r\n");
		ePaper.pinExit();
}



void EPD_screen_multiple_objects(){
	EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
	    		DC_GPIO_Port, DC_Pin,
				SPI_CS_GPIO_Port, SPI_CS_Pin,
	    		BUSY_GPIO_Port, BUSY_Pin,
	    		PWR_GPIO_Port, PWR_Pin,
				&hspi1
				);
	    ePaper.pinInit();

	    ePaper.EPD_4in26_Init();
	    ePaper.EPD_4in26_Clear();
	    vTaskDelay(pdMS_TO_TICKS(500));
	    //finish init

	    //start FRAMEBUFFER INIT
	    Screen screen1;

	    screen1.clear(WHITE);

	    Rectangle* rect1 = new Rectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	    Rectangle* rect2 = new Rectangle(150, 200, 220, 240, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

	    screen1.addDrawable(rect1);
	    screen1.addDrawable(rect2);

	    screen1.cleanRender();
//	    FrameBuffer fb(BlackImage,EPD_4in26_WIDTH, EPD_4in26_HEIGHT,ROTATE_0,WHITE);
//	    fb.Paint_Clear(WHITE);
//	    fb.Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//	    fb.Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
	    ePaper.EPD_4in26_Display_Base(screen1.getImage());
		vTaskDelay(pdMS_TO_TICKS(2000));
	    //work / testing




	    // finish testing

		printf("Clear...\r\n");
		ePaper.EPD_4in26_Init();
		ePaper.EPD_4in26_Clear();

	    printf("Goto Sleep...\r\n");
		ePaper.EPD_4in26_Sleep();

		vTaskDelay(pdMS_TO_TICKS(2000));//important, at least 2s

		printf("close 5V, Module enters 0 power consumption ...\r\n");
		ePaper.pinExit();
}


void EPD_basic_controller_test() {
    EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
    		DC_GPIO_Port, DC_Pin,
			SPI_CS_GPIO_Port, SPI_CS_Pin,
    		BUSY_GPIO_Port, BUSY_Pin,
    		PWR_GPIO_Port, PWR_Pin,
			&hspi1
			);
    ePaper.pinInit();

    ePaper.EPD_4in26_Init();
    ePaper.EPD_4in26_Clear();
    vTaskDelay(pdMS_TO_TICKS(500));
    vTaskDelay(pdMS_TO_TICKS(2000));

	printf("Goto Sleep...\r\n");
	ePaper.EPD_4in26_Sleep();

	vTaskDelay(pdMS_TO_TICKS(2000));//important, at least 2s
//	test1.;
}

extern UBYTE defaultImage[];
void EPD_frame_buffer_draw_test() {
    EPD_4in26 ePaper(RST_GPIO_Port, RST_Pin,
    		DC_GPIO_Port, DC_Pin,
			SPI_CS_GPIO_Port, SPI_CS_Pin,
    		BUSY_GPIO_Port, BUSY_Pin,
    		PWR_GPIO_Port, PWR_Pin,
			&hspi1
			);
    ePaper.pinInit();

    ePaper.EPD_4in26_Init();
    ePaper.EPD_4in26_Clear();
    vTaskDelay(pdMS_TO_TICKS(500));
    //finish init

    //start FRAMEBUFFER INIT
	FrameBuffer& fb = FrameBuffer::getInstance();

//    FrameBuffer fb(defaultImage, EPD_4in26_WIDTH, EPD_4in26_HEIGHT,ROTATE_0,WHITE);
    fb.Paint_Clear(WHITE);
    fb.Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    fb.Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
    ePaper.EPD_4in26_Display_Base(fb.getImage());
    vTaskDelay(pdMS_TO_TICKS(2000));
    //work / testing




    // finish testing

	printf("Clear...\r\n");
	ePaper.EPD_4in26_Init();
	ePaper.EPD_4in26_Clear();

    printf("Goto Sleep...\r\n");
	ePaper.EPD_4in26_Sleep();

	vTaskDelay(pdMS_TO_TICKS(2000));//important, at least 2s

	printf("close 5V, Module enters 0 power consumption ...\r\n");
	ePaper.pinExit();
    //finishes init


}
