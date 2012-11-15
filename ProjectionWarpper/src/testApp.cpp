#include "testApp.h"



#define USE_TRANSPARENCY false
static bool reload = false;

//--------------------------------------------------------------
void testApp::setup(){
	
    ofSetLogLevel(OF_LOG_VERBOSE);
    glDisable(GL_DEPTH_TEST);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
	
	
    settings.loadFile("configs.xml");
	ofSetWindowTitle(settings.getValue("TITLE","Projection Warpper"));
	
	N_SCREEN = settings.getValue("N_SCREEN",2);
	WIDTH = settings.getValue("WIDTH",1024);
	HEIGHT = settings.getValue("HEIGHT",768);
	rm.allocateForNScreens(N_SCREEN,WIDTH,HEIGHT);
	tex.allocate(WIDTH,HEIGHT,GL_RGB);
	rm.loadFromXml(settings.getValue("DEFAULT_SETTING_PATH","fbo_settings.xml"));
	img.loadImage(settings.getValue("IMAGE_FILE","game_of_thrones.jpg"));
	
	
	duration.setup(settings.getValue("PORT",12345));
	
	duration.setupFont("GUI/NewMedia Fett.ttf", 12);
	//ofxDuration is an OSC receiver, with special functions to listen for Duration specific messages
	//optionally set up a font for debugging
	ofAddListener(duration.events.trackUpdated, this, &testApp::trackUpdated);
	
	
	
	gui.setup(ofGetWidth(),ofGetHeight());
	gui.ofxControlPanel::addPanel("General", 1);
	gui.setWhichPanel("General");
	gui.addToggle("ExtendScreen", bExtendScreen) ;
	gui.addToggle("Grid", showGrid) ;
	
	gui.ofxControlPanel::addPanel("SyphonPanel", 1);
	gui.setWhichPanel("SyphonPanel");
	gui.addTextInput("ServerName", "Syphon Server", 128);
	gui.addTextInput("IncomingServerName", "Syphon Client", 128);
	
	
	gui.ofxControlPanel::addPanel("RenderManager", 1);
	gui.setWhichPanel("RenderManager");
	
	gui.ofxControlPanel::addPanel("InputPanel", 1);
	gui.setWhichPanel("InputPanel");
	inputPanel = new InputPanel(&rm);
	gui.addCustomRect("Input Diagnosis", inputPanel, gui.getWidth()*0.8, gui.getHeight()*0.8);
	
	gui.ofxControlPanel::addPanel("OutputPanel", 1);
	gui.setWhichPanel("OutputPanel");
	outputPanel = new OutputPanel(&rm);
	gui.addCustomRect("Output Diagnosis", outputPanel, gui.getWidth()*0.8, gui.getHeight()*0.8);
	
	syphonServer.setName(settings.getValue("OUTGOING_SYPHON_SERVER","Syphon Server"));
	syphonClient.setup();
	
	syphonClient.setApplicationName("");
	syphonClient.setServerName(settings.getValue("INCOMING_SYPHON_SERVER","VideoCue"));
	
}
//--------------------------------------------------------------
//Or wait to receive messages, sent only when the track changed
void testApp::trackUpdated(ofxDurationEventArgs& args){
	ofLogVerbose("Duration Event") << "track type " << args.track->type << " updated with name " << args.track->name << " and value " << args.track->value << endl;
	
	if(args.track->name=="/FBOSetting")
	{
		ofLogVerbose("FBOSetting") << "track type " << args.track->type << " updated with name " <<
		args.track->name << " and value " << args.track->flag.c_str() << endl;
		
		ofFile file(args.track->flag);
		if(file.exists() && file.getExtension()=="xml")
		{
			
			rm.loadFromXml(args.track->flag);
		}
	}
}
//this captures all our control panel events - unless its setup differently in testApp::setup
//--------------------------------------------------------------
void testApp::eventsIn(guiCallbackData & data){
	
	//lets send all events to our logger
	if( !data.isElement( "events logger" ) ){
		string logStr = data.getXmlName();
		
		for(int k = 0; k < data.getNumValues(); k++){
			logStr += " - " + data.getString(k);
		}
		
		logger.log(OF_LOG_NOTICE, "event - %s", logStr.c_str());
	}
	
	// print to terminal if you want to
	//this code prints out the name of the events coming in and all the variables passed
	printf("testApp::eventsIn - name is %s - \n", data.getXmlName().c_str());
	if( data.getDisplayName() != "" ){
		printf(" element name is %s \n", data.getDisplayName().c_str());
	}
	for(int k = 0; k < data.getNumValues(); k++){
		if( data.getType(k) == CB_VALUE_FLOAT ){
			printf("%i float  value = %f \n", k, data.getFloat(k));
		}
		else if( data.getType(k) == CB_VALUE_INT ){
			printf("%i int    value = %i \n", k, data.getInt(k));
		}
		else if( data.getType(k) == CB_VALUE_STRING ){
			printf("%i string value = %s \n", k, data.getString(k).c_str());
		}
	}
	
	printf("\n");
}
#define NUM_BYTE 512
//--------------------------------------------------------------
void testApp::update(){
	if(reload)
    {
        
		reload = false;
    }
	
	rm.startOffscreenDraw();
	ofSetColor(0);
	ofRect(0,0,WIDTH*N_SCREEN,HEIGHT);
    
	ofSetColor(255, 255, 255);
	
	ofPushMatrix();
	ofTranslate(0,0);
    
	ofBackgroundGradient(ofColor(150), ofColor(0));
    
	ofPopMatrix();
	syphonClient.draw(0,0,WIDTH,HEIGHT);
	
	
	if(showGrid)
	{
		//thick grid lines for blending
		ofSetColor(150,150,150);
		ofSetLineWidth(3);
		
		//vertical line
		for(int i = 0; i <= WIDTH ;i+=40){
			if(i%2==0)ofSetColor(255,255,255);
			ofLine(i, 0, i, HEIGHT);
		}
		
		//horizontal lines
		for(int j = 0; j <=HEIGHT; j+=40){
			if(j%2==0)ofSetColor(255,255,255);
			ofLine(0, j, WIDTH, j);
		}
	}
	
	rm.endOffscreenDraw();
    
	
}


//--------------------------------------------------------------
void testApp::draw(){
    
	ofEnableAlphaBlending();
    ofBackground(0);
    ofSetColor(255);
	
	rm.drawScreens();
	if(bExtendScreen)
	{
		ofPushMatrix();
		if(showGrid)
		{
			//thick grid lines for blending
			ofSetColor(255, 255, 255);
			ofSetLineWidth(3);
			
			//vertical line
			for(int i = 0; i <= ofGetScreenWidth() ;i+=40){
				if(i%2==0)ofSetColor(255,255,255);
				ofLine(i, 0, i, ofGetScreenHeight());
			}
			//horizontal lines
			for(int j = 0; j <=ofGetScreenHeight(); j+=40){
				if(j%2==0)ofSetColor(255,255,255);
				ofLine(0, j, ofGetScreenWidth(), j);
			}
		}
		ofTranslate(1920, 0);
		rm.drawScreens();
		ofPopMatrix();
	}
	tex.loadScreenData(0,0,WIDTH,HEIGHT);
	syphonServer.publishTexture(&tex);
	
    
	ofPushStyle();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	//	if(textInput.getIsEditing())return;
    switch(key) {
			break;
		case 'b':
			break;
		case 's':rm.saveToXml();
 			break;
		case 'l':rm.reloadFromXml();
 			break;
		case 'd':rm.resetCoordinates();
 			break;
			
    }
	
	
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
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}



