//
//  KinectPanel.h
//  ProjectionWarpper
//
//  Created by james KONG on 28/11/12.
//
//



#pragma once
#include "guiCustomImpl.h"
#include "guiBaseObject.h" //theo added for isInsideRect()
#include "ofxOpenCv.h"
#include "ofxKinect.h"
class KinectPanel : public guiCustomImpl {
private:
public:
	
	
	KinectPanel()
	{
		// enable depth->video image calibration
		kinect.setRegistration(true);
		
		kinect.init();
		//kinect.init(true); // shows infrared instead of RGB video image
		//kinect.init(false, false); // disable video image (faster fps)
		
		kinect.open();		// opens first available kinect
		//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
		grayImage.allocate(kinect.width, kinect.height);
		grayThreshNear.allocate(kinect.width, kinect.height);
		grayThreshFar.allocate(kinect.width, kinect.height);
		
		nearThreshold = 230;
		farThreshold = 70;
		
		angle = 0;
		kinect.setCameraTiltAngle(angle);
		
		maskFbo.allocate(kinect.width, kinect.height);
		fbo.allocate(kinect.width, kinect.height);
		string shaderProgram = "#version 120\n \
		#extension GL_ARB_texture_rectangle : enable\n \
		\
		uniform sampler2DRect tex0;\
		uniform sampler2DRect maskTex;\
		\
		void main (void){\
		vec2 pos = gl_TexCoord[0].st;\
		\
		vec3 src = texture2DRect(tex0, pos).rgb;\
		float mask = texture2DRect(maskTex, pos).r;\
		\
		gl_FragColor = vec4( src , mask);\
		}";
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
		shader.linkProgram();
		
		// Let´s clear the FBO´s
		// otherwise it will bring some junk with it from the memory
		maskFbo.begin();
		ofClear(0,0,0,255);
		maskFbo.end();
		
		fbo.begin();
		ofClear(0,0,0,255);
		fbo.end();
	}
	
	// -------------------------------------
	void saveSettings(string filename){
	}
	
	void reloadSettings(){
		
	}
	
	void saveSettings(){
		
	}
	
	void loadSettings(string filename) {
		
	}
	
	// -------------------------------------
	float getWidth() {
		return boundingRect.width;
	}
	float getHeight() {
		return boundingRect.height;
	}
	
	void update()
	{
		kinect.update();
		if(kinect.isConnected())
		{
			// there is a new frame and we are connected
			if(kinect.isFrameNew()) {
				
				// load grayscale depth image from the kinect source
				grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
				
				grayThreshNear = grayImage;
				grayThreshFar = grayImage;
				grayThreshNear.threshold(nearThreshold, true);
				grayThreshFar.threshold(farThreshold);
				cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
				
			}
			// update the cv images
			grayImage.flagImageChanged();
			
			//find 1 bolb and maxium size is
			contourFinder.findContours(grayImage, 1, (kinect.width*kinect.height)/2, 20, false);
			
			
			if(contourFinder.nBlobs>0)
			{
				ofxCvBlob blob =  contourFinder.blobs[0];
				ofRectangle rect = blob.boundingRect;
				
				ofPushStyle();
				ofEnableAlphaBlending();
				//			ofSetColor(255, 255, 255 , 100);
				//			ofPushMatrix();
				//
				//			ofTranslate(10, 320);
				//			ofScale(400/kinect.width, 300/kinect.height);
				//
				//			ofPopMatrix();
				//do mask
				{
					ofSetColor(255, 255, 255 , 200);
					maskFbo.begin();
					ofPushMatrix();
					
					ofTranslate(-rect.x,-rect.y);
					ofBeginShape();
					ofFill();
					for( int j=0; j<blob.nPts; j++ ) {
						ofVertex( blob.pts[j].x, blob.pts[j].y );
					}
					ofEndShape();
					ofPopMatrix();
					maskFbo.end();
				}
				maskFbo.draw(0,0);
				
				//apply mask
				{
					fbo.begin();
					ofClear(0, 0, 0, 0);
					
					shader.begin();
					shader.setUniformTexture("maskTex", maskFbo.getTextureReference(), 1 );
					shader.end();
					fbo.end();
					ofPushMatrix();
					
					fbo.draw(rect.x,rect.y);
					ofPopMatrix();
					ofPopStyle();
				}
				
			}
		}
		
	}
	
	void draw(float x, float y, float w, float h) {
		if(kinect.isConnected())
		{
			kinect.drawDepth(x+10, y+10, 400, 300);
			kinect.draw(x+420, y+10, 400, 300);
			
			grayImage.draw(x+10, y+320, 400, 300);
			contourFinder.draw(x+10, y+320, 400, 300);
		}
		else
		{
			ofDrawBitmapString("Kinect is not connected !!", x+20,y+20);
		}
	}
	
	// -------------------------------------
	void mousePressed(float x, float y) {
		
	}
	
	//these values come in with 0,0 being the top left of your bounding rect
	// -------------------------------------
	void mouseDragged(float x, float y){
		
	}
	
	void mouseReleased(){
		
	}
	ofxKinect kinect;
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	ofFbo       maskFbo;
    ofFbo       fbo;
    ofShader    shader;
};