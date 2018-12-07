#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//Src fbo's texcoord has to be normalized(0 - 1)
	ofDisableArbTex();
	fbo.allocate(ofGetWidth(), ofGetHeight());
	ofEnableArbTex();

	bloom = std::make_shared<ofxBloom>(ofGetWidth(), ofGetHeight(), &fbo);

	gui.setup();
	gui.add(scale.setup("Scale", 2.3f, 0.1f, 16.f));
	gui.add(brightness.setup("Brightness", 5.0f, 0.f, 30.f));
	gui.add(thresh.setup("Threshold", 0.f, 0.1f, 2.f));
}

//--------------------------------------------------------------
void ofApp::update() {
	time = ofGetElapsedTimef();

	fbo.begin();
	ofClear(0);
	float hue = 0;
	for (int i = 0; i < 180; i += 30) {
		ofSetColor(ofColor::fromHsb(hue, 360, 360));
		ofDrawCircle(
			1280 * 0.5 + 100 * sin(6 * time + PI + (DEG_TO_RAD * i)),
			720  * 0.5 + 100 * cos(8 * time + (DEG_TO_RAD * i)),
			10,
			10);
		hue += 30;
	}
	fbo.end();

	bloom->setBrightness(brightness);
	bloom->setScale(scale);
	bloom->setThreshold(thresh);
	bloom->process();
}

//--------------------------------------------------------------
void ofApp::draw() {
	bloom->draw();
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}