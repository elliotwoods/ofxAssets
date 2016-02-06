#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	ofGLFWWindowSettings windowSettings;

	windowSettings.setGLVersion(3, 2);
	windowSettings.width = 1024;
	windowSettings.height = 768;
	auto window = ofCreateWindow(windowSettings);

	ofRunApp(new ofApp());

}
