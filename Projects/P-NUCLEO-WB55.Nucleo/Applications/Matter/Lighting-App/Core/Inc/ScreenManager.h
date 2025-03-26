/*
 * ScreenManager.h
 *
 *  Created on: Feb 25, 2025
 *      Author: kobru
 */

#ifndef INC_SCREENMANAGER_H_
#define INC_SCREENMANAGER_H_

#include <EPD_4in26.hpp>
//#include <vector>
#include "Screen.h"
#include "ScreenState.h"

enum class ScreenType {
    HomeScreen,
    MainMenuScreen,
    LowBatteryScreen,
	ScheduleScreen,
	SetPointScreen,
	AlertScreen,
	ClockDateScreen,
    MaxScreens // Always keep this last for array sizing
};


class ScreenManager {
public:
	ScreenManager() = delete;
	ScreenManager(EPD_4in26 * _EPDController):  EPDController(_EPDController),
			currentScreen(nullptr), screentype(ScreenType::HomeScreen) {}

	//TODO:
	void initFirstTime();
	void initFromSleep();

	void addScreen(ScreenType screentype, Screen * screen);
	void setPrevScreen();
	void updateActiveScreen();

	void setNewActiveScreen(ScreenType scr_type);
//	void set
//	void switchScreens(size_t index);

	void displayActiveScreen();


	void cancel();
	void save();
	void increaseValue();
	void decreaseValue();
	//STATE FUNCTIONS

	void buttonPress(Button bt);
	void onInteract();
	void nextInteractable();
	void prevInteractable();

	void deleteSetPoint();
private:
	EPD_4in26* EPDController;
    Screen* currentScreen;
    ScreenType screentype;
//    Screen* prevScreen;

	Screen* screens[static_cast<size_t>(ScreenType::MaxScreens)];
//	std::vector<Screen*> screens;

	//TODO
//	std::vector<Screen*> inactiveScreen
};



#endif /* INC_SCREENMANAGER_H_ */
