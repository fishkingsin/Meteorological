/**
 *  testApp.cpp
 *  ofxTSPSReceiver example
 *
 * Created by James George, http://www.jamesgeorge.org
 * in collaboration with FlightPhase http://www.flightphase.com
 * and the Lab at the Rockwell Group http://lab.rockwellgroup.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 * This addon is a wrapper for receiving openTSPS People Tracking data
 * in an OF project.  This is useful for quickly prototyping interactive
 * designs without having to add all the computer vision code to your oF
 * project.
 *
 * For this example to work you must be running an instance of OpenTSPS configured
 * to send OSC data to this IP on port 12000
 */

#include "testApp.h"
int StringToWString(std::wstring &ws, const std::string &s)
{
    std::wstring wsTmp(s.begin(), s.end());
	
    ws = wsTmp;
	
    return 0;
}
//--------------------------------------------------------------
void testApp::setup(){
	
	//	ofEnableSmoothing();
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	tspsReceiver.connect(12000);
	
	ofxAddTSPSListeners(this);
    initSettings();
    
	setupFromTextFile(lister.getPath(0));
	count = 0;
    ofSetWindowTitle("TextPaint");
    server.setName("TextPaint");
//    screenTex.allocate(ofGetWidth(),ofGetHeight());
}
void testApp::initSettings()
{
	if(!settings.loadFile("./settings/configs.xml"))
	{
		ofLogError()<<"config.xml not found!";
		std::exit(0);
	}
	if(settings.pushTag("SETTINGS"))
	{
		fontSize = settings.getValue("FONT_SIZE",8);
		font.loadFont(settings.getValue("FONT","./fonts/LiHei.ttf"),fontSize);
		
		gui.setup(settings.getValue("GUI_WIDTH",1280),settings.getValue("GUI_HEIGHT",768));
		gui.loadFont(settings.getValue("GUI_FONT","MONACO.TTF"),settings.getValue("GUI_FONT_SIZE",8));
		gui.ofxControlPanel::addPanel("General", 3);
		gui.addToggle("Enable", "ENABLE", false);
		gui.addToggle("Debug", "DEBUG", false);
        
//        ofDirectory dir;
//        dir.allowExt("txt");
//        int num = dir.listDir("./text");
//		for (int  i  = 0 ; i < num; i++) {
//            files.push_back(dir.getPath(i));
//            
//        }
        lister.listDir("text/");
        currentSelectedText = 0;
        gui.addFileLister("TEXT_LISTER", &lister, 200, 300);
        
		gui.setWhichPanel("General");
		gui.setWhichColumn(0);
		gui.addSlider("FadeIntSpeed", "FADE_IN_SPEED", 7000, 1.0, 10000, true);
		
		//guiType2DSlider * ofxControlPanel::addSlider2D(string sliderName, string xmlName, float valueX, float valueY, float minX, float maxX, float minY, float maxY, bool isInt)
		gui.setWhichColumn(1);
		gui.addSlider2D("inputPosition", "INPUT_POSITION",input.x,input.y,0,ofGetWidth()*3,0,ofGetHeight()*3,true);
		gui.addSlider2D("inputSize", "INPUT_SIZE", input.width,input.height,0,ofGetWidth()*3,0,ofGetHeight()*3,true);
		gui.setWhichColumn(2);
		gui.addSlider2D("outputPosition", "OUTPUT_POSITION",output.x,output.y,0,ofGetWidth()*3,0,ofGetHeight()*3,true);
		gui.addSlider2D("outputSize", "OUTPUT_SIZE", output.width,output.height,0,ofGetWidth()*3,0,ofGetHeight()*3,true);
		//  -- this gives you back an ofEvent for all events in this control panel object
		ofAddListener(gui.guiEvent, this, &testApp::eventsIn);
		gui.setupEvents();
		gui.enableEvents();
		gui.loadSettings(settings.getValue("GUI_SETTING_PATH","./settings/control_settings.xml"));
        
	}
	else
	{
		ofLogError()<<" <SETTINGS> not found in config.xml!";
	}
	
}
void testApp::eventsIn(guiCallbackData & data){
	if( data.isElement( "FADE_IN_SPEED" ) ){
		fadeInSpeed = data.getInt(0);
		for (int i = 0 ; i<character.size(); i++) {
			character[i].speed = fadeInSpeed;
		}
	}
	else if( data.isElement( "INPUT_POSITION" ) ){
		input.x = data.getInt(0);
		input.y = data.getInt(1);
		
	}
	else if( data.isElement( "INPUT_SIZE" ) ){
		input.width = data.getInt(0);
		input.height = data.getInt(1);
	}
	else if( data.isElement( "OUTPUT_POSITION" ) ){
		output.x = data.getInt(0);
		output.y = data.getInt(1);
		resetOutputDimension();
	}
	else if( data.isElement( "OUTPUT_SIZE" ) ){
		output.width = data.getInt(0);
		output.height = data.getInt(1);
		resetOutputDimension();
	}
	else if( data.isElement( "ENABLE" ) ){
		enabled = data.getInt(0);
	}
	else if( data.isElement( "DEBUG" ) ){
		debug = data.getInt(0);
	}
    else if( data.isElement( "TEXT_LISTER" ) ){
        ofLogVerbose("TEXT_LISTER") << data.getString(0);
        currentSelectedText = data.getInt(0);
//        setupFromTextFile(lister.getPath(currentSelectedText));
	}
	
}
void testApp::resetOutputDimension()
{
	
	int maxChar = (output.width)/fontSize*1.0;
	if(maxChar>0)
	{
		int step = 0;
//        cout << "setup charactor position "<<endl;
		for (int i = 0 ; i<character.size(); i++) {
//            cout << character[i].charUC << "|";
            
             if(character[i].charUC=="\n" || character[i].charUC=="\r")
            {
                int temp = step%maxChar;
                step+=maxChar;
                step -= temp;
                character[i].offset.set(output.x+(step%maxChar)*fontSize,output.y+(step/maxChar*1.0f)*fontSize*1.1);
            }
			else
            {
                
                character[i].offset.set(output.x+(step%maxChar)*fontSize,output.y+(step/maxChar*1.0f)*fontSize*1.1);
            }
			//		character[i].offset.set(output.x,output.y);
			step++;
		}
	}
}
void testApp::setupFromTextFile(string file_path)
{
	ofFile file(file_path);
	ofBuffer buf = file.readToBuffer();
	
	
	
	ofUniString uni = ofTextConverter::toUTF32(buf.getText());
	count = 0;
	string utf8_1_out;
	
	character.clear();
	character.assign(uni.size(), myCharactor());
	for (int i = 0 ; i<uni.size(); i++) {
		
		
		character[i].font = &font;
		character[i].charUC =  ofTextConverter::toUTF8(uni[i]);
		character[i].setOriginalPosition(ofVec2f(0,ofGetHeight()*2));
        	character[i].speed = gui.getValueI("FADE_IN_SPEED");
		
	}
	resetOutputDimension();
}
void testApp::updateCharactorPosition(float x ,float y)
{
	

	if(count == character.size())
	{
//        count = 0;
        //fadeOut();
//		count = 0;
		//do something
		
		//reset char position
//		for (int i = 0 ; i<character.size(); i++) {
//            character[i].setNewPosition(ofVec2f(x,ofGetHeight()*2));
////			character[i].setOriginalPosition(ofVec2f(0,ofGetHeight()*2));
//            
//		}
		//or load other file
	}
    else{
        character[count].setNewPosition(ofVec2f(x,y));
        count++;
    }
}
//--------------------------------------------------------------
void testApp::update(){
	if(enabled)
	{
		for(int i = 0 ; i < character.size() ; i++)
		{
			
			character[i].update();
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(0);
	
	
	ofPushStyle();
	//
    ofSetColor(255);
	if(enabled)
	{
		for(int i = 0 ; i < character.size() ; i++)
		{
			
			character[i].draw ( );
		}
	}
//    server.publishScreen();
//    screenTex.getS
	if(debug)
	{
		tspsReceiver.draw(ofGetWidth(), ofGetHeight());
		
		ofNoFill();
		ofSetColor(0,255,0);
		ofRect(output);
		ofSetColor(255,0,0);
		ofRect(input);
	}
    server.publishScreen();
	ofPopStyle();
	
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
    // you can access the person like this:
    // tspsEvent.person
    float x = input.x+tspsEvent.person->centroid.x*input.width;
	float y = input.y+tspsEvent.person->centroid.y*input.height;
	updateCharactorPosition(x,y);
}

//--------------------------------------------------------------
void testApp::onPersonWillLeave( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "Person left!");
    // you can access the person like this:
    // tspsEvent.person
	
    
}

void testApp::fadeOut()
{
    
    for (int i = 0 ; i<character.size(); i++) {
        character[i].offset.set(character[i].offset.x,-ofRandom(100,ofGetHeight()));
        
        character[i].setNewPosition(character[i].pos,ofRandom(0,2000));
        character[i].speed = ofRandom(5000,10000);
    }
}
void testApp::fadeOut2()
{
//    count = 0;
    for (int i = 0 ; i<character.size(); i++) {
        character[i].offset.set(ofRandom(0,ofGetWidth()),ofGetHeight()+ofRandom(100,ofGetHeight()));
        
        character[i].setNewPosition(character[i].pos,ofRandom(0,2000));
        character[i].speed = ofRandom(5000,10000);
    }
}
void testApp::fadeOut3()
{
//    count = 0;
    for (int i = 0 ; i<character.size(); i++) {
        character[i].offset.set(character[i].offset.x,ofGetHeight()+ofRandom(100,ofGetHeight()));
        
        character[i].setNewPosition(character[i].pos,ofRandom(0,2000));
        character[i].speed = ofRandom(5000,10000);
    }
}
void testApp::fadeOut4()
{
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key=='1')
    {
        fadeOut();
    }
    if(key=='2')
    {
        fadeOut2();
    }
    if(key=='3')
    {
        fadeOut3();
    }
    if(key=='4')
    {
        fadeOut3();
    }
    if (key == OF_KEY_RIGHT)
    {
        currentSelectedText++;
        currentSelectedText%=lister.getNumEntries();
    }
    if (key == OF_KEY_LEFT)
    {
        currentSelectedText--;
        if(currentSelectedText<0)currentSelectedText=lister.getNumEntries()-1;
    }
    if(key==OF_KEY_RETURN)
    {
        setupFromTextFile(lister.getPath(currentSelectedText));
//        setupFromTextFile("text/data.txt");
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
	if(!gui.hidden)return;
	float _x = input.x+((x*1.0f/ofGetWidth()*1.0f)*input.width);
	float _y = input.y+((y*1.0f/ofGetHeight()*1.0f)*input.height);
	
	updateCharactorPosition(_x,_y);
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
void testApp::dragEvent(ofDragInfo dragInfo) {
	setupFromTextFile(dragInfo.files[0]);
	
}
