#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxCubeMap.h"
//#define USE_CUBEMAP
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
	ofVideoPlayer myVideo;
	ofxAssimpModelLoader model;
	ofEasyCam cam;
	ofxCubeMap myCubeMap;
	#ifdef USE_CUBEMAP
	
	ofShader cubeMapShader;
#else
	ofShader texMapShader;
	ofImage colormap,bumpmap;
	GLUquadricObj *quadratic;
	ofFbo sampler2dTex;
#endif
	bool bAnimate;
	float animationTime;
};
