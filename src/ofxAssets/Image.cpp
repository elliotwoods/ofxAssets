#include "Image.h"

namespace ofxAssets {
	//----------
	regex Image::getMask() const {
		return regex("^.*\\.(jpg|jpeg|png|gif|tif|tiff|gif)$");
	}
	
	//----------
	void Image::reload() {
		this->image.load(this->getFilename());
	}
	
	//----------
	ofImage & Image::get() {
		return this->image;
	}
}