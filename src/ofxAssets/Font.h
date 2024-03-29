#pragma once

#include "BaseAsset.h"
#include "ofTrueTypeFont.h"

namespace ofxAssets {
	class Font : public BaseAsset {
	public:
		regex getMask() const override;
		void reload() override;
		
		ofTrueTypeFont & get(int size);
		
		vector<int> getSizes() const;

		void setFullCharacterSetEnabled(bool);
		bool getFullCharacterSetEnabled() const;

		void setContoursEnabled(bool);
		bool getContoursEnabled() const;
	protected:
		map<int, ofTrueTypeFont> sizes;
		bool contoursEnabled = true;
		bool fullCharacterSetEnabled = false;
	};
}