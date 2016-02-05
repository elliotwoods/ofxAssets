#pragma once

#include "BaseAsset.h"
#include "ofImage.h"

namespace ofxAssets {
	class Image : public BaseAsset {
	public:
		regex getMask() const override;
		void reload() override;
		
		ofImage & get();
	protected:
		ofImage image;
	};
}