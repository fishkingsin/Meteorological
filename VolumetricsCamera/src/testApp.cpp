#include "testApp.h"
string currentCompositionDirectory = "./settings/";
string info = "Press 1-6 change model \
press button lock camera to change free mode\
w,s,a,d move camera forward, backward ,left, right\
etc..";
void testApp::initVolumetrics(ofxImageSequencePlayer &_imageSequence)
{
    volWidth = _imageSequence.getWidth();
    volHeight = _imageSequence.getHeight();
    volDepth = _imageSequence.getSequenceLength();
    
    cout << "setting up volume data buffer at " << volWidth << "x" << volHeight << "x" << volDepth <<"\n";
    
    if(bVolumeSetup)
    {
        //        myVolume.destroy();
        bVolumeSetup = false;
        free(volumeData);
    }
    
    volumeData = new unsigned char[volWidth*volHeight*volDepth*4];
    
    for(int z=0; z<volDepth; z++)
    {
        _imageSequence.loadFrame(z);
        for(int x=0; x<volWidth; x++)
        {
            for(int y=0; y<volHeight; y++)
            {
                // convert from greyscale to RGBA, false color
                int i4 = ((x+volWidth*y)+z*volWidth*volHeight)*4;
                int sample = _imageSequence.getPixels()[x+y*volWidth];
                ofColor c;
                c.setHsb(255-sample, 255-sample, 255-sample);
                
                volumeData[i4] = c.r;
                volumeData[i4+1] = c.g;
                volumeData[i4+2] = c.b;
                volumeData[i4+3] = sample;
            }
        }
    }
    
    
    myVolume.setup(volWidth, volHeight, volDepth, ofVec3f(1,1,0.5));
    myVolume.updateVolumeData(volumeData,volWidth,volHeight,volDepth,0,0,0);
    bVolumeSetup = true;
    //    myVolume.setRenderSettings(0.5, 0.75, 0.75, 0.1);
    
//    linearFilter = false;
    
}
//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
#ifdef USE_SYPHON
    server.setName("VolumetricsCamera");
    //	ofSetWindowTitle("VolumetricsCamera");
#endif
	ofDisableArbTex();
    ofEnableNormalizedTexCoords();
	model.loadModel("models/koala.obj",true);
//	model.setRotation(1, 180,0, 0, 1);
	myVideo.loadMovie("movies/cloud.mov");
	myVideo.play();
	myVideo.setLoopState(OF_LOOP_NORMAL);
	texMapShader.load("shaders/displace");
	sampler2dTex.allocate(512,512);
	ofEnableArbTex();
    #ifdef USE_TSPS
    tspsReceiver.connect(12000);
    
    ofxAddTSPSListeners(this);
#endif
	gui.setup("Settings", "defaultGuiSettings.xml");
    
    gui.add(shouldResetCamera.setup("Reset Camera", ofxParameter<bool>()));
    gui.add(cameraSpeed.setup("Camera Speed", ofxParameter<float>(), 0, 40));
    gui.add(cameraRollSpeed.setup("Cam Roll Speed", ofxParameter<float>(), .0, 4));
    gui.add(shouldSaveCameraPoint.setup("Set Camera Point", ofxParameter<bool>()));
    gui.add(currentLockCamera.setup("Lock to Track", ofxParameter<bool>()));
    gui.add(mode.setup("DisplayMode", ofxParameter<int>(), 0,3));
    gui.add(vebose.setup("LogLevel", ofxParameter<int>(), 0,5));
    
    
    gui.loadFromFile("defaultGuiSettings.xml");
    int logl = vebose;
    ofSetLogLevel((ofLogLevel)logl);
    ofDirectory dir;
    dir.allowExt("vol");
    int num = dir.listDir("./volumes");
    imageSequence.assign(num,ofxImageSequencePlayer());
    ofLogVerbose() << "List All Volume";
    for(int i = 0 ;i < num ; i++)
    {
        ofLogVerbose() << dir.getPath(i);
        imageSequence[i].init(dir.getPath(i)+"/",4,".tif",1);
    }
    bVolumeSetup = false;
    if(imageSequence.size()>0)initVolumetrics(imageSequence[0]);
    
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
    viewportGameCam.set(0,0,ofGetWidth()*0.5,ofGetHeight()*0.5);
    
    timeline.setup();
	timeline.getColors().load();
	timeline.setOffset(ofVec2f(0, ofGetHeight()-500));
	timeline.setPageName("Main");
//	timeline.setDurationInSeconds(482);
    timeline.setMovePlayheadOnDrag(false);
    
    
    populateTimelineElements();
}
void testApp::populateTimelineElements(){
    
    waveform.loadSoundfile("sounds/Yuh Solo.aif");

	timeline.setDurationInSeconds(waveform.getDuration());

	timeline.setPageName("Camera");
	timeline.addTrack("Camera", &cameraTrack);
    timeline.addTrack("Track", &waveform);

    timeline.addPage("Cue");

    volumeEnabled = timeline.addSwitches("VolumeEnabled");
    modelEnabled = timeline.addSwitches("3DModelEnabled");
    videoEnabled = timeline.addSwitches("VideoEnabled");
    timeline.addCurves("3DModelAlpha");
    timeline.addCurves("VideoAlpha");
    
    timeline.addPage("Files");
    timeline.addFlags("VolumeFiles");
    timeline.addFlags("3DModelFile");
    timeline.addFlags("VideoFile");

    timeline.addPage("Volume");
	timeline.addCurves("Volume Threshold", currentCompositionDirectory + "VolumeThreshold.xml", ofRange(0, 1), myVolume.getThreshold() );
	timeline.addCurves("Volume Density", currentCompositionDirectory + "VolumeDensity.xml", ofRange(0, 0.2), myVolume.getDensity() );
	timeline.addCurves("Volume XyQuality", currentCompositionDirectory + "VolumeXyQuality.xml", ofRange(0, 1), myVolume.getXyQuality() );
	timeline.addCurves("Volume ZQuality", currentCompositionDirectory + "VolumeZQuality.xml", ofRange(0, 1), myVolume.getZQuality() );
    
	ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);
	
	cameraTrack.setup();
    cameraTrack.load();
	cameraTrack.enable();
}
//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& bang){
	ofLogNotice("bangFired")<<bang.track->getDisplayName() <<" flag " <<  bang.flag;
	if(bang.track->getDisplayName()=="VideoFile")
	{
		
		//ofFile newFile(bang.flag);
		
		//if (newFile.canRead() && newFile.getExtension()=="mov") {
			myVideo.loadMovie(bang.flag);
		//}

	}
	else if(bang.track->getDisplayName()=="3DModelFile")
	{
		ofFile newFile(bang.flag);
		if (newFile.canRead() && newFile.getExtension()=="obj") {
			model.loadModel(bang.flag);
		}
		
	}
	else if(bang.track->getDisplayName()=="VolumeFile")
	{
		
	}
	
}
//--------------------------------------------------------------
void testApp::update(){
    int logl = vebose;
    ofSetLogLevel((ofLogLevel)logl);
//    volumeEnabled = timeline.getValue("VolumeEnabled");
//    modelEnabled = timeline.getValue("3DModelFile");
//    videoEnabled = timeline.getValue("VideoEnabled");
	if(volumeEnabled->isOn())
	{
        myVolume.setThreshold(timeline.getValue("Volume Threshold"));
        
        myVolume.setDensity(timeline.getValue("Volume Density"));
        
        myVolume.setXyQuality(timeline.getValue("Volume XyQuality"));
        
        myVolume.setZQuality(timeline.getValue("Volume ZQuality"));
        
	}
	else if (modelEnabled->isOn() || videoEnabled->isOn())
	{
		myVideo.update();
		
		sampler2dTex.begin();
		ofClear(0);
		myVideo.draw(0, 0,sampler2dTex.getWidth(),sampler2dTex.getHeight());
		sampler2dTex.end();
	}
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
#ifdef USE_TSPS
//--------------------------------------------------------------
void testApp::onPersonEntered( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "New person!");
    // you can access the person like this:
    // tspsEvent.person
}

//--------------------------------------------------------------
void testApp::onPersonUpdated( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "Person updated!");
    // you can access the person like this:
    // tspsEvent.person
    
}

//--------------------------------------------------------------
void testApp::onPersonWillLeave( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "Person left!");
    // you can access the person like this:
    // tspsEvent.person1
    
}
#endif
//--------------------------------------------------------------
void testApp::draw(){
	ofSetBackgroundColor(ofColor::black);
	glEnable(GL_DEPTH_TEST);
    ofClear(0);
    //    ofPushView();
    {
        //        ofViewport(viewportGameCam);
        //        ofSetupScreen();
        cam.begin();
        ofClear(0);
		if(volumeEnabled->isOn())
		{
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_ADD);
            ofPushMatrix();
//            ofSetColor(255, 255*timeline.getValue("VolumeAlpha"));
            ofRotate(90, 1, 0, 0);
            myVolume.drawVolume(0,0,0, ofGetHeight(), 0);
            ofPopMatrix();
			ofPopStyle();
		}
        if(modelEnabled->isOn())
		{
			
            
			ofPushStyle();
			ofPushMatrix();
			ofSetColor(255, 255*timeline.getValue("3DModelAlpha"));
			texMapShader.begin();
			texMapShader.setUniformTexture("colormap",sampler2dTex , 1);
			model.drawFaces();
			texMapShader.end();
			ofPopMatrix();
			ofPopStyle();
		}
        cam.end();
        
    }
    glDisable(GL_DEPTH_TEST);
#ifdef USE_TSPS
	
	//    ofPopView();
    
    vector<ofxTSPS::Person*> people = tspsReceiver.getPeople();
    for ( int i=0; i<people.size(); i++){
        
        ofPushStyle();
        ofFill();
        ofSetHexColor(0x000000);
        
        ofBeginShape();
        for( int j=0; j<people[i]->contour.size(); j++ ) {
            ofVertex( people[i]->contour[j].x * ofGetWidth(), people[i]->contour[j].y * ofGetHeight() );
        }
        ofEndShape();
        ofPopStyle();
    }
#endif
#ifdef USE_SYPHON
    server.publishScreen();
#endif
    if(videoEnabled->isOn())
	{
        ofPushStyle();
        ofSetColor(255, 255*timeline.getValue("VideoAlpha"));
        myVideo.draw(0,0,ofGetWidth(),ofGetHeight());
        ofPopStyle();
        //		myVideo.draw(512, 256, 256 , 256);
        //		sampler2dTex.draw(512, 512,256,256);
	}
	gui.draw();
    timeline.draw();
}
void testApp::alignCameraToTrack()
{
    cam.targetNode.setPosition(cam.getPosition());
    cam.targetNode.setOrientation(cam.getOrientationQuat());
    cam.rotationX = cam.targetXRot = -cam.getHeading();
    cam.rotationY = cam.targetYRot = -cam.getPitch();
    cam.rotationZ = -cam.getRoll();
    
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
    switch(key)
    {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        {
            int index = key-'1';
            
            if(index<imageSequence.size())initVolumetrics(imageSequence[index]);
        }
            break;
    }
    if (key == OF_KEY_F1)
    {
        mode = 0;
    }
    if (key == OF_KEY_F2)
    {
        mode = 1;
    }
    if (key == OF_KEY_F3)
    {
        mode = 2;
    }
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
	if(key == 'i'){
		timeline.setCurrentTimeToInPoint();
	}
	
	if(key == 'o'){
		timeline.setCurrentTimeToOutPoint();
	}
	if(key=='S')
	{
		saveSettings();
	}
    if(key == 'A')
    {
        alignCameraToTrack();
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
	timeline.setOffset(ofVec2f(0, ofGetHeight() *0.5));
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    if(dragInfo.files[0].find("mov")!=string::npos)
    {
        myVideo.loadMovie(dragInfo.files[0]);
    }
}