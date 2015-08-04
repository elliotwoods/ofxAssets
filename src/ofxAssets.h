//
//  ofxAssets.h
//
#pragma once

#ifdef HAS_OFXSINGLETON
#include "ofxSingleton.h"
#endif

#include "ofShader.h"
#include "ofImage.h"
#include "ofTrueTypeFont.h"

#include "ofEvents.h"

#include <map>
#include <set>
#include <string>
#include <regex>

namespace ofxAssets {

//setup singleton in class header
#ifdef HAS_OFXSINGLETON
	class Register : public ofxSingleton::Singleton<Register> {
#else
	class Register {
	public:
		static Register & X(); // get the singleton
	protected:
		static Register * singleton;
#endif


	public:
		Register();
		void refresh();
		void clear();

		ofImage & getImage(string name);
		ofShader & getShader(string name);
		ofTrueTypeFont & getFont(string name, int size);
		
		shared_ptr<ofImage> getImagePointer(string name);
		shared_ptr<ofShader> getShaderPointer(string name);
		shared_ptr<ofTrueTypeFont> getFontPointer(string name, int size);

		bool hasImage(string name);
		bool hasShader(string name);
		bool hasFont(string name);

		/// Load assets for an addon, e.g.:
		///		data/ofxMultiTrack/images/0.png
		/// becomes
		///		image("ofxMultiTrack::0")
		void addAddon(string addonName);
		
		ofEvent<Register> evtLoad;

	protected:
		bool isInitialised() const;
		void load(); // make sure everything is loaded
		void unload(); // unload everything
		
		void traverseDirectoryImages(boost::filesystem::path path, vector<string> targetNamespace);
		void traverseDirectoryShaders(boost::filesystem::path path, vector<string> targetNamespace);
		void traverseDirectoryFonts(boost::filesystem::path path, vector<string> targetNamespace);

		set<string> addonsRegistered;
		set<string> addonsLoaded;

		map<string, shared_ptr<ofImage>> images;
		map<string, shared_ptr<ofShader>> shaders;
		map<string, string> fontFilenames;
		map<pair<string,int>, shared_ptr<ofTrueTypeFont>> fonts;
		
		shared_ptr<ofImage> blankImage;
		shared_ptr<ofShader> blankShader;
		shared_ptr<ofTrueTypeFont> blankFont;

		bool initialised;
	};

	Register & AssetRegister();// for (close to) backwards compatability

	static ofShader & shader(string name) {
		return Register::X().getShader(name);
	}
	
	static ofImage & image(string name) {
		return Register::X().getImage(name);
	}
	
	static ofTrueTypeFont & font(string name, int size) {
		return Register::X().getFont(name, size);
	}
}