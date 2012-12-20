#include "testApp.h"
#define NUM_LED 25
#define NUM_PEGGY 2
string peggy1Serial = "/dev/tty.usbserial-AH0183W2";
string peggy2Serial = "/dev/tty.usbserial-A7004E4F";

unsigned char peggyHeader[6] = {0xde,0xad,0xbe,0xef,1,0};
unsigned char *peggyFrame;
ofShortPixels scaledPixels;
ofFbo fbo;
ofSerial port1,port2;
ofTexture tex;

//Ball bouncing -- can be removed later
int size = 60;       // Width of the shape
float xpos, ypos;    // Starting position of shape
float xspeed = 5.8;  // Speed of the shape
float yspeed = 5.2;  // Speed of the shape
int xdirection = 1;  // Left or Right
int ydirection = 1;  // Top to Bottom
bool bSetial = false;;
void renderToPeggy( int display)
{
    int idx = 0;
    
    // iterate over the image, pull out pixels and
    // build an array to serialize to the peggy
    int start_y = (display==0)?0:NUM_LED;
    int end_y = (display==0)?NUM_LED:NUM_LED*NUM_PEGGY;
    for (int y =start_y; y < end_y; y++)
    {
        unsigned val = 0;
        for (int x=0; x < NUM_LED; x++)
        {
            ofColor c = scaledPixels.getColor(y, x);
            int br = ((int)c.getBrightness())>>4;
            if (x % 2 ==0)
                val = (unsigned char)br;
            else
            {
                val = (unsigned char) ((br<<4)|val);
                peggyFrame[idx++]= val;
            }
        }
        peggyFrame[idx++]= val;  // write that one last leftover half-byte
    }
    if(bSetial)
    {
        // send the header, followed by the frame
        if (display == 0){
            port1.writeBytes(peggyHeader,6);
            port1.writeBytes(peggyFrame,13*NUM_LED);
        }else{
            port2.writeBytes(peggyHeader,6);
            port2.writeBytes(peggyFrame,13*NUM_LED);
        }
    }
    
}
//--------------------------------------------------------------
void testApp::setup(){
    peggyFrame = new unsigned char[13*25];
    port1.setup(peggy1Serial, 115200);
    port2.setup(peggy2Serial, 115200);
    scaledPixels.allocate(NUM_LED,NUM_LED*2,OF_IMAGE_COLOR);
    fbo.allocate(NUM_LED,NUM_LED*2);
    tex.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    // Set the starting position of the shape -- can be removed later
    xpos = ofGetWidth()/2;
    ypos = ofGetHeight()/2;
    ofSetFrameRate(30);
}

//--------------------------------------------------------------
void testApp::update(){
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(0);
    xpos = xpos + ( xspeed * xdirection );
    ypos = ypos + ( yspeed * ydirection );
    if (xpos > ofGetWidth()-size || xpos < 0) xdirection *= -1;
    if (ypos > ofGetHeight()-size || ypos < 0) ydirection *= -1;
    ofCircle(xpos+size/2, ypos+size/2, size, size);
    tex.loadScreenData(0, 0, ofGetWidth(), ofGetHeight());
    
    fbo.begin();
    tex.draw(0, 0, NUM_LED, NUM_LED*2);
    fbo.readToPixels(scaledPixels);
    fbo.end();
    fbo.draw(0,0,125,255);
    renderToPeggy(0);
    renderToPeggy(1);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == ' ')
    {
        bSetial = !bSetial;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    
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