#include "testApp.h"
string currentCompositionDirectory = "./settings/";
void testApp::initVolumetrics()
{
    imageSequence.init("volumes/head/cthead-8bit",3,".tif", 1);
    volWidth = imageSequence.getWidth();
    volHeight = imageSequence.getHeight();
    volDepth = imageSequence.getSequenceLength();
    
    cout << "setting up volume data buffer at " << volWidth << "x" << volHeight << "x" << volDepth <<"\n";
    
    volumeData = new unsigned char[volWidth*volHeight*volDepth*4];
    
    for(int z=0; z<volDepth; z++)
    {
        imageSequence.loadFrame(z);
        for(int x=0; x<volWidth; x++)
        {
            for(int y=0; y<volHeight; y++)
            {
                // convert from greyscale to RGBA, false color
                int i4 = ((x+volWidth*y)+z*volWidth*volHeight)*4;
                int sample = imageSequence.getPixels()[x+y*volWidth];
                ofColor c;
                c.setHsb(sample, 255-sample, sample);
                
                volumeData[i4] = c.r;
                volumeData[i4+1] = c.g;
                volumeData[i4+2] = c.b;
                volumeData[i4+3] = sample;
            }
        }
    }
    
    myVolume.setup(volWidth, volHeight, volDepth, ofVec3f(1,1,2));
    myVolume.updateVolumeData(volumeData,volWidth,volHeight,volDepth,0,0,0);
    myVolume.setRenderSettings(0.5, 0.75, 0.75, 0.1);
    
    linearFilter = false;
    
}
//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
   ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	gui.setup("Settings", "defaultGuiSettings.xml");
    
    gui.add(shouldResetCamera.setup("Reset Camera", ofxParameter<bool>()));
    gui.add(cameraSpeed.setup("Camera Speed", ofxParameter<float>(), 0, 40));
    gui.add(cameraRollSpeed.setup("Cam Roll Speed", ofxParameter<float>(), .0, 4));
    gui.add(shouldSaveCameraPoint.setup("Set Camera Point", ofxParameter<bool>()));
    gui.add(currentLockCamera.setup("Lock to Track", ofxParameter<bool>()));
    
    gui.loadFromFile("defaultGuiSettings.xml");
	
    initVolumetrics();

    cam.setup();
	cam.speed = 10;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.setFarClip(30000);
	cam.setScale(1, -1, 1);
	cam.targetNode.setScale(1,1,1);
	cameraTrack.setCamera(cam);
	cam.loadCameraPosition();
	cam.maximumY =  120;
	cam.minimumY = -120;
	cam.rollSpeed = 1;
    
    cameraTrack.lockCameraToTrack = false;
    
    
    timeline.setup();
	timeline.getColors().load();
	timeline.setOffset(ofVec2f(0, ofGetHeight() - 200));
	timeline.setPageName("Main");
	timeline.setDurationInFrames(300); //base duration
    timeline.setMovePlayheadOnDrag(false);
    
    populateTimelineElements();
}
void testApp::populateTimelineElements(){
	
	timeline.setPageName("Camera");
	timeline.addTrack("Camera", &cameraTrack);
	timeline.addCurves("Volume Threshold", currentCompositionDirectory + "VolumeThreshold.xml", ofRange(0, 1), myVolume.getThreshold() );
	timeline.addCurves("Volume Density", currentCompositionDirectory + "VolumeDensity.xml", ofRange(0, 1), myVolume.getDensity() );
	timeline.addCurves("Volume XyQuality", currentCompositionDirectory + "VolumeXyQuality.xml", ofRange(0, 1), myVolume.getXyQuality() );
	timeline.addCurves("Volume ZQuality", currentCompositionDirectory + "VolumeZQuality.xml", ofRange(0, 1), myVolume.getZQuality() );
	timeline.addSwitches("Volume LinearFilter", currentCompositionDirectory + "VolumeLinearFilter.xml" );
	
	cameraTrack.setup();
    cameraTrack.load();
	cameraTrack.enable();
}

//--------------------------------------------------------------
void testApp::update(){
	
	myVolume.setThreshold(timeline.getValue("Volume Threshold"));
	
	myVolume.setDensity(timeline.getValue("Volume Density"));
	
	myVolume.setXyQuality(timeline.getValue("Volume XyQuality"));
	
	myVolume.setZQuality(timeline.getValue("Volume ZQuality"));
		
//	myVolume.setVolumeTextureFilterMode(GL_LINEAR);
//	linearFilter = true;
//	myVolume.setVolumeTextureFilterMode(GL_NEAREST);
//	linearFilter = false;
	
//	cameraTrack.lockCameraToTrack = currentLockCamera;
//	cameraTrack.setTimelineInOutToTrack();
	
	if(currentLockCamera != cameraTrack.lockCameraToTrack){
		if(!currentLockCamera){
			cam.targetNode.setPosition(cam.getPosition());
			cam.targetNode.setOrientation(cam.getOrientationQuat());
			cam.rotationX = cam.targetXRot = -cam.getHeading();
			cam.rotationY = cam.targetYRot = -cam.getPitch();
			cam.rotationZ = -cam.getRoll();
		}
		cameraTrack.lockCameraToTrack = currentLockCamera;
	}
	
	if(shouldSaveCameraPoint){
		
		cameraTrack.addKeyframe();
	}
	if(shouldResetCamera){
        resetCameraPosition();
	}
	cam.speed = cameraSpeed;
    cam.rollSpeed = cameraRollSpeed;
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetBackgroundColor(ofColor::white);
    cam.begin();
    ofPushMatrix();
    ofRotate(90, 1, 0, 0);
    myVolume.drawVolume(0,0,0, ofGetHeight(), 0);
    ofPopMatrix();
    cam.end();
	gui.draw();
    timeline.draw();
}
//--------------------------------------------------------------
void testApp::resetCameraPosition(){
    cam.targetNode.setPosition(ofVec3f());
    cam.targetNode.setOrientation(ofQuaternion());
    cam.targetXRot = -180;
    cam.targetYRot = 0;
    cam.rotationZ = 0;
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'f'){
		ofToggleFullscreen();
	}
    if(key == 'T'){
		cameraTrack.addKeyframe();
    }
	if(key == ' '){
		timeline.togglePlay();
	}
	if(key == 'h'){
		timeline.toggleShow();
	}
	if(key == 'L'){
    	currentLockCamera = !currentLockCamera;
    }
	if(key=='s')
	{
		saveSettings();
	}
}
void testApp::saveSettings()
{
	cam.saveCameraPosition();
	cameraTrack.save();
	
	timeline.save();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    timeline.setWidth(w);
	timeline.setOffset(ofVec2f(0, ofGetHeight() - timeline.getDrawRect().height));
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}