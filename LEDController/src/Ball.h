#pragma once

#include "ofMain.h"
#define NUM_TAIL 5
class Ball {
    
public:

    Ball(int _x, int _y, float _mWidth, float _mHeight , float vx, float vy) {
        x = _x;
        y = _y;
        xDir = vx;
        yDir = vy;
        d = 1;
        mWidth  = _mWidth;
        mHeight = _mHeight;
        len = NUM_TAIL;
        bBounce = false;

    }
    
    //--------------------------------------
    // Moves and changes direction if it hits a wall.
    void calc() {
        if(bBounce)
        {
        if (x < 0 || x > mWidth)  xDir *= -1;
        if (y < 0 || y > mHeight) yDir *= -1;
        }
        else
        {
            (x < -1 )? x = mWidth: (x > mWidth+1)?x=0:x;
            (y < -1 )? x = mHeight: (y > mHeight+1)?y=0:y;
        }
        x += xDir;
        y += yDir;
        
        for (int i = 1; i < len; i++)
        {
           tail[i-1] = tail[i];
        }
        
        tail[len-1].set(x, y);
    }
    
    //--------------------------------------
    void draw() {
        ofPushStyle();
        ofEnableAlphaBlending();
        
        for(int i = 0 ; i <  len; i++)
        {
            float div = i/(len*1.0f);
            float r = d;
            ofPushStyle();
            ofEnableAlphaBlending();
            ofSetColor(255,255*div);
            ofFill();
            ofRect(tail[i],r,r);
            ofPopStyle();

        }
        ofPopStyle();
    }
    
private:
    float x;
    float y;
    float xDir;
    float yDir;
    float d;
    float mWidth;
    float mHeight;
    bool bBounce;
    int len;
    ofPoint tail[NUM_TAIL];
    
};