#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    ofSetBackgroundColor(0);
    LEDs.assign(NUM_LED*NUM_LED, LED());
    for(int y = 0 ; y < NUM_LED ; y++)
    {
        for(int x = 0 ; x < NUM_LED ; x++)
        {
            int i = x+y*NUM_LED;
            LEDs[i].set(x,y);
            
        }
        
    }
    
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(320,240);
    colorImg.allocate(320,240);
	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);
    
	bLearnBakground = true;
	threshold = 80;

    
    ofxXmlSettings settings;
    settings.loadFile("config.xml");
    
    gui.setup(settings.getValue("GUI_WIDTH",1280),settings.getValue("GUI_HEIGHT",768));
	gui.loadFont(settings.getValue("FONT","MONACO.TTF"), 8);
	gui.ofxControlPanel::addPanel("General", 1);
	gui.setWhichPanel("Settings");
    gui.addSlider("LED_SIZE", 1, 1, 100,true);
	gui.addSlider("PADDING", 1, 1, 100,true );
    gui.addSlider("PADDING", 1, 1, 100,true );
    gui.addToggle("LEARN_BACKGROUND", bLearnBakground );
    gui.addSlider("THRESHOLD", threshold,1,255,true );
    
	//  -- this gives you back an ofEvent for all events in this control panel object
	ofAddListener(gui.guiEvent, this, &testApp::eventsIn);
	gui.setupEvents();
	gui.enableEvents();
	gui.loadSettings("control_settings.xml");
    


}
void testApp::eventsIn(guiCallbackData & data){
	if( data.isElement( "LED_SIZE" ) )
	{
        size = data.getInt(0);
    }
    else if( data.isElement( "PADDING" ) )
	{
        padding = data.getInt(0);
    }
    else if( data.isElement( "LEARN_BACKGROUND" ) )
	{

        gui.setValueB("LEARN_BACKGROUND", false);
        grayBg = grayImage;
    }
    else if( data.isElement( "THRESHOLD" ) )
	{
        threshold = data.getInt(0);
       
    }

}
//--------------------------------------------------------------
void testApp::update(){
    bool bNewFrame = false;
    
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
    
	if (bNewFrame){
        
        colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);

        
        grayImage = colorImg;
		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);
	}
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    ofSetColor(255);
    vector<LED>::iterator it;
    for(it = LEDs.begin() ; it!=LEDs.end() ;it++)
    {
        ofCircle(20+it->x*(size+padding), 20+it->y*(size+padding), size*0.5);
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}