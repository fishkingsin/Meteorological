#pragma once

#include "ofMain.h"
#include "ofxAutoControlPanel.h"
#include "ofxCvMain.h"
#define NUM_LED 25
#define NUM_PEGGY 2
class LED : public ofVec2f
{
public:
    
    int brightness;
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
		
    vector <LED> LEDs;
    void renderToPeggy( int display);
    
    ofxAutoControlPanel gui;
    void eventsIn(guiCallbackData & data);
    int size,padding;
    ofVideoGrabber 		vidGrabber;
    ofxCvColorImage			colorImg;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvContourFinder 	contourFinder;
    ofFbo scaledFbo;
    ofPixels scaledPixels;
    int 				threshold;
    bool				bLearnBakground,bSerial,bFlip,bMirror;
    ofSerial port1,port2;

};
