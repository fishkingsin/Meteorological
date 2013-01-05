#include "testApp.h"

const int width = 800;
const int height = 600;

//--------------------------------------------------------------
void testApp::setup(){
	ofEnableSmoothing();
	ofSetVerticalSync(true);
	ofSetWindowTitle("VideoCue");
    
	mainOutputSyphonServer.setName("VideoCue");
    
	ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
	
	dir.allowExt("mov");
	dir.allowExt("mp4"); 
	dir.allowExt("m4v");
	dir.allowExt("avi");
	int num = dir.listDir("./");
	currentIndex  =0;
	a.setPixelFormat(OF_PIXELS_RGB);
	ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE;
	if(num>0)a.loadMovie(dir.getPath(currentIndex), decodeMode);
    a.setSynchronousSeeking(false);
	a.play();
        a.setVolume(0);
    b.setPixelFormat(OF_PIXELS_RGB);
//	ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE;
	if(num>0)b.loadMovie(dir.getPath(currentIndex+1), decodeMode);
    b.setSynchronousSeeking(false);
	b.play();
    b.setVolume(0);
    
    currentPlayer = &a;
    prev = &b;
}

//--------------------------------------------------------------
void testApp::update(){
	if(currentPlayer->isLoaded())
	{
		currentPlayer->update();
		currentPlayer->draw(0, 0, ofGetWidth(), ofGetHeight());
		mainOutputSyphonServer.publishScreen();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ofEnableAlphaBlending();
	if(currentPlayer->isLoaded())currentPlayer->draw(0, 0, ofGetWidth(), ofGetHeight());
    a.draw(0, 0, 160, 120);
    b.draw(ofGetWidth()-160, 0, 160, 120);
    
  
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if(key== ' ')
	{
		currentIndex++;
		currentIndex%=dir.getFiles().size();
		ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE;
		prev->loadMovie(dir.getPath(currentIndex), decodeMode);
        prev->setVolume(0);
        ofQTKitPlayer *temp  = currentPlayer;
        currentPlayer = prev;
        prev = temp;
	}
    if (key=='1')
    {
        currentPlayer = &a;
        prev = &b;
    }
    if (key=='2')
    {
        currentPlayer = &b;
        prev = &a;
    }
    
    
}
