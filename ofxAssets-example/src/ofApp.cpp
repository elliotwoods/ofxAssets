#include "ofApp.h"

using namespace ofxAssets;

//--------------------------------------------------------------
void ofApp::setup(){
	AssetRegister().setDirectoryWatcherEnabled(true);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	image("FreeUCK").draw(10,10);	
	font("verdana", 30).drawString("Why didn't I think of this before?", 10, 600);

	shader("invert").begin();
	image("FreeUCK").draw(510, 10);
	shader("invert").end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == OF_KEY_F5) {
		AssetRegister().refresh();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}