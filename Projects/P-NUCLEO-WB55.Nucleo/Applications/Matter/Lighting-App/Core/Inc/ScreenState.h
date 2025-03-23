/*
 * ScreenState.h
 *
 *  Created on: Feb 25, 2025
 *      Author: kobru
 */

#ifndef INC_SCREENSTATE_H_
#define INC_SCREENSTATE_H_

class ScreenState {
public:

	//TODO
	// logic to implement partial refreshes etc...
//    enum class RefreshType { NONE, PARTIAL, FULL };

    ScreenState() :
//    	refreshType(RefreshType::FULL),
		needsRefresh(true) {}

    void markDirty() { needsRefresh = true; }  // Indicate that something changed
    void markClean() { needsRefresh = false; } // Reset state
    bool shouldRefresh() const { return needsRefresh; }
//    RefreshType getRefreshType() const { return refreshType; }
//    void setRefreshType(RefreshType type) { refreshType = type; }

private:
//    RefreshType refreshType;
    bool needsRefresh;
};


#endif /* INC_SCREENSTATE_H_ */
