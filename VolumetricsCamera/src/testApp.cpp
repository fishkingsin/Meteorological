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
    ofSetEscapeQuitsApp(false);`
    toggleDraw = false;
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
#ifdef USE_SYPHON
    server.setName("VolumetricsCamera");
	client.setup();
	client.setApplicationName("VideoCue");
	client.setServerName("VideoCue");
    //	ofSetWindowTitle("VolumetricsCamera");
#endif
	ofDisableArbTex();
    ofEnableNormalizedTexCoords();
	ofDirectory dirModel;
	dirModel.allowExt("obj");
	dirModel.allowExt("dae");
	dirModel.allowExt("3ds");
	int nModel =  dirModel.listDir("models/");
	models.assign(nModel, ofxAssimpModelLoader());
	for(int i = 0 ; i < nModel ; i++)
	{
		models[i].loadModel(dirModel.getPath(i));
	}
	currentModel = 0;
	model = &models[currentModel];
    //	model->setRotation(1, 180,0, 0, 1);
    //	myVideo.loadMovie("movies/cloud.mov");
    //	myVideo.play();
    //	myVideo.setLoopState(OF_LOOP_NORMAL);
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
    gui.add(mode.setup("DisplayMode", ofxParameter<int>(), -1,3));
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
    
    mask.loadImage("mask.png");
    maskHeight = ofGetHeight();
    
    
    player.loadMovie("movies/pilling_drive.mov");
    player.play();
    player.setLoopState(OF_LOOP_NORMAL);
    player.setPaused(true);
    
    colorControl.load("shaders/colorcontrol");
    colorControl.begin();
    colorControl.setUniform1i("tex", 0);
    colorControl.end();
    
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
    sender.setup("localhost", 7170);
    timeline.setup();
	timeline.getColors().load();
	timeline.setOffset(ofVec2f(0, ofGetHeight()-500));
	timeline.setPageName("Main");
	timeline.setDurationInSeconds(482);
    timeline.setMovePlayheadOnDrag(false);
    
    
    populateTimelineElements();
    
    
}
void testApp::populateTimelineElements(){
    
    waveform.loadSoundfile("sounds/Yuh Solo.aif");
    
	timeline.setDurationInSeconds(waveform.getDuration());
    
	timeline.setPageName("Camera");
	timeline.addTrack("Camera", &cameraTrack);
    timeline.addTrack("Track", &waveform);
	timeline.addFlags("SyphonServer");
    timeline.addCurves("MASK",ofRange(0,ofGetHeight()),ofGetHeight());
    
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
    timeline.addPage("PillingDrive1");
    timeline.addFlags("PD_STATE");
    timeline.addSwitches("PD_ON");
    timeline.addCurves("PD_ALPHA");
    timeline.addCurves("PD_VOLUME");
    timeline.addPage("PillingDrive2");
    timeline.addCurves("PD_X");
    timeline.addCurves("PD_Y");
    timeline.addCurves("PD_WIDTH");
    timeline.addCurves("PD_HEIGHT");
    
    timeline.addPage("ColorContrl");
    timeline.addCurves("brightness", ofRange(0.0, 2.0), 1.0);
    timeline.addCurves("contrast", ofRange(.5, 2.0), 1.0);
    timeline.addCurves("saturation", ofRange(0.0, 1.5), 1.0);
    timeline.addSwitches("invert");
    
	ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);
	
	cameraTrack.setup();
    cameraTrack.load();
	cameraTrack.enable();
}
//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& bang){
	ofLogNotice("bangFired")<<bang.track->getDisplayName() <<" flag " <<  bang.flag;
	if(bang.track->getDisplayName()=="SyphonServer")
	{
		
		//ofFile newFile(bang.flag);
		
		//if (newFile.canRead() && newFile.getExtension()=="mov") {
        //			myVideo.loadMovie(bang.flag);
		//}
        
	}
	else if(bang.track->getDisplayName()=="3DModelFile")
	{
        //		ofFile newFile(bang.flag);
        //		if (newFile.canRead() && newFile.getExtension()=="obj") {
        //			model->loadModel(bang.flag);
        //		}
		currentModel = ofToInt(bang.flag);
		if(currentModel<models.size() && currentModel >=-0)
		{
			model = &models[currentModel];
		}
        
	}
	else if(bang.track->getDisplayName()=="VideoFile")
	{
        ofxOscBundle bundle;
        ofxOscMessage m;
        m.setAddress("/FILE");
        m.addIntArg(ofToInt(bang.flag));
        bundle.addMessage(m);
		sender.sendBundle(bundle);
        bundle.clear();
        m.clear();
	}
	else if(bang.track->getDisplayName()=="PD_STATE")
	{
        if(bang.flag=="START")
        {
            player.setPaused(false);
            player.setPosition(0);
        }
        else if(bang.flag=="STOP")
        {
            player.setPaused(true);
        }
        
	}
}
//--------------------------------------------------------------
void testApp::update(){
    int logl = vebose;
    ofSetLogLevel((ofLogLevel)logl);
    if(logl==OF_LOG_VERBOSE)
    {
        ofDrawGrid();
        //        ofDrawAxis(500);
    }
    //    volumeEnabled = timeline.getValue("VolumeEnabled");
    //    modelEnabled = timeline.getValue("3DModelFile");
    //    videoEnabled = timeline.getValue("VideoEnabled");
	if(volumeEnabled->isOn() || mode==0)
	{
        myVolume.setThreshold(timeline.getValue("Volume Threshold"));
        
        myVolume.setDensity(timeline.getValue("Volume Density"));
        
        myVolume.setXyQuality(timeline.getValue("Volume XyQuality"));
        
        myVolume.setZQuality(timeline.getValue("Volume ZQuality"));
        
	}
	else if (modelEnabled->isOn() || videoEnabled->isOn() || mode==1 || mode==2)
	{
        //		myVideo.update();
		
		sampler2dTex.begin();
		ofClear(0);
		ofDisableNormalizedTexCoords();
		client.draw(0,0,sampler2dTex.getWidth(),sampler2dTex.getHeight());
		ofEnableNormalizedTexCoords();
        //		myVideo.draw(0, 0,sampler2dTex.getWidth(),sampler2dTex.getHeight());
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
    glDisable(GL_DEPTH_TEST);
    ofPushStyle();
    ofSetColor(0);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    ofPopStyle();
	glEnable(GL_DEPTH_TEST);
    ofClear(0);
    //    ofPushView();
    {
        
        
        //        ofViewport(viewportGameCam);
        //        ofSetupScreen();
        cam.begin();
        ofClear(0);
		if(volumeEnabled->isOn() || mode==0)
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
        if(modelEnabled->isOn() || mode==1)
		{
			
            
			ofPushStyle();
			ofPushMatrix();
			ofSetColor(255, 255*timeline.getValue("3DModelAlpha"));
			texMapShader.begin();
			texMapShader.setUniformTexture("colormap",sampler2dTex , 1);
			texMapShader.setUniform1f("alpha",timeline.getValue("3DModelAlpha") );
			model->drawFaces();
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
    
    
    if(timeline.getValue("PD_ON"))
    {
        //        timeline.getValue("PD_START");
        float x = timeline.getValue("PD_X")*ofGetWidth();
        float y = timeline.getValue("PD_Y")*ofGetHeight();
        float w = timeline.getValue("PD_WIDTH")*ofGetWidth();
        float h = timeline.getValue("PD_HEIGHT")*ofGetHeight();
        float a = timeline.getValue("PD_ALPHA")*255;
        player.setVolume(timeline.getValue("PD_VOLUME"));
        player.update();
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255,a);
        player.draw(x,y,w,h);
        ofPopStyle();
        
    }
    if(videoEnabled->isOn() || mode==2)
	{
		colorControl.begin();
        colorControl.setUniform1f("brightness", timeline.getValue("brightness"));
        colorControl.setUniform1f("contrast", timeline.getValue("contrast"));
        colorControl.setUniform1f("saturation", timeline.getValue("saturation"));
        colorControl.setUniform1i("invert", (timeline.isSwitchOn("invert") ? 1 : 0) );
        colorControl.setUniform1f("alpha", timeline.getValue("VideoAlpha") );
        ofPushStyle();
		ofDisableNormalizedTexCoords();
//        ofSetColor(255, 255*timeline.getValue("VideoAlpha"));
        //        myVideo.draw(0,0,ofGetWidth(),ofGetHeight());
		client.draw(0,0,ofGetWidth(),ofGetHeight());
		ofEnableNormalizedTexCoords();
        ofPopStyle();
        //		myVideo.draw(512, 256, 256 , 256);
        //		sampler2dTex.draw(512, 512,256,256);
            colorControl.end();
	}
    ofPushStyle();
    ofEnableAlphaBlending();
    ofSetColor(0);
    maskHeight = timeline.getValue("MASK");
    ofRect(0, 0, ofGetWidth(), maskHeight);
    ofSetColor(255);
    mask.draw(0,maskHeight,ofGetWidth(),ofGetHeight()-maskHeight);
    ofPopStyle();

#ifdef USE_SYPHON
    server.publishScreen();
#endif
	if(toggleDraw)gui.draw();
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
        case '\t':
            toggleDraw = !toggleDraw;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        {
            if(!timeline.isModal())
            {
                int index = key-'1';
                
                if(index<imageSequence.size())initVolumetrics(imageSequence[index]);
            }
        }
            break;
    }
    
    if (key == '`')
    {
        mode = -1;
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
	if(key == OF_KEY_RETURN)
	{
		ofSaveFrame();
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
    mode = -1;
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
    //    if(dragInfo.files[0].find("mov")!=string::npos)
    //    {
    //        myVideo.loadMovie(dragInfo.files[0]);
    //    }
}