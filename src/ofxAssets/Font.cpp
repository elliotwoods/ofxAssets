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
			font.load(this->getFilename(), size, true, false, true);
			return font;
		}
	}
}