#include "testApp.h"
#define CAMW 320
#define CAMH 240
string peggy1Serial = "/dev/tty.usbserial-A7004E4E";
string peggy2Serial = "/dev/tty.usbserial-A7004E4F";

unsigned char peggyHeader[6] = {0xde,0xad,0xbe,0xef,1,0};
unsigned char *peggyFrame;
//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofSetBackgroundColor(0);
    port1.setup(peggy1Serial, 115200);
    port2.setup(peggy2Serial, 115200);
    ofLogVerbose("peggyHeader") << peggyHeader;
    peggyFrame = new unsigned char[13*25];
    
    LEDs.assign(NUM_LED*NUM_LED*NUM_PEGGY, LED());
    for(int y = 0 ; y < NUM_LED*NUM_PEGGY ; y++)
    {
        for(int x = 0 ; x < NUM_LED ; x++)
        {
            int i = x+y*NUM_LED;
            LEDs[i].set(x,y);
            
        }
        
    }
    scaledFbo.allocate(NUM_LED, NUM_LED*NUM_PEGGY);
    scaledPixels.allocate(NUM_LED, NUM_LED*NUM_PEGGY,OF_IMAGE_COLOR );
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(CAMW,CAMH);
    colorImg.allocate(CAMW,CAMH);
	grayImage.allocate(CAMW,CAMH);
	grayBg.allocate(CAMW,CAMH);
	grayDiff.allocate(CAMW,CAMH);
    
	bLearnBakground = true;
	threshold = 80;
    
    
    ofxXmlSettings settings;
    settings.loadFile("config.xml");
    
    gui.setup(settings.getValue("GUI_WIDTH",1280),settings.getValue("GUI_HEIGHT",768));
	gui.loadFont(settings.getValue("FONT","MONACO.TTF"), 8);
	gui.ofxControlPanel::addPanel("Settings", 1);
	gui.setWhichPanel("Settings");
    gui.addSlider("LED_SIZE", 1, 1, 100,true);
	gui.addSlider("PADDING", 1, 1, 100,true );
    gui.addToggle("EnableSerial",bSerial);
    gui.addToggle("bFlip",bFlip);
    gui.addToggle("bMirror",bMirror);
    
    gui.ofxControlPanel::addPanel("cv", 3);
	gui.setWhichPanel("cv");
    gui.setWhichColumn(0);
    gui.addToggle("LEARN_BACKGROUND", bLearnBakground );
    gui.addSlider("THRESHOLD", threshold,1,255,true );
    
    gui.setWhichColumn(1);
    gui.addDrawableRect("Color", &colorImg, CAMW*0.5, CAMH*0.5);
    gui.addDrawableRect("Gray", &grayImage, CAMW*0.5, CAMH*0.5);
    
    gui.addDrawableRect("GrayDiff", &grayDiff  , CAMW*0.5, CAMH*0.5);
    gui.addDrawableRect("GrayBG", &grayBg, CAMW*0.5, CAMH*0.5);
    gui.setWhichColumn(2);
    gui.addDrawableRect("scaledFbo", &scaledFbo, CAMW*0.5, CAMH);
    gui.addDrawableRect("contourFinder", &contourFinder, CAMW*0.5, CAMH*0.5);
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
    else if( data.isElement( "EnableSerial" ) )
	{
        bSerial = data.getInt(0);
        
    }
    else if( data.isElement( "bFlip" ) )
	{
        bFlip = data.getInt(0);
        
    }
    else if( data.isElement( "bMirror" ) )
	{
        bMirror = data.getInt(0);
        
    }
    
}
//--------------------------------------------------------------
void testApp::update(){
    bool bNewFrame = false;
    
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
    
	if (bNewFrame){
        
        colorImg.setFromPixels(vidGrabber.getPixels(), CAMW,CAMH);
        
        
        grayImage = colorImg;
		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);
        
	}
    contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
    scaledFbo.begin();
    ofClear(0);
//    contourFinder.draw(0,0,NUM_LED,NUM_LED*NUM_PEGGY);
    float scalex = NUM_LED*1.0f/CAMW*1.0f;
    float scaley = (NUM_LED*NUM_PEGGY)*1.0f/CAMH*1.0f;
    float tx = 0;
    float ty = 0;
    ofPushStyle();
    glPushMatrix();
    if(bMirror)
    {
        scalex = -scalex;
        tx = NUM_LED;
    }
    if(bFlip)
    {
        scaley = -scaley;
        ty = NUM_LED*NUM_PEGGY;
    }
    glTranslatef(tx, ty, 0);
    glScalef( scalex, scaley, 0.0 );


	// ---------------------------- draw the blobs
	ofSetHexColor(0xFFFFFF);
    ofFill();
	for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {

		ofBeginShape();
		for( int j=0; j<contourFinder.blobs[i].nPts; j++ ) {
			ofVertex( contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y );
		}
		ofEndShape();
        
	}
	glPopMatrix();
	ofPopStyle();
    scaledFbo.end();
    scaledFbo.readToPixels(scaledPixels);
    
    if(bSerial){
        //Top screen
        renderToPeggy(0);
        //Bottom screen
        renderToPeggy(1);
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    
    
    for(int y = 0 ; y < NUM_LED*NUM_PEGGY ; y++)
    {
        for(int x = 0 ; x < NUM_LED ; x++)
        {
            int i = x+y*NUM_LED;
            ofPushStyle();
            ofColor c = scaledPixels.getColor(x, y);
            float avg = (c.r+c.g+c.b)*0.333333;

                ofSetColor(avg);
            ofCircle(20+LEDs[i].x*(size+padding), 20+LEDs[i].y*(size+padding), size*0.5);
            ofPopStyle();
        }
    }
    
}
//potfrom morganhk https://github.com/morganhk/Dual-Peggy-display-controller
// "render a ofPixels to the Peggy by transmitting it serially.
// fix to  25x25
// -- from http://planetclegg.com/projects/QC-Peggy.html
// support for 2 peggys

void testApp::renderToPeggy( int display)
{
    int idx = 0;
    
    // iterate over the image, pull out pixels and
    // build an array to serialize to the peggy
    int start_y = (display==0)?0:NUM_LED;
    int end_y = (display==0)?NUM_LED:NUM_LED*NUM_PEGGY;
    for (int y =start_y; y < end_y; y++)
    {
        unsigned val = 0;
        for (int x=0; x < NUM_LED; x++)
        {
            ofColor c = scaledPixels.getColor(x, y);
            int br = ((int)c.getBrightness())>>4;
            if (x % 2 ==0)
                val = (unsigned char)br;
            else
            {
                val = (unsigned char) ((br<<4)|val);
                peggyFrame[idx++]= val;
            }
        }
        peggyFrame[idx++]= val;  // write that one last leftover half-byte
    }
    
    // send the header, followed by the frame
    if (display == 0){
        port1.writeBytes(peggyHeader,6);
        port1.writeBytes(peggyFrame,13*25);
    }else{ 
        port2.writeBytes(peggyHeader,6);
        port2.writeBytes(peggyFrame,13*25);
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