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
    players.assign(num, ofQTKitPlayer());
    for(int i = 0 ; i < num ; i++)
    {
//        players.push_back(ofQTKitPlayer());
        ofQTKitPlayer* p = &players[i];
        p->setPixelFormat(OF_PIXELS_RGB);
        ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE ;
        p->loadMovie(dir.getPath(i), decodeMode);
        p->setSynchronousSeeking(false);
        p->stop();
        p->setVolume(0);
    }
    currentPlayer = &players.front();
    currentPlayer->play();
    
//	a.setPixelFormat(OF_PIXELS_RGB);
//	ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE ;
//	if(num>0)a.loadMovie(dir.getPath(currentIndex), decodeMode);
//    a.setSynchronousSeeking(false);
//	a.play();
//        a.setVolume(0);
//    b.setPixelFormat(OF_PIXELS_RGB);
////	ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_TEXTURE_AND_PIXELS;
//	if(num>0)b.loadMovie(dir.getPath(currentIndex+1), decodeMode);
//    b.setSynchronousSeeking(false);
//	b.play();
//    b.setVolume(0);
//    
//    currentPlayer = &a;
//    prev = &b;
    receiver.setup(7170);

}

//--------------------------------------------------------------
void testApp::update(){
	if(currentPlayer->isLoaded())
	{
		currentPlayer->update();
		currentPlayer->draw(0, 0, ofGetWidth(), ofGetHeight());
		mainOutputSyphonServer.publishScreen();
	}
    while(receiver.hasWaitingMessages())
    {
        ofxOscMessage m;
        receiver.getNextMessage(&m);
//        ofLogNotice("OSC")<<m.getAddress() << " " << m.getArgAsString(0);
        if(m.getAddress()=="/FILE")
        {
            if(m.getArgAsString(0)=="NEXT")
            {
                keyPressed(OF_KEY_RIGHT);
            }
            else if(m.getArgAsString(0)=="PREV")
            {
                 keyPressed(OF_KEY_LEFT);
            }
            else
            {
                currentIndex = m.getArgAsInt32(0);
                ofQTKitPlayer *temp  = currentPlayer;
                currentPlayer = &players[currentIndex];
                currentPlayer->play();
                currentPlayer->setPosition(0);
                if(temp!=currentPlayer)temp->stop();

            }
//            players.pop_front();
            
//            ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE;
//            prev->loadMovie(m.getArgAsString(0), decodeMode);
//            prev->setVolume(0);
//            ofQTKitPlayer *temp  = currentPlayer;
//            currentPlayer = prev;
//            prev = temp;

        }
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){
	
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
//    glClearColor(0.0, 0.0, 0.0, 0.0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	ofEnableAlphaBlending();
	if(currentPlayer->isLoaded())currentPlayer->draw(0, 0, ofGetWidth(), ofGetHeight());
//    a.draw(0, 0, 160, 120);
//    b.draw(ofGetWidth()-160, 0, 160, 120);
    float div = ofGetWidth()/players.size();
    for(int i = 0 ; i< players.size() ; i++)
    {
        players[i].draw(div*i,0,div,div);
    }

    
  
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if(key== OF_KEY_RIGHT)
	{
		currentIndex++;
        currentIndex%=players.size();
        ofQTKitPlayer *temp  = currentPlayer;
        currentPlayer = &players[currentIndex];
        currentPlayer->play();
        currentPlayer->setPosition(0);
        temp->stop();
	}
    else if (key== OF_KEY_LEFT)
	{
		currentIndex--;
        if(currentIndex<0)currentIndex=players.size()-1;
        ofQTKitPlayer *temp  = currentPlayer;
        currentPlayer = &players[currentIndex];
        currentPlayer->play();
        currentPlayer->setPosition(0);
        temp->stop();
    }

//    if (key=='1')
//    {
//        currentPlayer = &a;
//        prev = &b;
//    }
//    if (key=='2')
//    {
//        currentPlayer = &b;
//        prev = &a;
//    }
    
    
}
