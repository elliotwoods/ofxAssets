#pragma once

#include "BaseAsset.h"
#include "ofSoundPlayer.h"
#include "ofSoundBuffer.h"

namespace ofxAssets {
	class Sound : public BaseAsset {
	public:
		virtual regex getMask() const override;
		virtual void reload() override;
		
		ofSoundPlayer & get() { return this->getSoundPlayer(); }
		ofSoundPlayer & getSoundPlayer();
		ofSoundBuffer & getSoundBuffer();
	protected:
		ofSoundPlayer player;
		ofSoundBuffer buffer; /// Note : you need ofxAudioDecoder in your project for this to be filled. And define HAS_OFXAUDIODECODER in your project preprocessor flags
	};
}
