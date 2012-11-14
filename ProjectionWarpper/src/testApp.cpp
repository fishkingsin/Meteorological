#include "testApp.h"



#define USE_TRANSPARENCY false
static bool reload = false;

//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetWindowShape(WIDTH,HEIGHT);
    glDisable(GL_DEPTH_TEST);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetVerticalSync(true);
    ofHideCursor();
    ofSetFrameRate(60);
    ofToggleFullscreen();
	
    settings.loadFile("configs.xml");
    if(settings.pushTag("DATA"))
    {
        helpStr = settings.getValue("HELP", "help:\nH/h toggle instruction");
        ofLog(OF_LOG_VERBOSE,"helpStr:\n"+helpStr);
        string fontName = settings.getAttribute("FONT","name","/Library/Fonts/Arial.ttf");
        float fontSize = settings.getAttribute("FONT","size",12.0f);
        font.loadFont(fontName,fontSize);
        helpY = ofGetHeight()-font.stringHeight(helpStr);
        ofLog(OF_LOG_VERBOSE,"rect.width:%f rect.height:%f helpX :%f , helpY :%f",font.stringWidth(helpStr), font.stringHeight(helpStr),helpX,helpY);
        settings.popTag();
    }
	
    img.loadImage("game_of_thrones.jpg");

    gui.setDefaultKeys(true);
	
    gui.loadFromXML();
    
	gui.setAutoSave(true);
    bAlignRight = false;
//	warpper.setup();
	rm.allocateForNScreens(N_SCREEN, ofGetWidth(),ofGetHeight());
	rm.loadFromXml("fbo_settings.xml");
	guiIn.set(10,10,500,300);
	guiOut.set(520,10,400,300);;
	
	showDraw = true;
	wall.loadModel("models/wall.dae");
	wall.setScale(0.5, 0.5, 0.5);
	
	mainOutputSyphonServer.setName("Projection Warpper");
	
}
#define NUM_BYTE 512
//--------------------------------------------------------------
void testApp::update(){
	

//    warpper.update();
	
	
	
    
	if(reload)
    {
        
		reload = false;
    }
	
	
    
//    warpper.begin();
	rm.startOffscreenDraw();
	ofSetColor(0);
	ofRect(0,0,WIDTH*N_SCREEN,HEIGHT);
    
	ofSetColor(255, 255, 255);
	
	ofPushMatrix();
	ofTranslate(0,0);
    
	ofBackgroundGradient(ofColor(150), ofColor(50));
    
	ofPopMatrix();
	img.draw(0,0,WIDTH,HEIGHT);
	ofPushMatrix();
	ofTranslate(WIDTH*0.5,HEIGHT*0.5);
	wall.drawFaces();
	ofPopMatrix();
//    warpper.end();
		rm.endOffscreenDraw();
    
	
}


//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(0);
    ofSetColor(255);
	rm.drawScreens();
	mainOutputSyphonServer.publishScreen();
    if(showHelp)
	{
		rm.drawInputDiagnostically(guiIn);
		rm.drawOutputDiagnostically(guiOut);
	}
//    if(showDraw)warpper.draw();
    
	ofPushStyle();

	
	gui.draw();
    
    if(showHelp)font.drawString(helpStr, helpX, helpY);

	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch(key) {
		case 'h':
            showHelp = !showHelp;
            //ofShowCursor();
            break;
		case 'f':
			ofToggleFullscreen();
			break;
            
        case OF_KEY_LEFT:
                       break;
        case OF_KEY_RIGHT:
                        break;
			
        case '\\':
            bAlignRight =!bAlignRight;
            gui.setAlignRight(bAlignRight);
            break;
        case ' ':
            //gui.isOn();
            if(gui.isOn())
            {
//				   gui.hide();
                ofHideCursor();
            }
            else {
//                gui.show();
                ofShowCursor();
            }
            break;
			
		case 'b': 			break;
		case 's':rm.saveToXml();
 			break;
		case 'l':rm.reloadFromXml();
 			break;
		case 'd':rm.resetCoordinates;
 			break;
			
    }
//    warpper.keyPressed(key);
	
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
	
//    warpper.keyReleased(key);
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
//    warpper.mouseDragged(x, y, button);
	if(!rm.mouseDragInputPoint(guiIn, ofVec2f(x,y)))
	{
		rm.mouseDragOutputPoint(guiOut, ofVec2f(x,y));
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
//    warpper.mousePressed(x, y, button);
	if(!rm.mouseSelectInputPoint(guiIn, ofVec2f(x,y)))
	{
		rm.mouseSelectOutputPoint(guiOut, ofVec2f(x,y));
	}

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
//    warpper.mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}



