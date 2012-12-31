#pragma once

#include "ofMain.h"
#include "ofxTimeline.h"
#include "ofxGameCamera.h"
#include "ofxVolumetrics.h"
#include "ofxTLCameraTrack.h"
#include "ofxTLAudioTrack.h"
#include "ofxGui.h"
#ifdef USE_TSPS
#include "ofxTSPSReceiver.h"
#endif
#include "ofxAssimpModelLoader.h"
#define USE_SYPHON
#ifdef USE_SYPHON
#include "ofxSyphon.h"
#endif
#define VOLUMETRICS 1
#define MODEL 2
#define VIDEO 3
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
    
    
    ofxVolumetrics myVolume;
    bool bVolumeSetup;
    unsigned char * volumeData;
    int volWidth, volHeight, volDepth;
    ofImage background;
    vector<ofxImageSequencePlayer >imageSequence;
    bool linearFilter;
    void initVolumetrics(ofxImageSequencePlayer &_imageSequence);
    
    //timeline
    ofxTimeline timeline;
    ofxTLAudioTrack waveform;
    void bangFired(ofxTLBangEventArgs& bang);
	
    //camera
    ofxGameCamera cam;
    ofRectangle viewportGameCam;
    ofxTLCameraTrack cameraTrack;
	
    void populateTimelineElements();
	void saveSettings();
	
    ofxTLSwitches* volumeEnabled,
    *modelEnabled,
    *videoEnabled;
	
	//gui
	void resetCameraPosition();
    void alignCameraToTrack();
	ofxPanel gui;
    ofxButton shouldResetCamera;
    ofxFloatSlider cameraSpeed;
    ofxFloatSlider cameraRollSpeed;
    ofxButton shouldSaveCameraPoint;
    ofxToggle currentLockCamera;
    ofxIntSlider mode;
    ofxIntSlider vebose;
    
    //TSPS contour
    #ifdef USE_TSPS
    ofxTSPS::Receiver tspsReceiver;
    
    // event listeners
    void onPersonEntered( ofxTSPS::EventArgs & tspsEvent );
    void onPersonUpdated( ofxTSPS::EventArgs & tspsEvent );
    void onPersonWillLeave( ofxTSPS::EventArgs & tspsEvent );
#endif
	//3d model
//	ofVideoPlayer myVideo;
	ofxAssimpModelLoader model;
	ofShader texMapShader;
	ofFbo sampler2dTex;
	#ifdef USE_SYPHON
	ofxSyphonServer server;
    ofxSyphonClient client;
#endif
};
