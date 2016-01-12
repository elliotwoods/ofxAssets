//
//  ofxAssets.cpp
//

#include "ofxAssets.h"

#ifdef HAS_OFXAUDIODECODER
#include "ofxAudioDecoder.h"
#endif

namespace fs = std::filesystem;

//define statics
#ifdef HAS_OFXSINGLETON
	//---------
	OFXSINGLETON_DEFINE(ofxAssets::Register);
#else
	//---------
	ofxAssets::Register * ofxAssets::Register::singleton = 0;

	//---------
	ofxAssets::Register & ofxAssets::Register::X() {
		if (!Register::singleton) {
			Register::singleton = new Register();
		}
		return *Register::singleton;
	}
#endif

namespace ofxAssets {
#pragma mark public
	//---------
	Register::Register() {
		this->blankFont = make_shared<ofTrueTypeFont>();
		this->blankImage = make_shared<ofImage>();
		this->blankShader = make_shared<ofShader>();
		this->blankSound = make_shared<Sound>();
		
		this->blankImage->allocate(1, 1, OF_IMAGE_GRAYSCALE);
		this->blankImage->getPixels()[0] = 255;
		this->blankImage->update();
		
		this->clear();
	}

	//---------
	void Register::refresh() {
		this->unload();
		this->load();
	}

	//---------
	void Register::clear() {
		this->unload();
		this->addonsRegistered.clear();
		this->addonsRegistered.insert(""); // add the root namespace
	}

	//---------
	ofImage & Register::getImage(string name) {
		return * this->getImagePointer(name);
	}

	//---------
	ofShader & Register::getShader(string name) {
		return * this->getShaderPointer(name);
	}

	//---------
	ofTrueTypeFont & Register::getFont(string name, int size) {
		return * this->getFontPointer(name, size);
	}
	
	//---------
	Register::Sound & Register::getSound(string name) {
		return * this->getSoundPointer(name);
	}
	
	//---------
	shared_ptr<ofImage> Register::getImagePointer(string name) {
		this->load();

		auto findImage = this->images.find(name);
		if (findImage != this->images.end()) {
			return findImage->second;
		} else {
			ofLogError("ofxAssets") << "Requested image asset '" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return this->blankImage;
		}
	}

	//---------
	shared_ptr<ofShader> Register::getShaderPointer(string name) {
		this->load();

		auto findShader = this->shaders.find(name);
		if (findShader != this->shaders.end()) {
			return findShader->second;
		} else {
			ofLogError("ofxAssets") << "Requested shader asset '" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return this->blankShader;
		}
	}

	//---------
	shared_ptr<ofTrueTypeFont> Register::getFontPointer(string name, int size) {
		this->load();
		
		pair<string, int> id = pair<string, int>(name, size);
		auto findFont = this->fonts.find(id);
		if (findFont != this->fonts.end()) {
			//this font+size exists
			return findFont->second;
		} else if (this->fontFilenames.find(name) != this->fontFilenames.end()) {
			//font fontexists, but we need to load this size
			auto font = make_shared<ofTrueTypeFont>();
			this->fonts.insert(pair<pair<string,int>,shared_ptr<ofTrueTypeFont>>(id, font));
			font->load(this->fontFilenames[name], size, true, false, true);
			ofLogVerbose("ofxAssets") << "Loaded font asset '" << name << "' (" << size << ")" << endl;
			return font;
		} else {
			//font doesn't exist
			ofLogError("ofxAssets") << "Requested font asset '" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return this->blankFont;
		}
	}
	
	//---------
	shared_ptr<Register::Sound> Register::getSoundPointer(string name) {
		this->load();
		
		auto findSound = this->sounds.find(name);
		if (findSound != this->sounds.end()) {
			return findSound->second;
		} else {
			ofLogError("ofxAssets") << "Requested sound asset '" << name << "' doesn't exist, have you got all the files in the right place in your data/assets/ folder?";
			return this->blankSound;
		}
	}

	//---------
	bool Register::hasImage(string name) const {
		return this->images.find(name) != this->images.end();
	}

	//---------
	bool Register::hasShader(string name) const {
		return this->shaders.find(name) != this->shaders.end();
	}

	//---------
	bool Register::hasFont(string name) const {
		return this->fontFilenames.find(name) != this->fontFilenames.end();
	}
	
	//---------
	bool Register::hasSound(string name) const {
		return this->sounds.find(name) != this->sounds.end();
	}

	//---------
	//boost's version of copy doesn't support recursive, so we take this solution from http://stackoverflow.com/questions/8593608/how-can-i-copy-a-directory-using-boost-filesystem
	bool copyDir(fs::path const & source, fs::path const & destination) {
		try
		{
			// Check whether the function call is valid
			if (
				!fs::exists(source) ||
				!fs::is_directory(source)
				)
			{
				std::cerr << "Source directory " << source.string()
					<< " does not exist or is not a directory." << '\n'
					;
				return false;
			}
			if (fs::exists(destination))
			{
				std::cerr << "Destination directory " << destination.string()
					<< " already exists." << '\n'
					;
				return false;
			}
			// Create the destination directory
			if (!fs::create_directories(destination))
			{
				std::cerr << "Unable to create destination directory"
					<< destination.string() << '\n'
					;
				return false;
			}
		}
		catch (fs::filesystem_error const & e)
		{
			std::cerr << e.what() << '\n';
			return false;
		}
		// Iterate through the source directory
		for (
			fs::directory_iterator file(source);
			file != fs::directory_iterator(); ++file
			)
		{
			try
			{
				fs::path current(file->path());
				if (fs::is_directory(current))
				{
					// Found directory: Recursion
					if (
						!copyDir(
							current,
							destination / current.filename()
							)
						)
					{
						return false;
					}
				}
				else
				{
					// Found file: Copy
					fs::copy_file(
						current,
						destination / current.filename()
						);
				}
			}
			catch (fs::filesystem_error const & e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		return true;
	}

	//---------
	void Register::addAddon(string addonName) {
		if (this->addonsRegistered.find(addonName) != this->addonsRegistered.end()) {
			//addon is already registered
			return;
		}

		this->addonsRegistered.insert(addonName);
		this->initialised = false;

		//whilst we're in debug build mode, we'll actually copy over the assets from the addon's folder
#if defined(__DEBUGGING__) || defined(_DEBUG)
		//if we're still debugging in the build location, copy in latest assets
		auto addonAssetsSource = fs::path(ofToDataPath("../../../../../addons/" + addonName + "/data/assets/" + addonName + "/"));
		auto addonAssetsDestination = fs::path(ofToDataPath("assets/" + addonName));
		if (fs::exists(addonAssetsSource)) {
			ofLogVerbose("ofxAssets") << "Copying in addon files from " << addonAssetsSource;
			fs::remove_all(addonAssetsDestination);
			copyDir(addonAssetsSource, addonAssetsDestination);
		} else {
			ofLogWarning("ofxAssets") << "Cannot copy in addon assets since folder doesn't exist : " << addonAssetsSource;
		}
#endif
	}

	//---------
	vector<string> Register::getImageNames() const {
		vector<string> result;
		for(const auto & image : this->images) {
			result.push_back(image.first);
		}
		return result;
	}
	
	//---------
	vector<string> Register::getShaderNames() const {
		vector<string> result;
		for(const auto & shader : this->shaders) {
			result.push_back(shader.first);
		}
		return result;
	}
	
	//---------
	vector<string> Register::getSoundNames() const {
		vector<string> result;
		for(const auto & sound : this->sounds) {
			result.push_back(sound.first);
		}
		return result;
	}
	
	//---------
	vector<string> Register::getFontFilenames() const {
		vector<string> result;
		for(const auto & fontFilename : this->fontFilenames) {
			result.push_back(fontFilename.first);
		}
		return result;
	}
	
	//---------
	vector<int> Register::getFontSizes(string fontName) const {
		vector<int> result;
		for(const auto & font : this->fonts) {
			if(font.first.first == fontName) {
				result.push_back(font.first.second);
			}
		}
		return result;
	}
	
#pragma mark protected
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
	bool Register::isInitialised() const {
		return this->initialised; // a cached flag for speed since we call here often
	}

	//---------
	void Register::load() {
		// This gets called often to check. Let's do a basic check before continuing
		if (this->isInitialised()) {
			return;
		}

		auto dataPath = fs::path(ofToDataPath("")) / "assets";

		for (const auto addon : this->addonsRegistered) {
			ofLogVerbose("ofxAssets") << "ofxAssets : loading addon '" << addon << "'";

			if (this->addonsLoaded.find(addon) != this->addonsLoaded.end()) {
				//we've already loaded this addon. call refresh() if you want to load it again
				continue;
			}
			if (!addon.empty()) {
				dataPath /= addon;
			}

			if (!fs::is_directory(dataPath)) {
				ofLogWarning("ofxAssets") << "Assets data path cannot be found. Be sure to have a ./assets subfolder inside your app's data/ folder if you want to use ofxAssets";
				return;
			}

			vector<string> outputNamespace;
			if (!addon.empty()) {
				outputNamespace.push_back(addon);
			}

			this->traverseDirectoryImages(dataPath / "images", outputNamespace);
			this->traverseDirectoryShaders(dataPath / "shaders", outputNamespace);
			this->traverseDirectoryFonts(dataPath / "fonts", outputNamespace);
			this->traverseDirectorySounds(dataPath / "sounds", outputNamespace);

			this->addonsLoaded.insert(addon);
		}
		
		this->initialised = true;
		ofNotifyEvent(evtLoad, this);

		ofLogVerbose("ofxAssets") << "";
	}

	//----------
	void Register::unload() {
		this->images.clear();
		this->shaders.clear();
		this->fontFilenames.clear();
		this->fonts.clear();
		this->sounds.clear();

		this->initialised = false;
		this->addonsLoaded.clear();
	}
	//----------
	void traverseDirectory(fs::path path, regex mask, const vector<string> & targetNamespace, function<void(const fs::path & path, string assetAddress)> loadFunction) {
		if (fs::is_directory(path)) {
			for (const auto & entry : fs::directory_iterator(path)) {
				const auto & path = entry.path();

				if (fs::is_directory(path)) {
					//it's a subfolder if so traverse it
					auto innerNamespace = targetNamespace;
					innerNamespace.push_back(path.filename().string());
					traverseDirectory(path, mask, innerNamespace, loadFunction);
				}
				else {
					auto extension = ofToLower(path.extension().string());
					if (regex_match(extension, mask)) {
						//matches the extension mask
						string assetAddress = path.stem().string();
						transformName(assetAddress, targetNamespace);
						if (loadFunction) {
							loadFunction(path, assetAddress);
						}
					}
				}
			}
		}
	}

	//----------
	void Register::traverseDirectoryImages(fs::path path, vector<string> targetNamespace) {
		traverseDirectory(path, regex("^.*\\.(jpg|jpeg|png|gif|tif|tiff|gif)$"), targetNamespace, [this](const fs::path & path, string assetAddress) {
			if (!this->hasImage(assetAddress)) {
				auto image = make_shared<ofImage>();
				this->images.insert(pair<string, shared_ptr<ofImage>>(assetAddress, image));
				image->load(path.string());
				ofLogVerbose("ofxAssets") << "Loaded image asset '" << assetAddress << "'" << endl;
			}
		});
	}


	//----------
	void Register::traverseDirectoryShaders(fs::path path, vector<string> targetNamespace) {
		traverseDirectory(path, regex("^.*\\.(vert|frag|geom)$"), targetNamespace, [this](const fs::path & path, string assetAddress) {
			if (!this->hasShader(assetAddress)) {
				//insert the shader
				auto shader = make_shared<ofShader>();
				this->shaders.insert(pair<string, shared_ptr<ofShader>>(assetAddress, shader));

				//load any available shader stages
				const auto withoutExtension = path.parent_path() / path.stem();
				const auto absolutePath = fs::absolute(withoutExtension);

				auto success = shader->load(absolutePath.string());

				if (success) {
					ofLogVerbose("ofxAssets") << "Loaded shader asset '" << assetAddress << "'" << endl;
				}
				else {
					ofLogError("ofxAssets") << "Failed to load shader asset '" << assetAddress << "'" << endl;
				}
			}
		});
	}

	//----------
	void Register::traverseDirectoryFonts(fs::path path, vector<string> targetNamespace) {
		traverseDirectory(path, regex("^.*\\.(ttf)$"), targetNamespace, [this](const fs::path & path, string assetAddress) {
			if (!this->hasFont(assetAddress)) {
				this->fontFilenames.insert(pair<string, string>(assetAddress, path.string()));
				ofLogVerbose("ofxAssets") << "Found font asset '" << assetAddress << "'" << endl;
			}
		});
	}
	
	//----------
	void Register::traverseDirectorySounds(fs::path path, vector<string> targetNamespace) {
		traverseDirectory(path, regex("^.*\\.(wav|aif|aiff|mp3)$"), targetNamespace, [this](const fs::path & path, string assetAddress) {
			if (!this->hasSound(assetAddress)) {
				auto sound = make_shared<Register::Sound>();
				this->sounds.insert(pair<string, shared_ptr<Sound>>(assetAddress, sound));
				sound->player.load(path.string());
#ifdef HAS_OFXAUDIODECODER
				ofxAudioDecoder::load(sound->buffer, path.string());
#endif
				ofLogVerbose("ofxAssets") << "Loaded sound asset '" << assetAddress << "'" << endl;
			}
		});
	}

	//----------
	Register & AssetRegister() { // backwards compatability
		return Register::X();
	}
}