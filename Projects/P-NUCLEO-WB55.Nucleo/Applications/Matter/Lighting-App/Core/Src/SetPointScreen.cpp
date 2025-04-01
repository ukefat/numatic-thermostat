/*
 * SetPointScreen.cpp
 *
 *  Created on: Mar 30, 2025
 *      Author: kobru
 */

#include "Screen.hpp"
#include "EPD_test.hpp"


SetPointScreen::SetPointScreen(Callback cb):
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

void SetPointScreen::optimizedRender() {
	FrameBuffer& fb = FrameBuffer::getInstance();

	for (int i = 0; i < setPointElementsCount; i++) {
		setPointElements[i]->draw(fb);
	}
	Screen::optimizedRender();
}
void SetPointScreen::cleanRender() {
	Screen::cleanRender();

	FrameBuffer& fb = FrameBuffer::getInstance();
	setPointUpdate();

	for (int i = 0; i < setPointElementsCount; i++) {
		setPointElements[i]->resetUpdated();
		setPointElements[i]->draw(fb);
	}
}

void SetPointScreen::interact(){
	if(isNewSetPoint){
//    		if( setPointSelection > -1){
			setPointElements[currentSetPointElement]->triggerAction();
//    		}
	}
	else{
		Screen::interact();
	}

}

void SetPointScreen::setPointUpdate() {
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

void SetPointScreen::setPointUpdateElements(){
	snprintf(hoursBuffer, sizeof(hoursBuffer), "%d", hours);
	setPointHoursContainer.resetClear();
	setPointHoursText.setString(hoursBuffer);

	lukeisfuckingretardisthehoelessmostincooherenceioadnawdnowa(temperatureBuffer, sizeof(temperatureBuffer),temperature);
//	snprintf(temperatureBuffer, sizeof(temperatureBuffer), "%.1f", temperature);
	setPointTempContainer.resetClear();
	setPointTempText.setString(temperatureBuffer);

	snprintf(minutesBuffer, sizeof(minutesBuffer), "%d", minutes);
	setPointMinutesContainer.resetClear();
	setPointMinutesText.setString(minutesBuffer);


	setPointAMText.setString(isAm ? "AM":"PM");
}

void SetPointScreen::selectPrev(){
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

void SetPointScreen::selectNext() {
	if(isNewSetPoint){

//		setPointTempContainer.resetClear();
//		setPointHoursContainer.resetClear();
//		setPointMinutesContainer.resetClear();
//        	setPointUpdateElements();
//    		setPointScreen.refreshBoxes();

		setPointElements[currentSetPointElement]->highlight(false);

		currentSetPointElement = (currentSetPointElement + 1) % setPointElementsCount;

		setPointElements[currentSetPointElement]->highlight(true);

	}
	else {
		Screen::selectNext();

		currentSetPointSelection = currentSelection;

		setPointUpdate();

	}


}
void SetPointScreen::addDrawable(Drawable* obj) {
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

void SetPointScreen::setSetPoint(bool setSetPoint){
	isNewSetPoint = setSetPoint;
}

void SetPointScreen::addSetpoint(){
	currentSetPointElement = 0;
	isNewSetPoint = true;
}
void SetPointScreen::editCurrentSetPoint(){
	currentSetPointElement = 0;
	isNewSetPoint = true;
//    	setPoin
	setPointElements[currentSetPointElement]->highlight(true);
	setPointUpdate();
}

void SetPointScreen::increaseValue(){
	if(currentSetPointElement ==0){
		hours = (hours+1) % 24;

		snprintf(hoursBuffer, sizeof(hoursBuffer), "%d", hours);
		setPointHoursContainer.resetClear();
		setPointHoursText.setString(hoursBuffer);
	}
	else if(currentSetPointElement ==1){
		minutes = (minutes+5) % 60;

		snprintf(minutesBuffer, sizeof(minutesBuffer), "%d", minutes);
		setPointMinutesContainer.resetClear();
		setPointMinutesText.setString(minutesBuffer);
	}
	else{
		temperature = (temperature + 0.5 > 30.0 ? 15.0: temperature+0.5);

		lukeisfuckingretardisthehoelessmostincooherenceioadnawdnowa(temperatureBuffer, sizeof(temperatureBuffer), temperature);

//		snprintf(temperatureBuffer, sizeof(temperatureBuffer), "%.1f", temperature);
		setPointTempContainer.resetClear();
		setPointTempText.setString(temperatureBuffer);
	}
//    	setPointUpdateElements();
}
void SetPointScreen::decreaseValue(){
	if(currentSetPointElement ==0){
		hours = (hours == 0 ? 23 : hours -1);

		snprintf(hoursBuffer, sizeof(hoursBuffer), "%d", hours);
		setPointHoursContainer.resetClear();
		setPointHoursText.setString(hoursBuffer);
	}
	else if(currentSetPointElement ==1){
		minutes = (minutes == 0 ? 55 : minutes -5);

		snprintf(minutesBuffer, sizeof(minutesBuffer), "%d", minutes);
		setPointMinutesContainer.resetClear();
		setPointMinutesText.setString(minutesBuffer);
	}
	else{
		temperature  = (temperature < 7 ? 30 : temperature - 0.5);
		lukeisfuckingretardisthehoelessmostincooherenceioadnawdnowa(temperatureBuffer, sizeof(temperatureBuffer), temperature);

//		snprintf(temperatureBuffer, sizeof(temperatureBuffer), "%.1f", temperature);
		setPointTempContainer.resetClear();
		setPointTempText.setString(temperatureBuffer);
	}
//    	setPointUpdateElements();
}
void SetPointScreen::cancel(){
	setPointUpdate();
	setPointElements[currentSetPointElement]->highlight(false);
	isNewSetPoint = false;

}

void SetPointScreen::remove(){
	setPointElements[currentSetPointElement]->highlight(false);
	isNewSetPoint = false;
}

void SetPointScreen::save(){
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
void SetPointScreen::deleteSetPoint(){}
