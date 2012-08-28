//
//  ofxAssets.cpp
//

#include "ofxAssets.h"

namespace ofxAssets {
	//---------
	Register AssetRegister = Register();
	
	//---------
	Register::Register() {
		ofAddListener(ofEvents().setup, this, & Register::setup);
	}
	
	//---------
	void Register::setup(ofEventArgs &) {
		this->init();
	}
	
	//---------
	void Register::init() {
		ofLogNotice("ofxAssets") << "//--------------------";
		ofLogNotice("ofxAssets") << "//ofxAssets::init";
		ofLogNotice("ofxAssets") << "//--------------------";
		ofLogNotice("ofxAssets") << "//";
		
		string filename, name, extension, withoutExtension, folder;
		ofDirectory files;
		
		string dataPath = "assets";
		if (!ofDirectory::doesDirectoryExist(dataPath))
		{
			ofLogError("ofxAssets") << "Assets data path cannot be found. Be sure to have a ./assets subfolder inside your app's data/ folder if you want to use ofxAssets";
			return;
		}
		
		////
		//images
		////
		//
		folder = dataPath + "/images/";
		files.listDir(folder);
		for (int i=0; i<files.size(); i++) {
			filename = files.getPath(i);
			extension = ofFilePath::getFileExt(filename);
			withoutExtension = filename.substr(0, filename.length() - extension.length() - 1);
			name = ofFilePath::getBaseName(filename);
			
			if (!(extension == "png" || extension == "jpeg" || extension == "jpg"))
				continue;
			
			if (this->images.count(name) > 0)
				continue;
			
			this->images.insert(pair<string, ofImage>(name, ofImage()));
			
			this->images[name].loadImage(filename);
			
			ofLogNotice("ofxAssets") << "Loaded image asset '" << name << "'" << endl;
		}
		//
		////
		
		
		////
		//shaders
		////
		//
		folder = ofToDataPath(dataPath + "/shaders/", true);
		bool hasFrag, hasVert;
		files.listDir(folder);
		for (int i=0; i<files.size(); i++) {
			filename = files.getPath(i);
			extension = ofFilePath::getFileExt(filename);
			withoutExtension = filename.substr(0, filename.length() - extension.length() - 1);
			name = ofFilePath::getBaseName(filename);
			
			if (!(extension == "vert" || extension == "frag" || extension == "geom"))
				continue;
			
			if (this->shaders.count(name) > 0)
				continue;
			
			this->shaders.insert(pair<string, ofShader>(name, ofShader()));
			
			if (ofFile::doesFileExist(withoutExtension + ".frag"))
				this->shaders[name].setupShaderFromFile(GL_FRAGMENT_SHADER, withoutExtension + ".frag");
			if (ofFile::doesFileExist(withoutExtension + ".vert"))
				this->shaders[name].setupShaderFromFile(GL_VERTEX_SHADER, withoutExtension + ".vert");
			if (ofFile::doesFileExist(withoutExtension + ".geom"))
				this->shaders[name].setupShaderFromFile(GL_GEOMETRY_SHADER, withoutExtension + ".geom");
			this->shaders[name].linkProgram();
							
			ofLogNotice("ofxAssets") << "Loaded shader asset '" << name << "'" << endl;
		}
		//
		////
		
		
		////
		//fonts
		////
		//
		folder = ofToDataPath(dataPath + "/fonts/", true);
		files.listDir(folder);
		for (int i=0; i<files.size(); i++) {
			filename = files.getPath(i);
			extension = ofFilePath::getFileExt(filename);
			withoutExtension = filename.substr(0, filename.length() - extension.length() - 1);
			name = ofFilePath::getBaseName(filename);
			
			if (!(extension == "ttf"))
				continue;
			
			if (this->fontFilenames.count(name) > 0)
				continue;
			
			this->fontFilenames.insert(pair<string, string>(name, filename));
			
			ofLogNotice("ofxAssets") << "Found font asset '" << name << "'" << endl;
		}
		//
		////

		
		ofLogNotice("ofxAssets") << "//";
		ofLogNotice("ofxAssets") << "//--------------------";
		
		ofNotifyEvent(evtLoad, *this, this);
	}
	
	//---------
	void Register::refresh() {
		this->shaders.clear();
		this->images.clear();
		this->fonts.clear();
		this->init();
	}

	//---------
	ofShader & Register::getShader(string name) {
		if (this->shaders.count(name) != 0)
			return this->shaders[name];
		else {
			ofLogError("ofxAssets") << "Requested shader asset'" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return  this->blankShader;
		}
	}
	
	//---------
	ofImage & Register::getImage(string name) {
		if (this->images.count(name) != 0)
			return this->images[name];
		else {
			ofLogError("ofxAssets") << "Requested image asset'" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return  this->blankImage;
		}
	}

	//---------
	ofTrueTypeFont & Register::getFont(string name, int size) {
		pair<string, int> id = pair<string, int>(name, size);
		if (this->fonts.count(id) > 0) {
			return this->fonts[id];
		} else if (this->fontFilenames.count(name) > 0) {
			this->fonts.insert(pair<pair<string,int>,ofTrueTypeFont>(id, ofTrueTypeFont()));
			this->fonts[id].loadFont(this->fontFilenames[name], size);
			ofLogNotice("ofxAssets") << "Loaded font asset '" << name << "' (" << size << ")" << endl;
		} else {
			ofLogError("ofxAssets") << "Requested font asset'" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return  this->blankFont;
		}
	}
}