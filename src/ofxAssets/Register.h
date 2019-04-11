#pragma once

#include "Font.h"
#include "Image.h"
#include "Shader.h"
#include "Sound.h"
#include "Set.h"

#ifdef HAS_OFXSINGLETON
#include "ofxSingleton.h"
#endif

#include "ofEvents.h"
#include <map>
#include <unordered_set>
#include <string>
#include <regex>

#include "Watchdog.h"

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
		virtual ~Register();
			
		void refresh();
		void clear();
		
		ofTrueTypeFont & getFont(string name, int size);
		ofImage & getImage(string name);
		ofShader & getShader(string name);
		ofSoundPlayer & getSound(string name);
		
		shared_ptr<Font> getFontPointer(string name);
		shared_ptr<Image> getImagePointer(string name);
		shared_ptr<Shader> getShaderPointer(string name);
		shared_ptr<Sound> getSoundPointer(string name);
		
		Set<Font> & getFonts();
		Set<Image> & getImages();
		Set<Shader> & getShaders();
		Set<Sound> & getSounds();
		
		vector<int> getFontSizes(string name);
		
		/// Load assets for an addon, e.g.:
		///		data/ofxMultiTrack/images/0.png
		/// becomes
		///		image("ofxMultiTrack::0")
		void addAddon(string addonName);
		
		void setDirectoryWatcherEnabled(bool);
		bool getDirectoryWatcherEnabled() const;

		ofEvent<void> evtLoad;
		
	protected:
		template<typename T>
		inline void applyToAllAssets(T function) {
			for(auto asset : this->fonts) {
				function(asset.second);
			}
			for(auto asset : this->images) {
				function(asset.second);
			}
			for(auto asset : this->shaders) {
				function(asset.second);
			}
			for(auto asset : this->sounds) {
				function(asset.second);
			}
		}
		
		bool isInitialised() const;
		inline void checkLoaded();
		void load(); // make sure everything is loaded
		void unload(); // unload everything
		
		void update(ofEventArgs &);
		
		unordered_set<string> addonsRegistered;
		unordered_set<string> addonsLoaded;
		
		Set<Font> fonts;
		Set<Image> images;
		Set<Shader> shaders;
		Set<Sound> sounds;
		
		bool initialised = false;

		
		void rebuildDirectoryWatchers();
		void callbackFileModified(const filesystem::path &);
		void checkAssetsChanged();
		struct {
			set<filesystem::path> watchedPaths;
			bool enabled = false;
			
			set<filesystem::path> changedFiles;
			mutex changedFilesMutex;
		} directoryWatchers;
	};
}