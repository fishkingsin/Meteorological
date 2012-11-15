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
	player.setPixelFormat(OF_PIXELS_RGB);
	ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE;
	if(num>0)player.loadMovie(dir.getPath(currentIndex), decodeMode);
	
    player.setSynchronousSeeking(false);
	
	player.play();
}

//--------------------------------------------------------------
void testApp::update(){
	if(player.isLoaded())
	{
		player.update();
		player.draw(0, 0, ofGetWidth(), ofGetHeight());
		mainOutputSyphonServer.publishScreen();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ofEnableAlphaBlending();
	if(player.isLoaded())player.draw(0, 0, ofGetWidth(), ofGetHeight());
	
    
  
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if(key== ' ')
	{
		currentIndex++;
		currentIndex%=dir.getFiles().size();
		ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE;
		player.loadMovie(dir.getPath(currentIndex), decodeMode);
	}
}
