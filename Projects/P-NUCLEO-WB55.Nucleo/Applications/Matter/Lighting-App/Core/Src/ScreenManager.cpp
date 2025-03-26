
#include "ScreenManager.h"
#include "FreeRTOS.h"
#include "task.h"


void ScreenManager::initFirstTime(){
	if (EPDController){
		EPDController->pinInit();
		EPDController->EPD_4in26_Init();

		vTaskDelay(500);

	}
}

void ScreenManager::deleteSetPoint(){
	if (currentScreen){
		currentScreen->deleteSetPoint();
	}
}

void ScreenManager::addScreen(ScreenType screentype, Screen* scr) {
    size_t index = static_cast<size_t>(screentype);
    if (index < static_cast<size_t>(ScreenType::MaxScreens)) {
        screens[index] = scr;
        displayActiveScreen();
    }
}

void ScreenManager::updateActiveScreen(){
	if(currentScreen){


		uint32_t start_render_time = xTaskGetTickCount();

		currentScreen->optimizedRender();

		uint32_t end_render_time = xTaskGetTickCount();

		uint32_t difference_render = end_render_time - start_render_time;



		const UBYTE* Image = currentScreen->getImage();

		//writes the image to the full back buffer

		uint32_t start = xTaskGetTickCount();
		EPDController->EPD_4in26_WriteToBuffer(Image, 0, 0,  EPD_4in26_WIDTH,EPD_4in26_HEIGHT);
		uint32_t end_time_spi = xTaskGetTickCount();
		//updates the screen witht eh back buffer
		EPDController->EPD_4in26_TurnOnDisplay_Part();
		uint32_t endTimeAfterRefresh = xTaskGetTickCount();

		uint32_t spi_communication = end_time_spi - start;
		uint32_t refresh_time = endTimeAfterRefresh - start;

//		LOG_INFO("render time: %d",difference_render);
		LOG_INFO("render time: %d SPI communication: %d, refresh %d",difference_render, spi_communication, refresh_time);

	}
}
void ScreenManager::setNewActiveScreen(ScreenType scr_type){
    size_t index = static_cast<size_t>(scr_type);
    if (index < static_cast<size_t>(ScreenType::MaxScreens)) {
//    	prevScreen = currentScreen;
		currentScreen = screens[index];
		screentype = scr_type;
		displayActiveScreen();

//		if(prevScreen == nullptr){
//			currentScreen = prevScreen;
//		}
	}
}

void ScreenManager::displayActiveScreen(){
	if(currentScreen){
		currentScreen->cleanRender();
//		currentScreen->OptimizedRender();
		UBYTE* img = currentScreen->getImage();
		EPDController->EPD_4in26_Display_Base(img);
	}
}

void ScreenManager::onInteract(){
	if(currentScreen){
		currentScreen->interact();
	}
}


void ScreenManager::nextInteractable(){
	if(currentScreen){
		currentScreen->selectNext();
	}
}

void ScreenManager::prevInteractable(){
	if(currentScreen){
		currentScreen->selectPrev();
	}
}

void ScreenManager::buttonPress(Button bt){
	if(currentScreen){
		currentScreen->buttonPress(bt);
	}
}

void ScreenManager::setPrevScreen(){
	switch(screentype){
	case ScreenType::MainMenuScreen:
		setNewActiveScreen(ScreenType::HomeScreen);
		break;
	case ScreenType::ScheduleScreen:
	case ScreenType::AlertScreen:
	case ScreenType::ClockDateScreen:
		setNewActiveScreen(ScreenType::MainMenuScreen);
		break;
	case ScreenType::SetPointScreen:
		setNewActiveScreen(ScreenType::ScheduleScreen);
	default:
		break;
	}
}

