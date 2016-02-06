#include "Register.h"

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
	ofTrueTypeFont & Register::getFont(string name, int size) {
		return this->getFontPointer(name)->get(size);
	}
	
	//---------
	ofImage & Register::getImage(string name) {
		return this->getImagePointer(name)->get();
	}
	
	//---------
	ofShader & Register::getShader(string name) {
		return this->getShaderPointer(name)->get();
	}
	
	//---------
	ofSoundPlayer & Register::getSound(string name) {
		return this->getSoundPointer(name)->get();
	}
	
	//---------
	shared_ptr<Font> Register::getFontPointer(string name) {
		this->checkLoaded();
		return this->fonts[name];
	}
	
	//---------
	shared_ptr<Image> Register::getImagePointer(string name) {
		this->checkLoaded();
		return this->images[name];
	}
	
	//---------
	shared_ptr<Shader> Register::getShaderPointer(string name) {
		this->checkLoaded();
		return this->shaders[name];
	}
	
	//---------
	shared_ptr<Sound> Register::getSoundPointer(string name) {
		this->checkLoaded();
		return this->sounds[name];
	}

	//---------
	Set<Font> & Register::getFonts() {
		this->checkLoaded();
		return this->fonts;
	}

	//---------
	Set<Image> & Register::getImages() {
		this->checkLoaded();
		return this->images;
	}

	//---------
	Set<Shader> & Register::getShaders() {
		this->checkLoaded();
		return this->shaders;
	}
	
	//---------
	Set<Sound> & Register::getSounds() {
		this->checkLoaded();
		return this->sounds;
	}
	
#if defined(__DEBUGGING__) || defined(_DEBUG)
	//---------
	//when in debug mode, we copy an addon's asset folder locally
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
#endif
	
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
	void Register::setDirectoryWatcherEnabled(bool directoryWatcherEnabled) {
		if (directoryWatcherEnabled == this->getDirectoryWatcherEnabled()) {
			return;
		}

		if (!directoryWatcherEnabled) {
			this->directoryWatcher.reset();
		}
		else {
			string watchDirectory = "E:\\";
			this->directoryWatcher = make_shared<Poco::DirectoryWatcher>(watchDirectory);
			this->directoryWatcher->itemModified += Poco::delegate(this, &Register::callbackFileModified);
			this->directoryWatcher->itemAdded += Poco::delegate(this, &Register::callbackFileModified);
		}
	}

	//---------
	bool Register::getDirectoryWatcherEnabled() const {
		if (this->directoryWatcher) {
			return true;
		}
		else {
			return false;
		}
	}

	//---------
	vector<int> Register::getFontSizes(string name) {
		this->checkLoaded();
		return this->fonts[name]->getSizes();
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
	bool Register::isInitialised() const {
		return this->initialised;
	}

	//---------
	void Register::checkLoaded() {
		if(!this->isInitialised()) {
			this->load();
		}
	}
	
	//---------
	void Register::load() {
		//we call this function to ensure that files are loaded
		const auto dataPath = fs::path(ofToDataPath("")) / "assets";
		
		for (const auto addon : this->addonsRegistered) {
			//if addon = "" we're referring to root namespace
			if (this->addonsLoaded.find(addon) != this->addonsLoaded.end()) {
				//we've already loaded this addon. call refresh() if you want to load it again
				continue;
			}
			
			auto addonDataPath = dataPath;
			if (!addon.empty()) {
				addonDataPath /= addon;
			}
			
			if (!fs::is_directory(addonDataPath)) {
				ofLogWarning("ofxAssets") << "Assets data path [" << addonDataPath.string() << "] cannot be found.";
				continue;
			}
			
			vector<string> outputNamespace;
			if (!addon.empty()) {
				outputNamespace.push_back(addon);
			}
			
			this->fonts.addDirectory(addonDataPath / "fonts", outputNamespace);
			this->images.addDirectory(addonDataPath / "images", outputNamespace);
			this->shaders.addDirectory(addonDataPath / "shaders", outputNamespace);
			this->sounds.addDirectory(addonDataPath / "sounds", outputNamespace);
			
			this->addonsLoaded.insert(addon);
		}
		
		this->initialised = true;
		ofNotifyEvent(evtLoad, this);
	}
	
	//----------
	void Register::unload() {
		this->fonts.clear();
		this->images.clear();
		this->shaders.clear();
		this->sounds.clear();
		
		this->initialised = false;
		this->addonsLoaded.clear();
	}

	//----------
	void Register::callbackFileModified(const Poco::DirectoryWatcher::DirectoryEvent & args) {
		auto fileChanged = args.item.path();
		for (auto asset : this->shaders) {
			cout << fileChanged << " vs " << asset.second->getFilename() << endl;
		}
	}
}