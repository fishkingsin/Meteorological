#include "testApp.h"
int size = 255;
ofPoint pos[6]={ofPoint(size*2,size),
	ofPoint(0,size),
	ofPoint(size,size*2),
	ofPoint(size,0),
	ofPoint(size,size),
	ofPoint(size*3,size)};
//--------------------------------------------------------------
void testApp::setup(){
	glEnable(GL_DEPTH_TEST);
	model.loadModel("body/respitory_body.obj");
//	model.loadModel("NewSquirrel.3ds");
	model.setRotation(1, 90, 1, 0, 0);
	myVideo.loadMovie("myVideo.mov");
	myVideo.play();
	myVideo.setLoopState(OF_LOOP_NORMAL);
	
	cubeMapShader.load("CubeMap");
	myCubeMap.initEmptyTextures(size);
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	myVideo.update();
	myCubeMap.beginDrawingInto2D(size);
	
	myCubeMap.endDrawingInto2D();
	
	for( int i = 0; i < 6; i++ )
	{
		myCubeMap.beginDrawingInto2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i );
		
        ofClear(0,0,0);
		myVideo.getTextureReference().drawSubsection(0, 0, size, size, pos[i].x, pos[i].y, size, size);
		//		myVideo.draw(0, 0, myCubeMap.getWidth(), myCubeMap.getHeight());
		
		
		myCubeMap.endDrawingInto2D();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackgroundGradient(ofColor::white, ofColor::gray);
	glDisable(GL_DEPTH_TEST);
	myVideo.draw(0, 0);
	glEnable(GL_DEPTH_TEST);
	cam.begin();
	
	
	myCubeMap.bind();
	
    cubeMapShader.begin();
    cubeMapShader.setUniform1i("EnvMap", 0);
	
//	myCubeMap.drawSkybox( 800 );
	model.drawFaces();
	
    cubeMapShader.end();
	
	myCubeMap.unbind();
	
	
	
	
	
	cam.end();
	
	
	
	
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
