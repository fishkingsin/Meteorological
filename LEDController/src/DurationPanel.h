//
//  DurationPanel.h
//  LEDController
//
//  Created by james KONG on 20/11/12.
//
//

#pragma once
#include "guiCustomImpl.h"
#include "guiBaseObject.h" //theo added for isInsideRect()
#include "ofxDuration.h"
class DurationPanel : public guiCustomImpl {
public:
	DurationPanel(ofxDuration *_ptr = NULL)
	{
		ptr = _ptr;
	}
	// -------------------------------------
	float getWidth() {
		return boundingRect.width;
	}
	float getHeight() {
		return boundingRect.height;
	}
	
	
	
	void draw(float x, float y, float w, float h) {
		ptr->draw(x,y,w,h);
	}
	ofxDuration *ptr;
};
