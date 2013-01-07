#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxOsc.h"
class testApp : public ofBaseApp{
	
public:
	
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	vector<ofQTKitPlayer>players;
//    ofQTKitPlayer b;
    ofQTKitPlayer *currentPlayer;//,*prev;
    
	ofDirectory dir;
	int currentIndex;
	ofxSyphonServer mainOutputSyphonServer;
	ofxOscReceiver receiver;
};
#endif

