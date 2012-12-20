#include "testApp.h"
int size = 255;
bool bDebug = false;
ofPoint pos[6]={ofPoint(size*2,size),
	ofPoint(0,size),
	ofPoint(size,0),
	ofPoint(size,size*2),
	ofPoint(size,size),
	ofPoint(size*3,size)};
#define TEST_WITH_VIDEO
//--------------------------------------------------------------
void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	glEnable(GL_DEPTH_TEST);
	model.loadModel("koala.obj");
	//	model.loadModel("NewSquirrel.3ds");
	model.setRotation(1, 180,0, 0, 1);
	myVideo.loadMovie("myVideo.mov");
	myVideo.play();
	myVideo.setLoopState(OF_LOOP_NORMAL);
#ifdef USE_CUBEMAP
	cubeMapShader.load("CubeMap");
#ifdef TEST_WITH_VIDEO
	myCubeMap.initEmptyTextures(size);
#else
	myCubeMap.loadImages("skybox/positive_x.png", "skybox/negative_x.png", "skybox/positive_y.png", "skybox/negative_y.png", "skybox/positive_z.png", "skybox/negative_z.png");
#endif
	for( int i = 0; i < 6; i++ )
	{
		myCubeMap.beginDrawingInto2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i );
		
        ofClear(0,0,0);
		myVideo.getTextureReference().drawSubsection(0, 0, size, size, pos[i].x, pos[i].y, size, size);
		myCubeMap.endDrawingInto2D();
	}
#else
	ofDisableArbTex();
    ofEnableNormalizedTexCoords();
	texMapShader.load("shaders/displace");
//	colormap.loadImage("mars_1k_color.jpg");
//	bumpmap.loadImage("mars_1k_topo.jpg");
//	quadratic = gluNewQuadric();
//	gluQuadricTexture(quadratic, GL_TRUE);
//	gluQuadricNormals(quadratic, GLU_SMOOTH);
	sampler2dTex.allocate(512,512);
#endif
	bAnimate = false;
	animationTime	= 0.0;
	myVideo.update();
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	myVideo.update();
	
	#ifdef USE_CUBEMAP
	#ifdef TEST_WITH_VIDEO
	for( int i = 0; i < 6; i++ )
	{
		myCubeMap.beginDrawingInto2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i );
		
        ofClear(0,0,0);
		myVideo.getTextureReference().drawSubsection(0, 0, size, size, pos[i].x, pos[i].y, size, size);
		//		myVideo.draw(0, 0, myCubeMap.getWidth(), myCubeMap.getHeight());
		
		
		myCubeMap.endDrawingInto2D();
	}
#else
#endif
#else
	sampler2dTex.begin();
	ofClear(0);
	myVideo.draw(0, 0,sampler2dTex.getWidth(),sampler2dTex.getHeight());
	 sampler2dTex.end();
#endif
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
#ifndef USE_CUBEMAP
//	sampler2dTex.draw(0,0);
#endif
	if(bDebug)
	{
	glDisable(GL_DEPTH_TEST);
	ofPushStyle();
	ofSetColor(255);
	myVideo.draw(0, 0);
	ofSetColor(200);
	for( int i = 0; i < 6; i++ )
	{
		myVideo.getTextureReference().drawSubsection(pos[i].x, pos[i].y, size, size, pos[i].x, pos[i].y, size, size);
	}
	ofPopStyle();
	}
	glEnable(GL_DEPTH_TEST);

	cam.begin();
	
#ifdef USE_CUBEMAP
	myCubeMap.bind();
	
    cubeMapShader.begin();
    cubeMapShader.setUniform1i("EnvMap", 0);
	ofPushMatrix();
//	ofRotate(ofGetFrameNum(), 0, 1, 0);
//	myCubeMap.drawSkybox( 800 );
	model.drawFaces();
	ofPopMatrix();
    cubeMapShader.end();
	
	myCubeMap.unbind();
#else

	texMapShader.begin();
	texMapShader.setUniformTexture("colormap",sampler2dTex , 1);
//	texMapShader.setUniformTexture("bumpmap", bumpmap, 2);
//	texMapShader.setUniform1i("maxHeight",mouseX);
	ofPushMatrix();
//	ofRotate(ofGetFrameNum(), 0, 1, 0);
	model.drawFaces();
	ofPopMatrix();
//	myCubeMap.drawSkybox( 400 );
//	gluSphere(quadratic, 150, 400, 400);
	texMapShader.end();
#endif
	
	
	
	
	cam.end();
	
	
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key) {
		case '0':
			model.loadModel("body.obj");
			break;
        case '1':
            model.loadModel("koala.obj");
            ofEnableSeparateSpecularLight();
			model.setRotation(1, 180, 1, 0, 0);
            break;
        case '2':
            model.loadModel("TurbochiFromXSI.dae");
//            model.setRotation(0,90,1,0,0);
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
		case '6':
            model.loadModel("astroBoy_walk.dae");
            ofEnableSeparateSpecularLight();
			model.setRotation(1, 180, 1, 0, 0);
			model.setPosition(0, -(float)ofGetHeight() * 0.5 , 0);
            break;
		case ' ':
			bAnimate = !bAnimate;
			break;
        default:
            break;
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
