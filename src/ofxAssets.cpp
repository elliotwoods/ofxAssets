//
//  ofxAssets.cpp
//

#include "ofxAssets.h"

namespace ofxAssets {
	//----------
	Register & AssetRegister() {
		return Register::X();
	}

	//----------
	ofTrueTypeFont & font(const string & name, int size) {
		return Register::X().getFont(name, size);
	}

	//----------
	ofImage & image(const string & name) {
		return Register::X().getImage(name);
	}

	//----------
	ofShader & shader(const string & name) {
		return Register::X().getShader(name);
	}
	
#ifndef TARGET_NO_SOUND
	//----------
	ofSoundPlayer & sound(const string & name) {
		return Register::X().getSound(name);
	}
#endif
	
	//----------
	bool hasFont(const string & name) {
		return Register::X().getFonts().has(name);
	}
	
	//----------
	bool hasFont(const string & name, int size) {
		const auto & fonts = Register::X().getFonts();
		auto findFont = fonts.find(name);
		if (findFont == fonts.end()) {
			return false;
		} else {
			const auto & sizes = findFont->second->getSizes();
			if(find(sizes.begin(), sizes.end(), size) != sizes.end()) {
				return true;
			} else {
				return false;
			}
		}
	}
	
	//----------
	bool hasImage(const string & name) {
		return Register::X().getImages().has(name);
	}
	
	//----------
	bool hasShader(const string & name) {
		return Register::X().getShaders().has(name);
	}
	
#ifndef TARGET_NO_SOUND
	//----------
	bool hasSound(const string & name) {
		return Register::X().getSounds().has(name);
	}
#endif
}