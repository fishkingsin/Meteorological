#ifndef _TEST_APP
#define _TEST_APP

/*
 *  ParallaxScroll
 *
 *  Created by James Kong Kingsin on 12-08-07.
 *	ParallaxScroll effect control by network message
 *  
 *
 */
#include "ofMain.h"


#include "ofxOsc.h"
#include "ofxSimpleGuiToo.h"
#include "WarpGlobal.h"
#include "ofxRenderManager.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSyphon.h"
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
	
    ofImage img;
    
    ofxOscReceiver oscReceiver;
    
//    ofPoint warpPoints[4];
	ofxRenderManager rm;
	ofRectangle guiIn,guiOut;
//    MyBizierWarp warpper;
    bool bAlignRight;
    
    ofxXmlSettings settings;
    string helpStr;
    ofTrueTypeFont font;
    float helpX,helpY;
    bool showHelp,showDraw;
	ofxAssimpModelLoader wall;
	
	ofxSyphonServer mainOutputSyphonServer;
};

#endif
