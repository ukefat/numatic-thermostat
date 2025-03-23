/*
 * BitMapDrawing.hpp
 *
 *  Created on: Mar 9, 2025
 *      Author: kobru
 */

#ifndef INC_BITMAP_HPP_
#define INC_BITMAP_HPP_

#include "Drawable.h"

class BitMap: public Drawable{
public:
	BitMap(const UBYTE* img, UWORD xStart = 0, UWORD yStart = 0,UWORD width = 0, UWORD height = 0, UWORD flipColour = WHITE):
		Drawable(),
		img(img), xStart(xStart), yStart(yStart), width(width), height(height), flipColour(flipColour){}

	void draw(FrameBuffer& fb, UWORD xMove= 0, UWORD yMove=0) override{
//		LOG_WARN("x: %i, y: %i", this->xStart + xMove,this->yStart + yMove);
		if(updated){
			fb.Paint_DrawBitMap_Paste(img, this->xStart + xMove, this->yStart + yMove, width , height , flipColour);
		}
		updated = false;
	}

	void highlight(bool isSelected) override{};
private:
	const UBYTE* img;
	UWORD xStart;
	UWORD yStart;
	UWORD width;
	UWORD height;
	UWORD flipColour;
};



#endif /* INC_BITMAP_HPP_ */
