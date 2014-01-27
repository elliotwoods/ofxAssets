//
//  ofxAssets.h
//
#pragma once

#include "ofShader.h"
#include "ofImage.h"
#include "ofTrueTypeFont.h"

#include "ofEvents.h"

#include <map>
#include <set>
#include <string>

namespace ofxAssets {
	class Register {
	public:
		Register();
		void refresh();
		
		ofImage & getImage(string name);
		ofShader & getShader(string name);
		ofTrueTypeFont & getFont(string name, int size);
		
		/// Load assets for an addon, e.g.:
		///		data/ofxMultiTrack/images/0.png
		/// becomes
		///		image("ofxMultiTrack::0")
		void addAddon(string addonName);
		
		ofEvent<Register> evtLoad;

	protected:
		void setup(ofEventArgs &);
		void loadAssets(string addon = "");

		set<string> addonList;

		map<string, ofImage> images;
		map<string, ofShader> shaders;
		map<string, string> fontFilenames;
		map<pair<string,int>, ofTrueTypeFont> fonts;
		
		ofImage blankImage;
		ofShader blankShader;
		ofTrueTypeFont blankFont;
	};

	extern Register AssetRegister;
	
	static ofShader & shader(string name) {
		return AssetRegister.getShader(name);
	}
	
	static ofImage & image(string name) {
		return AssetRegister.getImage(name);
	}
	
	static ofTrueTypeFont & font(string name, int size) {
		return AssetRegister.getFont(name, size);
	}
}