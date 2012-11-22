#pragma once

#include "ofMain.h"
#include "ofxAutoControlPanel.h"
#include "ofxCvMain.h"
#include "ofxDuration.h"
#include "DurationPanel.h"
//#include "ofxPeggy.h"
#define NUM_LED 25
#define NUM_PEGGY 2
#define CAMW 320
#define CAMH 240
class LED : public ofVec2f
{
public:
    
    int brightness;
};
class Ripple : public ofVec2f
{
public:
	int size;
	int thickness;
	float   step;
	int prevMillis;
	int maxSize ;
	Ripple()
	{
		size = 0;
		step = ofRandom(0.1,0.5);
		prevMillis = 0;
		maxSize = (int)ofRandom(CAMW*0.5,CAMW);
		x = ofRandom(0,CAMW);
		y = ofRandom(0,CAMH*2);
		thickness = ofRandom(0.1,1);
	}
	void render()
	{
		float diffMillis = ofGetElapsedTimeMillis() - prevMillis;
		prevMillis = ofGetElapsedTimeMillis();
		size+=step*diffMillis;
		if(size>maxSize)
		{
			x = ofRandom(0,CAMW);
			y = ofRandom(0,CAMH*2);
			size=0;
		}
		ofPushStyle();
		ofNoFill();
		ofSetColor(ofMap(maxSize-size,0,maxSize,0,255));
		ofSetLineWidth(thickness);
		ofCircle(x, y, size);
		ofPopStyle();
	}
	
};
class Building
{
public:
	ofImage *image;
	ofVec2f pos;
	float step,prev;
	float scaleH,waiting;
    int brightness;
	Building(ofImage &_image)
	{
		image = &_image;
		pos.x = ofRandom(CAMW);
		pos.y = CAMH*2+image->height;
		step = ofRandom(2,3);
		prev = 0;
		scaleH = ofRandom(0.8,1.2);
		waiting = ofRandom(3,10);
        brightness = 255;
        
	}
	void update(float x, float y , float w ,float h)
	{
		
		if(pos.y<h-image->height*scaleH)
		{

			float diff = ofGetElapsedTimef()-prev;
            brightness = ofMap(diff,0,waiting,255,0);
			if(diff>waiting)
			{
				pos.y = h;
				pos.x = ofRandom(w);
                brightness = 255;
			}
		}
		else
		{		pos.y-=step;
			prev = ofGetElapsedTimef();
		}
	}
	void draw(float x , float y, float w ,float h)
	{
        ofSetColor(brightness);
		image->draw(x+pos.x-image->width*0.5,
					pos.y,
					image->width,
					image->height*scaleH);
	}
	void reset()
	{
		waiting = ofRandom(3,10);
		scaleH = ofRandom(0.8,1.2);
		prev = ofGetElapsedTimef();
		pos.y = CAMH*2+image->height;

	}
	

};
class Buildings :public ofBaseDraws{
public:
	Buildings()
	{
		ofDirectory dir;
		
		dir.allowExt("png");
		int num = dir.listDir("./images/buildings");
		images.assign(num, ofImage());

		
		for(int i = 0 ; i < num ; i++)
		{
			images[i].loadImage(dir.getPath(i));
			width+=images[i].width;
			if(images[i].height > height)height = images[i].height;
		}
		maxBuilding = 20;
	}
	void draw(float x , float y)
	{
	}
	void update(float x , float y , float w , float h)
	{
		for(int i = 0 ; i < _buildings.size() ; i++)
		{
			
			_buildings[i]->update(x,y,w,h);
		}
		if(maxBuilding>_buildings.size() && ofGetFrameNum()%50==0)
		{
			int r = (int)ofRandom(0,images.size());
			Building * newBuilding = new Building(images[r]);
			
			_buildings.push_back(newBuilding);
		}
	}
	void draw(float x , float y , float w , float h)
	{
				
		for(int i = 0 ; i < _buildings.size() ; i++)
		{
			_buildings[i]->draw(x,y,w,h);
		}
	}
	void reset()
	{
		
		for(int i = 0 ; i < _buildings.size() ; i++)
		{
			_buildings[i]->reset();
		}
	}

	float getWidth(){return width;}
	float getHeight(){return height;}
	vector <ofImage> images;
	vector <Building*> _buildings;
 	int maxBuilding;
	
	int width,height;
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
		void gotMessage(ofMessage msg);
		
	
    //gui
    ofxAutoControlPanel				gui;
    void							eventsIn(guiCallbackData & data);
    
	//cv
    ofVideoGrabber					vidGrabber;
    ofxCvColorImage					colorImg;
    ofRectangle						drawRect;
    ofxCvGrayscaleImage				grayImage;
    ofxCvGrayscaleImage				grayBg;
    ofxCvGrayscaleImage				grayDiff;
    ofxCvContourFinder				contourFinder;
	int								threshold,imageBrightness;
	bool							bLearnBakground,bSerial,bFlip,bMirror,bCV,bRipple,bImage,bContour;
	
	//buildings
	Buildings						buildings;
	
	
	//LED
    ofFbo							scaledFbo;
    ofPixels						scaledPixels;
	vector <Ripple*>				ripples;
    vector <LED>					LEDs;
    void							renderToPeggy( int display);
    
    ofSerial						port1,port2;
	int								size,padding;
	
	//duration
	DurationPanel					*durationPanel;
	ofxDuration						duration;
	void							trackUpdated(ofxDurationEventArgs& args);
	string							ip;
	int								port;


};
