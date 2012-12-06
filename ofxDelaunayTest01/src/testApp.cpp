#include "testApp.h"
#include "ofxDelaunay.h"
ofxDelaunay delaunay;
//--------------------------------------------------------------
void testApp::setup(){
	for(int i = 0 ; i < 10 ; i++)
	{
	Ball* ball = new Ball(ofRandomWidth(), ofRandomHeight()	, ofGetWidth(),ofGetHeight());
    balls.push_back(ball);
		delaunay.addPoint(ofPoint(ball->x,ball->y));
    }
    delaunay.triangulate();
}

//--------------------------------------------------------------
void testApp::update(){
	delaunay.reset();
    for (int i = 0; i < balls.size(); i++) {
        balls[i]->calc();
//        balls[i]->draw();
		delaunay.addPoint(ofPoint(balls[i]->x,balls[i]->y));
    }
    delaunay.triangulate();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackgroundGradient(ofColor::white, ofColor::gray);
	ofBackgroundGradient(ofColor::white, ofColor(200,200,200), OF_GRADIENT_LINEAR);

    ofSetColor(100,100,100,50);
	delaunay.triangleMesh.drawFaces();
	ofSetColor(100,0,100,50);
	delaunay.triangleMesh.drawWireframe();
	ofSetColor(0,100,100,50);
	delaunay.triangleMesh.drawVertices();
	
	
	ofSetColor(ofColor::red);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 10,10);
	
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
	Ball* ball = new Ball(x, y, ofGetWidth(),ofGetHeight());
	balls.push_back(ball);
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