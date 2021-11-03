#include "Font.h"

namespace ofxAssets {
	//----------
	regex Font::getMask() const{
		return regex("^.*\\.(ttf)$");
	}
	
	//----------
	void Font::reload() {
		//because of lazy loading, just unload at this point
		this->sizes.clear();
	}

	//----------
	vector<int> Font::getSizes() const {
		vector<int> sizes;
		for(auto size : this->sizes) {
			sizes.emplace_back(size.first);
		}
		return sizes;
	}
	
	//----------
	void Font::setFullCharacterSetEnabled(bool fullCharacterSetEnabled) {
		if (fullCharacterSetEnabled != this->fullCharacterSetEnabled) {
			this->fullCharacterSetEnabled = fullCharacterSetEnabled;

			// clear out previously loaded fonts
			this->sizes.clear();
		}
	}

	//----------
	bool Font::getFullCharacterSetEnabled() const {
		return this->fullCharacterSetEnabled;
	}

	//----------
	ofTrueTypeFont & Font::get(int size) {
		auto findFont = this->sizes.find(size);
		if(findFont != this->sizes.end()) {
			//it's already available
			return findFont->second;
		}
		else {
			//let's load it
			auto inserted = this->sizes.emplace(size, ofTrueTypeFont());
			
			auto & font = inserted.first->second;

			ofTrueTypeFontSettings settings(this->getFilename()
				, size);
			{
				settings.antialiased = true;
				settings.contours = true;
				if (this->fullCharacterSetEnabled) {
					// ranges for font awesome
					settings.addRange(ofUnicode::range{
					0xf000
					, 0xf8ff
						});
				}
			}
			font.load(settings);

			return font;
		}
	}
}