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
	bAnimate = false;
	animationTime	= 0.0;
	myVideo.update();
	for( int i = 0; i < 6; i++ )
	{
		myCubeMap.beginDrawingInto2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i );
		
        ofClear(0,0,0);
		myVideo.getTextureReference().drawSubsection(0, 0, size, size, pos[i].x, pos[i].y, size, size);
		myCubeMap.endDrawingInto2D();
	}
}

//--------------------------------------------------------------
void testApp::update(){
	
	myVideo.update();
	
	for( int i = 0; i < 6; i++ )
	{
		myCubeMap.beginDrawingInto2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i );
		
        ofClear(0,0,0);
		myVideo.getTextureReference().drawSubsection(0, 0, size, size, pos[i].x, pos[i].y, size, size);
		//		myVideo.draw(0, 0, myCubeMap.getWidth(), myCubeMap.getHeight());
		
		
		myCubeMap.endDrawingInto2D();
	}
	
	if( bAnimate ){
		animationTime += ofGetLastFrameTime();
		if( animationTime >= 1.0 ){
			animationTime = 0.0;
		}
	    model.setNormalizedTime(animationTime);
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
	switch (key) {
		case '0':
			model.loadModel("body/respitory_body.obj");
			model.setRotation(1, 90, 1, 0, 0);
			break;
        case '1':
            model.loadModel("astroBoy_walk.dae");
            ofEnableSeparateSpecularLight();
			model.setRotation(1, 180, 1, 0, 0);
            break;
        case '2':
            model.loadModel("TurbochiFromXSI.dae");
            model.setRotation(0,90,1,0,0);
//            ofEnableSeparateSpecularLight();
            break;
        case '3':
            model.loadModel("dwarf.x");
            ofDisableSeparateSpecularLight();
            break;
        case '4':
            model.loadModel("monster-animated-character-X.X");
            ofDisableSeparateSpecularLight();
            break;
		case '5':
			model.loadModel("squirrel/NewSquirrel.3ds");
//            model.setRotation(0,-90,1,0,0);
            ofDisableSeparateSpecularLight();
			break;
		case ' ':
			bAnimate = !bAnimate;
			break;
        default:
            break;
    }
	
	model.setPosition(0, -(float)ofGetHeight() * 0.75 , 0);
	
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
