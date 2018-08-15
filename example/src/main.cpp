#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ) {
	ofGLFWWindowSettings settings;
	settings.setGLVersion(4, 1);
	settings.setSize(1280, 720);
	//settings.resizable = false;
	ofCreateWindow(settings);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}
