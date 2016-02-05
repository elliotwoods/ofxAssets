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
}