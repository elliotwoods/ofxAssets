#pragma once

#include "BaseAsset.h"
#include "ofShader.h"

namespace ofxAssets {
	class Shader : public BaseAsset {
	public:
		regex getMask() const override;
		void reload() override;
		
		ofShader & get();
	protected:
		ofShader shader;
	};
}