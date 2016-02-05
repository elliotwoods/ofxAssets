#include "Sound.h"

#ifdef HAS_OFXAUDIODECODER
#include "ofxAudioDecoder.h"
#endif

namespace ofxAssets {
	//----------
	regex Sound::getMask() const {
		return regex("^.*\\.(wav|aif|aiff|mp3)$");
	}
	
	//----------
	void Sound::reload() {
		this->player.load(this->getFilename());
#ifdef HAS_OFXAUDIODECODER
		ofxAudioDecoder::load(this->buffer, this->getFilename());
#endif
	}
	
	//----------
	ofSoundPlayer & Sound::getSoundPlayer() {
		return this->player;
	}
	
	//----------
	ofSoundBuffer & Sound::getSoundBuffer() {
		return this->buffer;
	}
}