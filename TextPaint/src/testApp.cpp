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
#define FONT_SIZE 12
int StringToWString(std::wstring &ws, const std::string &s)
{
    std::wstring wsTmp(s.begin(), s.end());
	
    ws = wsTmp;
	
    return 0;
}
//--------------------------------------------------------------
void testApp::setup(){
	
	ofEnableSmoothing();
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	tspsReceiver.connect(12000);
  
	ofxAddTSPSListeners(this);
    
    font.loadFont("LiHei.ttf",FONT_SIZE , true,true ,0.3,0);
	setupFromTextFile("data.txt");
	count = 0;
}
void testApp::setupFromTextFile(string file_path)
{
	ofFile file(file_path);
	ofBuffer buf = file.readToBuffer();
	
	int step=0;
	int maxChar = (ofGetWidth()-100)/FONT_SIZE;
	
	ofUniString uni = ofTextConverter::toUTF32(buf.getText());
	count = 0;
	string utf8_1_out;
	
	character.clear();
	character.assign(uni.size(), myCharactor());
	for (int i = 0 ; i<uni.size(); i++) {
		
		
		character[i].font = &font;
		character[i].charUC =  ofTextConverter::toUTF8(uni[i]);
		character[i].offset.set(100+(step%maxChar)*FONT_SIZE,100+(step/maxChar*1.0f)*FONT_SIZE*1.1);
		step++;
		character[i].setOriginalPosition(ofVec2f(0,ofGetHeight()*2));
	}
}
void testApp::updateCharactorPosition(float x ,float y)
{
	character[count].setNewPosition(ofVec2f(x,y));
	count++;
	if(count == character.size())
	{
		count = 0;
		//do something
		
		//reset char position
		for (int i = 0 ; i<character.size(); i++) {
			character[i].setOriginalPosition(ofVec2f(0,ofGetHeight()*2));
		}
		//or load other file
	}
}
//--------------------------------------------------------------
void testApp::update(){
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(255, 255, 255);
	ofSetLineWidth(3);
	
	tspsReceiver.draw(ofGetWidth(), ofGetHeight());
    ofSetColor(0, 0, 0);

    for(int i = 0 ; i < character.size() ; i++)
    {
		
        character[i].draw ( );


    }
	
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
    float x = tspsEvent.person->centroid.x*ofGetWidth();
	float y = tspsEvent.person->centroid.y*ofGetHeight();
	updateCharactorPosition(x,y);
}

//--------------------------------------------------------------
void testApp::onPersonWillLeave( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "Person left!");
    // you can access the person like this:
    // tspsEvent.person
	
    
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
	
	updateCharactorPosition(x,y);
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
