//
//  InputPanel.h
//  ProjectionWarpper
//
//  Created by james KONG on 15/11/12.
//
//

#pragma once
#include "guiCustomImpl.h"
#include "guiBaseObject.h" //theo added for isInsideRect()
#include "ofxRenderManager.h"
class InputPanel : public guiCustomImpl {
private:
	
	
	
public:
	
	
	InputPanel(ofxRenderManager *_rm = NULL)
	{
		ptr = _rm;
	}
	
	// -------------------------------------
	void saveSettings(string filename){
	}
	
	void reloadSettings(){
		
	}
	
	void saveSettings(){
		
	}
	
	void loadSettings(string filename) {
		
	}
	
	// -------------------------------------
	float getWidth() {
		return boundingRect.width;
	}
	float getHeight() {
		return boundingRect.height;
	}
	
	
	
	void draw(float x, float y, float w, float h) {
		ptr->drawInputDiagnostically(x,y,w,h);
	}
	
	// -------------------------------------
	void mousePressed(float x, float y) {
		ofRectangle temp = boundingRect;
		temp.x = 0;
		temp.y = 0;
		if(!ptr->mouseSelectInputPoint(temp, ofVec2f(x,y),50))
		{
			//ptr->mouseSelectOutputPoint(boundingRect, ofVec2f(x,y));
		}
	}
	
	//these values come in with 0,0 being the top left of your bounding rect
	// -------------------------------------
	void mouseDragged(float x, float y){
		ofRectangle temp = boundingRect;
		temp.x = 0;
		temp.y = 0;
		if( isInsideRect(x, y, ofRectangle(0, 0, boundingRect.width, boundingRect.height) ) ){
			if(!ptr->mouseDragInputPoint(temp, ofVec2f(x,y)))
			{
				//ptr->mouseDragOutputPoint(boundingRect, ofVec2f(x,y));
				
			}
		}
	}
	
	void mouseReleased(){
		
	}
	ofxRenderManager *ptr;
};

