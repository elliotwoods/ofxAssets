//
//  ofxAssets.h
//
#pragma once

#include "ofxAssets/Register.h"

namespace ofxAssets {
	Register & AssetRegister();
	
	ofTrueTypeFont & font(const string & name, int size);
	ofImage & image(const string & name);
	ofShader & shader(const string & name);
	

#ifndef TARGET_NO_SOUND
	ofSoundPlayer & sound(const string & name);
#endif
	
	bool hasFont(const string & name);
	bool hasFont(const string & name, int size);
	bool hasImage(const string & name);
	bool hasShader(const string & name);
	
#ifndef TARGET_NO_SOUND
	bool hasSound(const string & name);
#endif
}