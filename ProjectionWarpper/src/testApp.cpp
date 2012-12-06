#include "testApp.h"



#define USE_TRANSPARENCY false


//--------------------------------------------------------------
void testApp::setup(){
	
    ofSetLogLevel(OF_LOG_VERBOSE);
    glDisable(GL_DEPTH_TEST);
    ofEnableAlphaBlending();
//    ofEnableSmoothing();
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
	
	
    if(!settings.loadFile("./settings/configs.xml"))
	{
		ofLogError()<<"config.xml not found!";
		std::exit(0);
	}
	if(settings.pushTag("SETTINGS"))
	{
		ofSetWindowTitle(settings.getValue("TITLE","Projection Warpper"));
		
		N_SCREEN = settings.getValue("N_SCREEN",2);
		WIDTH = settings.getValue("WIDTH",1024);
		HEIGHT = settings.getValue("HEIGHT",768);
		rm.allocateForNScreens(N_SCREEN,WIDTH,HEIGHT);
		tex.allocate(WIDTH,HEIGHT,GL_RGB);
		rm.loadFromXml(settings.getValue("FBO_SETTING_PATH","fbo_settings.xml"));
		img.loadImage(settings.getValue("IMAGE_FILE","game_of_thrones.jpg"));
		
		
		//	duration.setup(settings.getValue("PORT",12345));
		//
		//	duration.setupFont(settings.getValue("FONT","GUI/NewMedia Fett.ttf"), 12);
		//ofxDuration is an OSC receiver, with special functions to listen for Duration specific messages
		//optionally set up a font for debugging
		//	ofAddListener(duration.events.trackUpdated, this, &testApp::trackUpdated);
		
		
		
		gui.setup(settings.getValue("GUI_WIDTH",1280),settings.getValue("GUI_HEIGHT",768));
		gui.loadFont(settings.getValue("FONT","MONACO.TTF"),settings.getValue("FONT_SIZE",8));
		gui.ofxControlPanel::addPanel("General", 1);
		gui.setWhichPanel("General");
		gui.addToggle("ExtendScreen", bExtendScreen) ;
		gui.addToggle("Grid", showGrid) ;
		gui.addToggle("showDemoPic", showDemoPic) ;
		gui.addToggle("bEnableTriangleMesh", bEnableTriangleMesh) ;
		
		//	vector <string> list;
		//	list.push_back("ExtendScreen");
		//	list.push_back("Grid");
		//	gui.createEventGroup("BUTTON_EVENT_GROUP", list);
		//	ofAddListener(gui.getEventGroup("BUTTON_EVENT_GROUP"), this, &testApp::eventsIn);
		gui.setWhichPanel(1);
		gui.enableIgnoreLayoutFlag();
		gui.addLogger("events logger", &logger, 410, 300);
		gui.disableIgnoreLayoutFlag();
		
		gui.ofxControlPanel::addPanel("SyphonPanel", 1);
		gui.setWhichPanel("SyphonPanel");
		outServName = gui.addTextInput("OutgoingServerName", "Syphon Server", 128);
		gui.addToggle("SetOutServName", false) ;
		inServName = gui.addTextInput("IncomingServerName", "VideoCue", 128);
		inServAppName = gui.addTextInput("IncomingServerApplicationName", "VideoCue", 128);
		gui.addToggle("SetInServName", false) ;
		
		gui.addToggle("Enable_Incoming", false) ;
		gui.addToggle("Enable_Outgoing", false) ;
		vector <string> list;
		list.push_back("SetOutServName");
		list.push_back("SetInServName");
		list.push_back("Enable_Incoming");
		list.push_back("Enable_Outgoing");
		gui.createEventGroup("SyphonEvent_GROUP", list);
		ofAddListener(gui.getEventGroup("SyphonEvent_GROUP"), this, &testApp::SyphonEvent);
		
		gui.ofxControlPanel::addPanel("RenderManager", 1);
		gui.setWhichPanel("RenderManager");
		gui.addSlider2D("Screen Position", "SCREEN_POSTION", 0,0,
						0, ofGetScreenWidth(),
						0, ofGetScreenHeight(),true);
		gui.ofxControlPanel::addPanel("InputPanel", 1);
		gui.setWhichPanel("InputPanel");
		inputPanel = new InputPanel(&rm);
		gui.addCustomRect("Input Diagnosis", inputPanel, gui.getWidth()*0.8, gui.getHeight()*0.8);
		
		gui.ofxControlPanel::addPanel("OutputPanel", 1);
		gui.setWhichPanel("OutputPanel");
		outputPanel = new OutputPanel(&rm);
		gui.addCustomRect("Output Diagnosis", outputPanel, gui.getWidth()*0.8, gui.getHeight()*0.8);
		
		
		
		//  -- this gives you back an ofEvent for all events in this control panel object
		ofAddListener(gui.guiEvent, this, &testApp::eventsIn);
		gui.setupEvents();
		gui.enableEvents();
		gui.loadSettings(settings.getValue("GUI_SETTING_PATH","./settings/control_settings.xml"));
		
		syphonServer.setName(settings.getValue("OUTGOING_SYPHON_SERVER","Syphon Server"));
		syphonClient.setup();
		
		syphonClient.setApplicationName(settings.getValue("INCOMING_SYPHON_SERVER_APPLICATION","QLab"));
		syphonClient.setServerName(settings.getValue("INCOMING_SYPHON_SERVER","QLab"));
	}
	else
	{
		ofLogError()<<" <SETTINGS> not found in config.xml!";
	}
	
	tspsReceiver.connect(12000);
	
	ofxAddTSPSListeners(this);
	
}

//--------------------------------------------------------------
void testApp::SyphonEvent(guiCallbackData & data){
	//	printf("testApp::SyphonEvent - name is %s - \n", data.getXmlName().c_str());
	if( data.isElement( "SetOutServName" ) && data.getInt(0) == 1 ){
		syphonServer.setName( outServName->getValueText() );
		gui.setValueB("SetOutServName", false);
		logger.log(OF_LOG_NOTICE, "syphonServer.setName %s",outServName->getValueText().c_str() );
		ofLogVerbose("syphonServer.setName ")<<outServName->getValueText();;
	}
	else if( data.isElement( "SetInServName" ) && data.getInt(0) == 1 ){
		syphonClient.setServerName(inServName->getValueText());
		syphonClient.setApplicationName(inServAppName->getValueText());
		gui.setValueB("SetInServName", false);
		
		logger.log(OF_LOG_NOTICE, "syphonClient.setName %s",inServName->getValueText().c_str() );
		ofLogVerbose("syphonClient.setName ")<<inServName->getValueText();
	}
	else if( data.isElement( "Enable_Incoming" ))
	{
		bSyphonClient = data.getInt(0);
	}
	else if( data.isElement( "Enable_Outgoing" ))
	{
		bSyphonServer = data.getInt(0);
	}
	
}
void testApp::eventsIn(guiCallbackData & data){
	if( data.isElement( "ExtendScreen" ) )//&& data.getInt(0) == 1 )
	{
		bExtendScreen = data.getInt(0);
		//		gui.setValueB("ExtendScreen", false);
	}
	if( data.isElement( "Grid" ) )//&& data.getInt(0) == 1 )
	{
		showGrid = data.getInt(0);
		//		gui.setValueB("Grid", false);
	}
	if(data.isElement("showDemoPic"))
	{
		showDemoPic = data.getInt(0);
	}
	if(data.isElement("bEnableTriangleMesh"))
	{
		bEnableTriangleMesh = data.getInt(0);
	}
	
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

//called when the person enters the system
//--------------------------------------------------------------
void testApp::onPersonEntered( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "New person!");
    // you can access the person like this:
    // tspsEvent.person
}

//--------------------------------------------------------------
void testApp::onPersonUpdated( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "Person updated!");
	if(bEnableTriangleMesh)
	{
		ofxTSPS::Person* person = tspsEvent.person;
		
		ofPolyline lineRespaced;
		for(int j = 0 ; j < person->contour.size() ; j++)
		{
			lineRespaced.addVertex(person->contour[j].x*WIDTH,person->contour[j].y*HEIGHT);
		}
		lineRespaced.addVertex(lineRespaced[0]);
		
		lineRespaced = lineRespaced.getResampledBySpacing(20);
		// I want to make sure the first point and the last point are not the same, since triangle is unhappy:
		lineRespaced.getVertices().erase(lineRespaced.getVertices().begin());
		
		// if we have a proper set of points, mesh them:
		if (lineRespaced.size() > 5){
			
			mesh.triangulate(lineRespaced, -1, 200);
			int num = mesh.triangulatedMesh.getNumVertices();
			//apply texcoord
			if(bSyphonClient){
				for (int j = 0 ;  j < num ; j++)
				{
					ofVec2f v2= mesh.triangulatedMesh.getVertex(j);
					mesh.triangulatedMesh.addTexCoord(v2);
					
				}
			}
			else
			{
				for (int j = 0 ;  j < num ; j++)
				{
					ofVec2f v2= mesh.triangulatedMesh.getVertex(j);
					mesh.triangulatedMesh.addTexCoord(v2);
					
				}
			}
			//apply color
			//			for(int i = 0 ; i <mesh.triangles.size() ; i++)
			//			{
			//				ofVec2f v2 = mesh.triangles[i].pts[0];
			//				mesh.triangles[i].randomColor = vidGrabber.getPixelsRef().getColor(v2.x, v2.y);
			//
			//			}
		}
	}
	
	
}

//--------------------------------------------------------------
void testApp::onPersonWillLeave( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "Person left!");
	
	
    
}
#define NUM_BYTE 512
//--------------------------------------------------------------
void testApp::update(){
	
	
	rm.startOffscreenDraw();
	ofSetColor(0);
	ofRect(0,0,WIDTH*N_SCREEN,HEIGHT);
    
	ofSetColor(255, 255, 255);
	
	ofPushMatrix();
	ofTranslate(0,0);
    
	ofBackgroundGradient(ofColor(150), ofColor(0));
    
	ofPopMatrix();
	if(bEnableTriangleMesh)
	{
		
		
		ofPushMatrix();
		if(bSyphonClient){
			syphonClient.bind();
			mesh.triangulatedMesh.drawFaces();
			syphonClient.unbind();
		}
		else{
			mesh.draw();
		}
		ofPopMatrix();
	}
	else if(bSyphonClient) syphonClient.draw(0,0,WIDTH,HEIGHT);
	
	if(showDemoPic)img.draw(0,0,WIDTH,HEIGHT);
	if(showGrid)
	{
		ofPushStyle();
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
		ofPopStyle();
	}
	
	rm.endOffscreenDraw();
    
	
}


//--------------------------------------------------------------
void testApp::draw(){
    
	ofEnableAlphaBlending();
    ofBackground(0);
    ofSetColor(255);
	ofPushMatrix();
	ofTranslate(gui.getValueI("SCREEN_POSTION",0), gui.getValueI("SCREEN_POSTION",1));
	rm.drawScreens();
	ofPopMatrix();
	if(bExtendScreen)
	{
		ofPushMatrix();
		if(showGrid)
		{
			ofPushStyle();
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
			ofPopStyle();
		}
		ofTranslate(1920, 0);
		//		rm.drawScreens();
		ofPopMatrix();
	}
	if(bSyphonClient)
	{
		tex.loadScreenData(0,0,WIDTH,HEIGHT);
		syphonServer.publishTexture(&tex);
	}
	
    
	ofPushStyle();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	//	if(textInput.getIsEditing())return;
	bool control_panel_ate_key = gui.ofxControlPanel::keyPressed( key );
	
	if ( !control_panel_ate_key )
	{
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



