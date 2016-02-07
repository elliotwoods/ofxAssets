#include "BaseAsset.h"

namespace ofxAssets {
	//----------
	void BaseAsset::setPath(const filesystem::path & path) {
		this->path = path;
	}
	
	//----------
	const filesystem::path & BaseAsset::getPath() const {
		return this->path;
	}
	
	//----------
	string BaseAsset::getFilename() const {
		return this->getPath().string();
	}
	
	//----------
	bool BaseAsset::isAssociatedWith(const set<filesystem::path> & paths) {
		for(const auto & path : paths) {
			if(path == this->getFilename()) {
				return true;
			}
		}
		return false;
	}
}