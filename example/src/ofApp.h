#pragma once

#include "ofMain.h"
#include "ofxBloom.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);

	ofxPanel gui;
	ofxFloatSlider scale;
	ofxFloatSlider thresh;
	ofxFloatSlider brightness;

	float time;
	std::shared_ptr<ofxBloom> bloom;
	ofFbo fbo;
};
