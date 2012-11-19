#pragma once

#include "ofMain.h"
#include "ofxAutoControlPanel.h"
#include "ofxCvMain.h"
#define NUM_LED 25
#define NUM_PEGGY 2
#define CAMW 320
#define CAMH 240
class LED : public ofVec2f
{
public:
    
    int brightness;
};
class Ripple : public ofVec2f
{
public:
	int size;
	int thickness;
	float   step;
	int prevMillis;
	int maxSize ;
	Ripple()
	{
		size = 0;
		step = ofRandom(0.1,0.5);
		prevMillis = 0;
		maxSize = (int)ofRandom(CAMW*0.5,CAMW);
		x = ofRandom(0,CAMW);
		y = ofRandom(0,CAMH*2);
		thickness = ofRandom(0.1,1);
	}
	void render()
	{
		float diffMillis = ofGetElapsedTimeMillis() - prevMillis;
		prevMillis = ofGetElapsedTimeMillis();
		size+=step*diffMillis;
		if(size>maxSize)
		{
			x = ofRandom(0,CAMW);
			y = ofRandom(0,CAMH*2);
			size=0;
		}
		ofPushStyle();
		ofNoFill();
		ofSetColor(255);
		ofSetLineWidth(thickness);
		ofCircle(x, y, size);
		ofPopStyle();
	}
	
};
class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
	vector <Ripple*> ripples;
    vector <LED> LEDs;
    void renderToPeggy( int display);
    
    ofxAutoControlPanel gui;
    void eventsIn(guiCallbackData & data);
    int size,padding;
    ofVideoGrabber 		vidGrabber;
    ofxCvColorImage			colorImg;
    ofRectangle  drawRect;

    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvContourFinder 	contourFinder;
    ofFbo scaledFbo;
    ofPixels scaledPixels;
    int 				threshold;
    bool				bLearnBakground,bSerial,bFlip,bMirror,bCV,bRipple;
    ofSerial port1,port2;

};
