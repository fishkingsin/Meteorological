#ifndef _TEST_APP
#define _TEST_APP

/*
 *  testApp
 *
 *  Created by James Kong Kingsin on 12-08-07.
 *	
 *  
 *
 */
#include "ofMain.h"


#include "ofxRenderManager.h"
#include "ofxSyphon.h"
//#include "ofxDuration.h"
#include "ofxTextInputField.h"
#include "ofxAutoControlPanel.h"
#include "InputPanel.h"
#include "OutputPanel.h"
class testApp : public ofBaseApp {
    
public:
    void setup();
    void update();
    void draw();
    void exit()
    {
    }
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
	//Test Copntent-----------------------------------------------------
    ofImage img;
	
	//video Mapping-----------------------------------------------------
	ofxRenderManager rm;
	
	//Settings-----------------------------------------------------
    
    ofxXmlSettings settings;

	//syphon
	ofxSyphonServer syphonServer;
	ofxSyphonClient syphonClient;
	ofTexture tex;
	//gui
	ofxAutoControlPanel gui;
	simpleLogger logger;
	void eventsIn(guiCallbackData & data);
	void SyphonEvent(guiCallbackData & data);
	InputPanel *inputPanel;
	OutputPanel *outputPanel;
	guiTypeTextInput *inServName;
	guiTypeTextInput *outServName;
	bool showGrid,bExtendScreen,showDemoPic;
	
	//Duration-----------------------------------------------------
//	ofxDuration duration;
	
//	void trackUpdated(ofxDurationEventArgs& args);
//	string ip;
//	int port;
	int WIDTH,HEIGHT,N_SCREEN;
	
};

#endif
