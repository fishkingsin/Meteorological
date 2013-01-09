/**
 *  testApp.h
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

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxTSPSReceiver.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxTween.h"
#include "ofUTF8.h"
#include "ofUnicode.h"
#include "ofTextConverter.h"
#define PORT 12000
#include "ofxAutoControlPanel.h"
#include "ofxSyphon.h"
class myCharactor
{
	public :
	myCharactor()
	{
		speed = ofRandom(500,2000);
	}
	void update()
	{
		pos.x = tweenX.update();
        pos.y = tweenY.update();
	}
	void draw()
	{
		
		font->drawString(charUC,pos.x,pos.y);
	}
    
	void setOriginalPosition(ofVec2f v2)
	{
		tweenX.setParameters(0,easing,ofxTween::easeOut,v2.x,v2.x,0,0.);
		tweenY.setParameters(0,easing,ofxTween::easeOut,v2.y,v2.y,0,0.);
	}
	void setNewPosition(ofVec2f v2,int delay = 0)
	{
		tweenX.setParameters(1,easing,ofxTween::easeOut,v2.x,offset.x,speed,delay);
		tweenY.setParameters(1,easing,ofxTween::easeOut,v2.y,offset.y,speed,delay);
	}
    ofPoint pos;
	ofxTween tweenX;
	ofxTween tweenY;
	ofxEasingQuad 	easing;
	string charUC;
	ofVec2f offset;
	ofxTrueTypeFontUC *font;
	int speed;
};
class testApp : public ofBaseApp{
    
public:
	void setup();
	void update();
	void draw();
    
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void setupFromTextFile(string path);
	void updateCharactorPosition(float x , float y);
	ofxTSPS::Receiver tspsReceiver;
    
	// event listeners
	void onPersonEntered( ofxTSPS::EventArgs & tspsEvent );
	void onPersonUpdated( ofxTSPS::EventArgs & tspsEvent );
	void onPersonWillLeave( ofxTSPS::EventArgs & tspsEvent );
	
	//font
	int fontSize;
	ofxTrueTypeFontUC font;
    vector<myCharactor>character;
	int fadeInSpeed;
	int count;
	
	//control panel
	ofxAutoControlPanel gui;
	ofxXmlSettings settings;
    simpleFileLister lister;
    int currentSelectedText;
	void eventsIn(guiCallbackData & data);
	void initSettings();
	void resetOutputDimension();
	ofRectangle output;
	ofRectangle input;
	bool enabled,debug;
    void fadeOut();
    void fadeOut2();
    void fadeOut3();
    void fadeOut4();
    ofxSyphonServer server;
    ofTexture screenTex;
    vector<string>files;;
};

#endif
