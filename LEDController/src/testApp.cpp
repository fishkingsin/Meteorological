#include "testApp.h"

string peggy1Serial = "/dev/tty.usbserial-AH0183W2";
string peggy2Serial = "/dev/tty.usbserial-A700eyrb";

unsigned char peggyHeader[6] = {0xde,0xad,0xbe,0xef,1,0};
unsigned char *peggyFrame;
//--------------------------------------------------------------
void testApp::setup(){
    
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
#ifdef USE_CV
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(CAMW,CAMH);
    colorImg.allocate(CAMW,CAMH);
	grayImage.allocate(CAMW,CAMH);
	grayBg.allocate(CAMW,CAMH);
	grayDiff.allocate(CAMW,CAMH);
    
	bLearnBakground = true;
	threshold = 80;
#endif
    
    ofxXmlSettings settings;
    settings.loadFile("settings/config.xml");
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(25);
    ofSetBackgroundColor(0);
    ofEnableAlphaBlending();
    port1.listDevices();
    port1.setup(settings.getValue("PEGGY_SERIAL_NAME",peggy1Serial,0), 115200);
    port2.setup(settings.getValue("PEGGY_SERIAL_NAME",peggy2Serial,1), 115200);
	
	
	ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 30, 125));
	ofxControlPanel::setTextColor(simpleColor(255, 255, 255, 255));
	
	gui.setup(settings.getValue("GUI_WIDTH",1280),settings.getValue("GUI_HEIGHT",768));
	gui.loadFont(settings.getValue("FONT","MONACO.TTF"), 8);
	gui.ofxControlPanel::addPanel("Settings", 1);
	gui.setWhichPanel("Settings");
    gui.addSlider("LED_SIZE", 1, 1, 100,true);
	gui.addSlider("PADDING", 1, 1, 100,true );
    gui.addToggle("EnableSerial",bSerial);
    gui.addToggle("bRipple",bRipple);
    gui.addToggle("bImage",bImage);
    gui.addToggle("+RAIN",false);
    gui.addToggle("-RAIN",false);
#ifdef USE_CV
    
    gui.addToggle("bFlip",bFlip);
    gui.addToggle("bMirror",bMirror);
	gui.addToggle("bCV",bCV);
	gui.addToggle("bContour",bContour);
	gui.addToggle("bGrayscale",bGrayscale);
#endif
    gui.addToggle("bVideo",bVideo);
    gui.addDrawableRect("Moon", &player, 25, 50);
    //	gui.addSlider("rippleBrightness",rippleBrightness,0,255,true );
	gui.addSlider("imageBrightness",imageBrightness,0,255,true );
	gui.addToggle("resetBuilding",false);
	
#ifdef USE_CV
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
    
    gui.addDrawableRect("contourFinder", &contourFinder, CAMW*0.5, CAMH*0.5);
#endif
    gui.addDrawableRect("scaledFbo", &scaledFbo, CAMW*0.5, CAMH);
	gui.ofxControlPanel::addPanel("duration", 1);
	gui.setWhichPanel("duration");
	duration.setup(settings.getValue("DURATION_PORT",12345));
	//ofxDuration is an OSC receiver, with special functions to listen for Duration specific messages
	//optionally set up a font for debugging
	duration.setupFont("GUI/NewMedia Fett.ttf", 12);
	ofAddListener(duration.events.trackUpdated, this, &testApp::trackUpdated);
	
	durationPanel = new DurationPanel(&duration);
	gui.addCustomRect("Duration", durationPanel, gui.getWidth()*0.8, gui.getHeight()*0.8);
	
	//  -- this gives you back an ofEvent for all events in this control panel object
	ofAddListener(gui.guiEvent, this, &testApp::eventsIn);
	gui.setupEvents();
	gui.enableEvents();
	gui.loadSettings("./settings/control_settings.xml");
    
	//    ripples.assign(10,Ripple());
    //	for(int i= 0 ; i < 10 ; i++)
    //	{
    //		ripples.push_back(new Ripple());
    //	}
	player.loadMovie("moon_phase.mov");
    player.setLoopState(OF_LOOP_NONE);
	player.play();
    player.setPaused(true);
    bVideo = false;
}
void testApp::exit()
{
    ofLogVerbose("Serial") << "close";
    port1.close();
    port2.close();
}
//--------------------------------------------------------------
//Or wait to receive messages, sent only when the track changed
void testApp::trackUpdated(ofxDurationEventArgs& args){
	ofLogVerbose("Duration Event") << "track type " << args.track->type << " updated with name " << args.track->name << " and value " << args.track->value << endl;
    if( args.track->name == "/EnableSerial"  && args.track->type == "Switches")
	{
        bSerial = args.track->on;
		gui.setValueB("EnableSerial", bSerial);
        
    }
    
#ifdef USE_CV
	if( args.track->name == "/LEARN_BACKGROUND"  && args.track->type == "Bangs")
	{
        grayBg = grayImage;
    }
    else if( args.track->name == "/THRESHOLD"  && args.track->type == "Curves")
	{
        threshold = args.track->value*255;
		gui.setValueI("THRESHOLD", args.track->value*255);
        
    }
    else if( args.track->name == "/bFlip" && args.track->type == "Switches")
	{
		bFlip = args.track->on;
		gui.setValueB("bFlip", bFlip);
		
    }
    else if( args.track->name == "/bMirror" && args.track->type == "Switches" )
	{
		bMirror = args.track->on;
		gui.setValueB("bMirror", bMirror);
		
    }
	else if( args.track->name == "/bCV" && args.track->type == "Switches" )
	{
		bCV = args.track->on;
		gui.setValueB("bCV", bCV);
		
    }
	
#endif
    else if( args.track->name == "/imageBrightness" && args.track->type == "Curves" )
	{
		imageBrightness = args.track->value*255;
		gui.setValueB("imageBrightness", imageBrightness);
		
    }
    else if( args.track->name == "/bRipple" && args.track->type == "Switches" )
	{
		bRipple = args.track->on;
		gui.setValueB("bRipple", bRipple);
		
    }
	else if( args.track->name == "/resetBuilding" && args.track->type == "Bangs" )
	{
		buildings.reset();
		
    }
    //	else if( args.track->name == "/rippleBrightness" && args.track->type == "Curves" )
    //	{
    //		rippleBrightness = args.track->value*255;
    //		gui.setValueB("rippleBrightness", rippleBrightness);
    //
    //    }
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
    else if( data.isElement( "-RAIN" ) )
	{
        if(!balls.empty())
        {
        Ball*ball =  balls.back();
        ball->~Ball();
        balls.pop_back();
        }
         gui.setValueB("-RAIN",false);
        
    }
    else if( data.isElement( "+RAIN" ) )
	{
        balls.push_back(new Ball(ofRandom(NUM_LED),0,NUM_LED,NUM_LED*2,0,ofRandom(1,2)));
         gui.setValueB("+RAIN",false);
    }
#ifdef USE_CV
    else if( data.isElement( "LEARN_BACKGROUND" ) )
	{
        
        gui.setValueB("LEARN_BACKGROUND", false);
        grayBg = grayImage;
    }
    
    else if( data.isElement( "THRESHOLD" ) )
	{
        threshold = data.getInt(0);
        
    }
    else if( data.isElement( "bFlip" ) )
	{
        bFlip = data.getInt(0);
        
    }
    else if( data.isElement( "bMirror" ) )
	{
        bMirror = data.getInt(0);
        
    }
    else if( data.isElement( "bCV" ) )
	{
        bCV = data.getInt(0);
        
    }
	
	else if( data.isElement( "bContour" ) )
	{
        bContour = data.getInt(0);
        
    }
	else if( data.isElement( "bGrayscale" ) )
	{
        bGrayscale = data.getInt(0);
        
    }
#endif
    else if( data.isElement( "bRipple" ) )
	{
        bRipple = data.getInt(0);
        
    }
    
    else if( data.isElement( "EnableSerial" ) )
	{
        bSerial = data.getInt(0);
        
    }
    
	else if( data.isElement( "bImage" ) )
	{
        bImage = data.getInt(0);
        
    }
	else if( data.isElement( "imageBrightness" ) )
	{
        imageBrightness = data.getInt(0);
        
    }
	else if( data.isElement( "resetBuilding" ) )
	{
        gui.setValueB("resetBuilding",false);
		buildings.reset();
    }
    
    
}
//--------------------------------------------------------------
void testApp::update(){
    bool bNewFrame = false;
    player.update();
#ifdef USE_CV
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
#endif
    scaledFbo.begin();
    ofClear(0);
    if (bVideo) {
        ofSetColor(255);
        player.draw(0,0,scaledFbo.getWidth() ,scaledFbo.getHeight());
    }
    else
    {

            float scalex = NUM_LED*1.0f/CAMW*1.0f;
            float scaley = NUM_LED*1.0f/CAMH*1.0f;
            int x = 0;
            glPushMatrix();
            glScalef( scalex, scaley, 0.0 );
        if(bImage)
        {
            buildings.addBuilding();
        }
            buildings.update(0,0,CAMW,CAMH*2);
        
            buildings.draw(0,0,CAMW,CAMH*2);
            
            glPopMatrix();
        
        if(bRipple)
        {
            if(ofGetFrameNum()%10==0)
            {
                ripples.makeRipples(ofRandom(0,ripples.xSize*0.5),ofRandom(0,ripples.ySize));
            }
        }
        {
            ripples.render();
            ofPushStyle();
            		ofSetColor(255);
            float scalex = NUM_LED*2.0f/((50)*1.0f);
            float scaley = scalex;//(NUM_LED*NUM_PEGGY)*1.0f/CAMH*2.0f;
            glPushMatrix();
            glScalef( scalex, scaley, 0.0 );
            //		vector <Ripple*>::iterator r;
            
            ripples.draw();
            //		for(r = ripples.begin() ; r!=ripples.end() ;r++)
            //		{
            //			Ripple * ripple = *r;
            //			ripple->render();
            //		}
            glPopMatrix();
            ofPopStyle();
        }
#ifdef USE_CV
        float scalex = NUM_LED*1.0f/CAMW*1.0f;
        float scaley = (NUM_LED*NUM_PEGGY)*1.0f/CAMH*1.0f;
        if(bCV)
        {
            
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
            
            //		if(bImage)
            //		{
            //			ofSetColor(imageBrightness);
            //			colorImg.draw(0,0,CAMW,CAMH);
            //		}
            if(bContour)
            {
                // ---------------------------- draw the blobs
                ofSetColor(imageBrightness);
                ofFill();
                for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
                    
                    ofBeginShape();
                    for( int j=0; j<contourFinder.blobs[i].nPts; j++ ) {
                        ofVertex( contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y );
                    }
                    ofEndShape();
                    
                }
            }
            else if (bGrayscale)
            {
                grayImage.draw(0, 0,CAMW,CAMH);
            }
            
            glPopMatrix();
            
            ofPopStyle();
        }
#endif
        {
            glPushMatrix();
            //        float scalex = NUM_LED*1.0f/CAMW*1.0f;
            //        float scaley = (NUM_LED*NUM_PEGGY)*1.0f/CAMH*1.0f;
            //        glScalef( scalex, scalex, 0.0 );
            for (int i = 0; i < balls.size(); i++) {
                balls[i]->calc();
                balls[i]->draw();
            }
            glPopMatrix();
        }
    }
    ofPushStyle();
    ofSetColor(0);
    float h = scaledFbo.getHeight();
    ofRect(0,h-h*0.02,scaledFbo.getWidth() ,h*0.02);
    ofPopStyle();
	scaledFbo.readToPixels(scaledPixels);
    scaledFbo.end();
    
    
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
			ofFill();
            ofColor c = scaledPixels.getColor(x, y);
            ofSetColor(c.getBrightness());
            ofCircle(20+LEDs[i].x*(size+padding), 20+LEDs[i].y*(size+padding), size*0.5);
            ofNoFill();
            ofSetColor(100);
            ofCircle(20+LEDs[i].x*(size+padding), 20+LEDs[i].y*(size+padding), size*0.5);
            ofPopStyle();
        }
    }
	buildings.draw(ofGetWidth()*0.5,0,CAMW,CAMH);
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5,0);
    ripples.draw();
    for (int i = 0; i < balls.size(); i++) {
        
        balls[i]->draw();
    }
    ofPopMatrix();
    //	int x = 0;
    //	for(int i = 0 ; i < images.size() ; i++)
    //	{
    //		images[i].draw(ofGetWidth()*0.5 -(ofGetFrameNum()) %(totalWidth)+x, CAMH-images[i].height);
    //		x+=images[i].width;
    //	}
    
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
    for (int y =24; y >=0 ; y--)
    {
        unsigned val = 0;
        for (int x=24; x >=0; x--)
        {

//    for (int y =start_y; y < end_y; y++)
//    {
//        unsigned val = 0;
//        for (int x=0; x < NUM_LED; x++)
//        {
            ofColor c;
            if(display==0)
            {
                c = scaledPixels.getColor((y-24)*-1, (x-24)*-1);
            }
            else{
                c = scaledPixels.getColor(y, x+25   );
            }
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
    switch(key)
    {
        case '1':
        {
            balls.push_back(new Ball(0,ofRandom(NUM_LED*2),NUM_LED,NUM_LED*2,ofRandom(1,2),0));
        }
            break;
        case '2':
        {
            balls.push_back(new Ball(ofRandom(NUM_LED),0,NUM_LED,NUM_LED*2,0,ofRandom(1,2)));
        }
            break;
        case '3':
            while(!balls.empty())
            {
                Ball*ball =  balls.back();
                ball->~Ball();
                balls.pop_back();
            }
            balls.clear();
            break;
        case '4':
            player.setPaused(false);
            player.setPosition(0);
            bVideo = true;
            break;
        case '5':
            bVideo = false;
            player.setPaused(true);
            player.setPosition(0);
            
            break;
        case OF_KEY_BACKSPACE:
            if(!balls.empty())
            {
                Ball*ball =  balls.back();
                ball->~Ball();
                balls.pop_back();
            }
            break;
    }
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