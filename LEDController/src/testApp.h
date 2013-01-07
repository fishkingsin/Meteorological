#pragma once

#include "ofMain.h"
#include "ofxAutoControlPanel.h"

#include "ofxDuration.h"
#include "DurationPanel.h"
//#include "ofxPeggy.h"
#define NUM_LED 25
#define NUM_PEGGY 2
#define CAMW 320
#define CAMH 240
#include "Ball.h"
//#define USE_CV
#ifdef USE_CV
#include "ofxCvMain.h"
#endif
class LED : public ofVec2f
{
public:
    
    int brightness;
};
class Ripple : public ofVec2f
{
public:
	Ripple()
	{
        xSize = ofGetWidth();
        ySize = ofGetHeight();
        block  = 16;
        yRes = ySize/block;
        xRes = xSize/block;
        r0                 = new float*[xRes];
        r1                 = new float*[xRes];
        r2                 = new float*[xRes];
        for(int i = 0 ; i < xRes ; i++)
        {
            r0[i]=	 new float[yRes];
            r1[i]=	 new float[yRes];
            r2[i]=	 new float[yRes];
            
        }
        _decay              = .9;  // the speed at which the waves decay (1.0 is no decay) (0.0 is instant decay to nothing)
        heightMulti        = .02;
        for(int i = 0 ; i < xRes ; i++)
        {
            for(int j = 0 ; j < yRes ; j++)
            {
                r0[i][j] = 0;
                r1[i][j] = 0;
                r2[i][j] = 0;
            }
        }
	}
    void findRipples(){
        for (int y=1; y<yRes-1; y++){
            for (int x=1; x<xRes-1; x++){
                r0[x][y] = (r1[x-1][y] + r1[x+1][y] + r1[x][y-1] + r1[x][y+1]) / 4.0;
                r0[x][y] = r0[x][y] * 2.0 - r2[x][y];
                r0[x][y] *= _decay;
            }
        }
    }
    
    void swapBuffers(){
        for (int y=0; y<yRes; y++){
            for (int x=0; x<xRes; x++){
                r2[x][y] -= (r2[x][y] - r1[x][y]) * _decay;
                r1[x][y] -= (r1[x][y] - r0[x][y]) * _decay;
            }
        }
    }
    void makeRipples(int x,  int y){
        xm = ofMap(x,0,ofGetWidth(),0,xRes-1);
        ym = ofMap(y,0,ofGetHeight(),0,yRes-1);
        for (int y=1; y<yRes-1; y++){
            for (int x=1; x<xRes-1; x++){
                float d = ofDist(xm,ym,x,y);
                if (d < 3){
                    
                    r1[x][y] -= pow(((3 - d)/3.0),2) * 2000.0;
                }
            }
        }
    }
	void render()
	{
        if(ofGetFrameNum()%10==0)
        {
            makeRipples(ofRandom(0,xSize*0.5),ofRandom(0,ySize));
        }
		findRipples();
		swapBuffers();
    }
    void draw()
    {
        for (int y=1; y<yRes-1; y++){
            for (int x=1; x<xRes-1; x++){
                int c = ofMap(r0[x][y],0,100,0,255);
                ofSetColor(c,c,c,c);
                ofRect(x*block,y*block,1*block,1*block);
            }
        }
    }
    float **r0;
    float **r1;
    float **r2;
    float _decay;
    float heightMulti;
    int yRes,xRes;
    float xm, ym;
    int xSize,ySize;
    int block;
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
        ofSetColor(255,brightness);
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
    
#ifdef USE_CV
    //cv
    ofVideoGrabber					vidGrabber;
    ofxCvColorImage					colorImg;
    ofRectangle						drawRect;
    ofxCvGrayscaleImage				grayImage;
    ofxCvGrayscaleImage				grayBg;
    ofxCvGrayscaleImage				grayDiff;
    ofxCvContourFinder				contourFinder;
    int								threshold;
    bool							bLearnBakground,bFlip,bMirror,bCV,bContour,bGrayscale;
#endif

    bool bRipple,bImage,bVideo,bSerial;
    int imageBrightness;
    //buildings
    Buildings						buildings;
    
    
    //LED
    ofFbo							scaledFbo;
    ofPixels						scaledPixels;
    Ripple				ripples;
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
    vector<Ball*>balls;
    ofVideoPlayer player;
    
};
