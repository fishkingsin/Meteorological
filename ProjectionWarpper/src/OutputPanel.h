//
//  OutputPanel.h
//  ProjectionWarpper
//
//  Created by james KONG on 15/11/12.
//
//

#pragma once
#include "guiCustomImpl.h"
#include "guiBaseObject.h" //theo added for isInsideRect()
#include "ofxRenderManager.h"
class OutputPanel : public guiCustomImpl {
private:
	
	
	
public:
	
	
	OutputPanel(ofxRenderManager *_rm = NULL)
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
		if(ptr!=NULL)ptr->drawOutputDiagnostically(boundingRect);
	}
	
	// -------------------------------------
	void mousePressed(float x, float y) {
		ofRectangle temp = boundingRect;
		temp.x = 0;
		temp.y = 0;
		if(!ptr->mouseSelectOutputPoint(temp, ofVec2f(x,y),50))
		{
			//ptr->mouseSelectInputPoint(boundingRect, ofVec2f(x,y));
		}
	}
	
	//these values come in with 0,0 being the top left of your bounding rect
	// -------------------------------------
	void mouseDragged(float x, float y){
		ofRectangle temp = boundingRect;
		temp.x = 0;
		temp.y = 0;
		if(!ptr->mouseDragOutputPoint(temp, ofVec2f(x,y)))
		{
			//ptr->mouseDragInputPoint(boundingRect, ofVec2f(x,y));
			
		}
		
	}
	
	void mouseReleased(){
		
	}
	ofxRenderManager *ptr;
};