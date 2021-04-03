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
		ofAddListener(ofEvents().update, this, &Register::update);
		this->clear();
	}

	//---------
	Register::~Register() {
		ofRemoveListener(ofEvents().update, this, &Register::update);
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
		try {
			auto addonAssetsSource = fs::path(ofToDataPath("../../../../../addons/" + addonName + "/data/assets/" + addonName + "/", true));
			auto addonAssetsDestination = fs::path(ofToDataPath("assets/" + addonName, true));
			if (fs::exists(addonAssetsSource)) {
				ofLogVerbose("ofxAssets") << "Copying in addon files from " << addonAssetsSource;
				try {
					fs::remove_all(addonAssetsDestination);
					copyDir(addonAssetsSource, addonAssetsDestination);
				}
				catch (const std::exception & e) {
					ofLogWarning("ofxAssets") << e.what();
				}
			}
			else {
				ofLogWarning("ofxAssets") << "Cannot copy in addon assets since folder doesn't exist : " << addonAssetsSource;
			}
		}
		catch (...) {
			ofLogError("ofxAssets") << "Copying assets for addon  [" << addonName << "] failed.";
		}
#endif
	}

	//---------
	void Register::setDirectoryWatcherEnabled(bool directoryWatcherEnabled) {
		if (directoryWatcherEnabled == this->getDirectoryWatcherEnabled()) {
			return;
		}

		if (!directoryWatcherEnabled) {
			for (const auto & watchedPath : this->directoryWatchers.watchedPaths) {
				wd::unwatch(watchedPath);
			}
			this->directoryWatchers.watchedPaths.clear();
		}
		else {
			//flag this to happen on next update (we use flags because we rebuild on add events)
			this->directoryWatchers.enabled = true;
			this->rebuildDirectoryWatchers();
		}
	}

	//---------
	bool Register::getDirectoryWatcherEnabled() const {
		return this->directoryWatchers.enabled;
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
		if (!this->isInitialised()) {
			this->load();
		}
	}

	//---------
	void Register::load() {
		//we call this function to ensure that files are loaded
		const auto assetsPath = fs::path(ofToDataPath("", true)) / "assets";

		for (const auto addon : this->addonsRegistered) {
			//if addon = "" we're referring to root namespace
			if (this->addonsLoaded.find(addon) != this->addonsLoaded.end()) {
				//we've already loaded this addon. call refresh() if you want to load it again
				continue;
			}

			auto addonDataPath = assetsPath;
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
		this->rebuildDirectoryWatchers();
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
	void Register::update(ofEventArgs &) {
		if (this->directoryWatchers.enabled) {
			this->checkAssetsChanged();
		}
	}

	//----------
	void Register::rebuildDirectoryWatchers() {
		// we don't use wd::unwatchAll() in case anybody else is using it
		for (const auto & watchedPath : this->directoryWatchers.watchedPaths) {
			wd::unwatch(watchedPath);
		}
		this->directoryWatchers.watchedPaths.clear();
		
		auto makeWatcher = [this](shared_ptr<BaseAsset> asset) {
			const auto assetPath = filesystem::path(asset->getFilename());
			wd::watch(assetPath, [this](const filesystem::path & path) {
				this->callbackFileModified(path);
			});
		};
		
		applyToAllAssets(makeWatcher);
	}
	
	//----------
	void Register::callbackFileModified(const filesystem::path & path) {
		// Add to the cache of updated files (these will be reloaded on next update)
		this->directoryWatchers.changedFilesMutex.lock();
		this->directoryWatchers.changedFiles.insert(path);
		this->directoryWatchers.changedFilesMutex.unlock();
	}
	
	//----------
	void Register::checkAssetsChanged() {
		// This function is called when a directory watcher event occurs
		// We check through the files flagged by the directoryWatcher and reload any associated assets
		
		//create a local copy of the flagged files and clear the buffer
		set<filesystem::path> changedFiles;
		{
			this->directoryWatchers.changedFilesMutex.lock();
			swap(changedFiles, this->directoryWatchers.changedFiles);
			this->directoryWatchers.changedFilesMutex.unlock();
		}
		
		//function which will reload an asset if its pattern matches a file which has changed
		auto checkAsset = [& changedFiles](shared_ptr<BaseAsset> asset) {
			if (asset->isAssociatedWith(changedFiles)) {
				asset->reload();
			}
		};
		
		//apply this test function to all assets
		this->applyToAllAssets(checkAsset);
	}
}