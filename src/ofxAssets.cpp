//
//  ofxAssets.cpp
//

#include "ofxAssets.h"

namespace ofxAssets {
#pragma mark public
	//---------
	Register AssetRegister = Register();
	
	//---------
	Register::Register() {
		this->initialised = false;
		ofAddListener(ofEvents().setup, this, & Register::setup);
	}

	//---------
	void Register::refresh() {
		this->shaders.clear();
		this->images.clear();
		this->fonts.clear();
		this->loadAssets();
		for(auto addon : this->addonList) {
			this->loadAssets(addon);
		}
	}
	
	//---------
	ofImage & Register::getImage(string name) {
		if (!this->initialised) {
			this->loadAssets();
		}
		auto findImage = this->images.find(name);
		if (findImage != this->images.end()) {
			return findImage->second;
		} else {
			ofLogError("ofxAssets") << "Requested image asset'" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return  this->blankImage;
		}
	}

	//---------
	ofShader & Register::getShader(string name) {
		if (!this->initialised) {
			this->loadAssets();
		}
		auto findShader = this->shaders.find(name);
		if (findShader != this->shaders.end()) {
			return findShader->second;
		} else {
			ofLogError("ofxAssets") << "Requested shader asset'" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return  this->blankShader;
		}
	}

	//---------
	ofTrueTypeFont & Register::getFont(string name, int size) {
		if (!this->initialised) {
			this->loadAssets();
		}
		pair<string, int> id = pair<string, int>(name, size);
		auto findFont = this->fonts.find(id);
		if (findFont != this->fonts.end()) {
			return findFont->second;
		} else if (this->fontFilenames.find(name) != this->fontFilenames.end()) {
			this->fonts.insert(pair<pair<string,int>,ofTrueTypeFont>(id, ofTrueTypeFont()));
			ofTrueTypeFont & font = this->fonts[id];
			font.loadFont(this->fontFilenames[name], size, true, true, true);
			ofLogNotice("ofxAssets") << "Loaded font asset '" << name << "' (" << size << ")" << endl;
			return font;
		} else {
			ofLogError("ofxAssets") << "Requested font asset'" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return this->blankFont;
		}
	}

	//---------
	bool Register::hasImage(string name) {
		return this->images.find(name) != this->images.end();
	}

	//---------
	bool Register::hasShader(string name) {
		return this->shaders.find(name) != this->shaders.end();
	}

	//---------
	bool Register::hasFont(string name) {
		return this->fontFilenames.find(name) != this->fontFilenames.end();
	}

	//---------
	void Register::addAddon(string addonName) {
		this->init();
		
		if (this->addonList.find(addonName) != this->addonList.end()) {
			ofLogWarning("ofxAddons::Register::addAddon") << "Addon [" << addonName << "] has already been added to the ofxAssets::Register.";
			return;
		}
		

		//--
		//COPY FILES
		//--
		//
		//whilst we're in debug build mode, we'll actually copy over the assets from the addon's folder
		//
#if defined(__DEBUGGING__) || defined(_DEBUG)
		//if we're still debugging in the build location, copy in latest assets
		auto checkDir = ofDirectory("../../../../../addons/" + addonName + "/data/assets/" + addonName);
		if (checkDir.exists()) {
			ofLogNotice("ofxAssets") << "Copying in addon files from " << checkDir.getOriginalDirectory();
			
			//clear existing folder
			ofDirectory("assets/" + addonName).remove(true);
			
			//copy in current resources
			checkDir.copyTo("assets/" + addonName, true, true);
		} else {
			ofLogNotice("ofxAssets") << "Cannot copy in addon assets since folder doesn't exist : " << checkDir.getOriginalDirectory();
		}
#endif
		//
		//--
		
		
		//actually perform the load
		this->loadAssets(addonName);
	}

#pragma mark protected
	//---------
	void Register::init() {
		if (!this->initialised) {
			this->loadAssets();
		}
	}
	
	//---------
	void Register::setup(ofEventArgs &) {
		this->init();
	}

	//---------
	void transformName(string &name, vector<string> outputNamespace) {
		string flatNamespace = "";
		for (auto level : outputNamespace) {
			flatNamespace += level + "::";
		}
		name = flatNamespace + name;
	}

	//---------
	void transformName(string &name, string addonName) {
		if (!addonName.empty()) {
			name = addonName + "::" + name;
		}
	}

	//---------
	void Register::loadAssets(string addon) {
		ofLogNotice("ofxAssets") << "//--------------------";
		ofLogNotice("ofxAssets") << "//ofxAssets::init";
		ofLogNotice("ofxAssets") << "//--------------------";
		ofLogNotice("ofxAssets") << "//";
		
		string filename, name, extension, withoutExtension, folder;
		ofDirectory files;
		
		string dataPath = "assets";
		if (!addon.empty()) {
			dataPath += "/" + addon;
		}

		if (!ofDirectory::doesDirectoryExist(dataPath)) {
			ofLogNotice("ofxAssets") << "Assets data path cannot be found. Be sure to have a ./assets subfolder inside your app's data/ folder if you want to use ofxAssets";
			return;
		}
		
		vector<string> outputNamespace;
		if (!addon.empty()) {
			outputNamespace.push_back(addon);
		}

		this->traverseDirectoryImages(ofToDataPath(dataPath + "/images/", true), outputNamespace);
		this->traverseDirectoryShaders(ofToDataPath(dataPath + "/shaders/", true), outputNamespace);
		this->traverseDirectoryFonts(ofToDataPath(dataPath + "/fonts/", true), outputNamespace);
		
		ofLogNotice("ofxAssets") << "//";
		ofLogNotice("ofxAssets") << "//--------------------";
		
		ofNotifyEvent(evtLoad, *this, this);
		
		if(addon == "") {
			this->initialised = true;
		} else {
			//this->addonList.insert(addon);
		}
	}

	//----------
	void Register::traverseDirectoryImages(string dataPath, vector<string> outputNamespace) {
		if (ofDirectory::doesDirectoryExist(dataPath)) {
			ofDirectory files;
			files.listDir(dataPath);
			for (int i = 0; i<files.size(); i++) {
				const auto filename = files.getPath(i);
				auto outputName = ofFilePath::getBaseName(filename); 
				
				//check if it's a subfolder
				if (ofDirectory(filename).isDirectory()) {
					auto innerNamespace = outputNamespace;
					innerNamespace.push_back(outputName);
					this->traverseDirectoryImages(dataPath + "/" + outputName, innerNamespace);
				}

				//if not, check whether it has the right extension
				const auto extension = ofFilePath::getFileExt(filename);
				if (!(extension == "png" || extension == "jpeg" || extension == "jpg")) {
					continue;
				}

				//transform the output name to include namespace
				transformName(outputName, outputNamespace);

				//insert and load the image
				if (this->images.find(outputName) != this->images.end()) {
					continue;
				}
				this->images.insert(pair<string, ofImage>(outputName, ofImage()));
				this->images[outputName].loadImage(filename);

				ofLogNotice("ofxAssets") << "Loaded image asset '" << outputName << "'" << endl;
			}
		}
	}


	//----------
	void Register::traverseDirectoryShaders(string dataPath, vector<string> outputNamespace) {
		if (ofDirectory::doesDirectoryExist(dataPath)) {
			ofDirectory files;
			files.listDir(dataPath);
			for (int i = 0; i<files.size(); i++) {
				const auto filename = files.getPath(i);
				auto outputName = ofFilePath::getBaseName(filename);

				//check if it's a subfolder
				if (ofDirectory(filename).isDirectory()) {
					auto innerNamespace = outputNamespace;
					innerNamespace.push_back(outputName);
					this->traverseDirectoryShaders(dataPath + "/" + outputName, innerNamespace);
				}

				//if not, check whether it has the right extension
				const auto extension = ofFilePath::getFileExt(filename);
				if (!(extension == "vert" || extension == "frag" || extension == "geom")) {
					continue;
				}

				//transform the output name to include namespace
				transformName(outputName, outputNamespace);

				//check if the shader already exists (this often happens when you hit a .vert file and we've already loaded when we hit the .geom)
				if (this->shaders.find(outputName) != this->shaders.end()) {
					continue;
				}

				//insert the shader
				this->shaders.insert(pair<string, ofShader>(outputName, ofShader()));
				auto & shader = this->shaders[outputName];

				//load any available shader stages
				const auto withoutExtension = filename.substr(0, filename.length() - extension.length() - 1);
				if (ofFile::doesFileExist(withoutExtension + ".frag"))
					shader.setupShaderFromFile(GL_FRAGMENT_SHADER, withoutExtension + ".frag");
				if (ofFile::doesFileExist(withoutExtension + ".vert"))
					shader.setupShaderFromFile(GL_VERTEX_SHADER, withoutExtension + ".vert");
#ifndef TARGET_IPHONE_SIMULATOR
				if (ofFile::doesFileExist(withoutExtension + ".geom"))
					shader.setupShaderFromFile(GL_GEOMETRY_SHADER, withoutExtension + ".geom");
#endif
				shader.linkProgram();

				ofLogNotice("ofxAssets") << "Loaded shader asset '" << outputName << "'" << endl;
			}
		}
	}

	//----------
	void Register::traverseDirectoryFonts(string dataPath, vector<string> outputNamespace) {
		if (ofDirectory::doesDirectoryExist(dataPath)) {
			ofDirectory files;
			files.listDir(dataPath);
			for (int i = 0; i<files.size(); i++) {
				const auto filename = files.getPath(i);
				auto outputName = ofFilePath::getBaseName(filename);

				//check if it's a subfolder
				if (ofDirectory(filename).isDirectory()) {
					auto innerNamespace = outputNamespace;
					innerNamespace.push_back(outputName);
					this->traverseDirectoryFonts(dataPath + "/" + outputName, innerNamespace);
				}

				//if not, check whether it has the right extension
				const auto extension = ofFilePath::getFileExt(filename);
				if (!(extension == "ttf")) {
					continue;
				}

				//transform the output name to include namespace
				transformName(outputName, outputNamespace);

				//check it's not already loaded
				if (this->fontFilenames.find(outputName) != this->fontFilenames.end()) {
					continue;
				}

				//register the font for lazy loading later
				this->fontFilenames.insert(pair<string, string>(outputName, filename));

				ofLogNotice("ofxAssets") << "Found font asset '" << outputName << "'" << endl;
			}
		}
	}
}