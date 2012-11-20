#pragma once

#include "ofMain.h"
#include "ofxAutoControlPanel.h"
#include "ofxCvMain.h"
#include "ofxDuration.h"
#include "DurationPanel.h"
#include "ofxPeggy.h"
//#define NUM_LED 25
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
		ofSetColor(ofMap(maxSize-size,0,maxSize,0,255));
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
		
	
    //gui
    ofxAutoControlPanel				gui;
    void							eventsIn(guiCallbackData & data);
    
	//cv
    ofVideoGrabber					vidGrabber;
    ofxCvColorImage					colorImg;
    ofRectangle						drawRect;
    ofxCvGrayscaleImage				grayImage;
    ofxCvGrayscaleImage				grayBg;
    ofxCvGrayscaleImage				grayDiff;
    ofxCvContourFinder				contourFinder;
	int								threshold,imageBrightness;
	bool							bLearnBakground,bSerial,bFlip,bMirror,bCV,bRipple,bImage,bContour;
	
	//LED
    ofFbo							scaledFbo;
    ofPixels						scaledPixels;
	vector <Ripple*>				ripples;
    vector <LED>					LEDs;
    void							renderToPeggy( int display);
    
    ofSerial						port1,port2;
	int								size,padding;
	
	//duration
	DurationPanel					*durationPanel;
	ofxDuration						duration;
	void							trackUpdated(ofxDurationEventArgs& args);
	string							ip;
	int								port;


};
