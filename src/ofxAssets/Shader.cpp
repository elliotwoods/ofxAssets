#include "Shader.h"

#include "ofLog.h"

namespace ofxAssets {
	//----------
	regex Shader::getMask() const {
		return regex("^.*\\.(vert|frag|geom)$");
	}
	
	//----------
	void Shader::reload() {
		const auto & path = this->getPath();
		const auto withoutExtension = path.parent_path() / path.stem();
		const auto absolutePath = filesystem::absolute(withoutExtension);
		
		const auto stem = absolutePath.string();

		bool success = false;
		if (ofFile::doesFileExist(stem + ".geom")
			&& ofFile::doesFileExist(stem + ".vert")
			&& ofFile::doesFileExist(stem + ".frag")) {
			success = this->shader.load(stem + ".vert", stem + ".frag", stem + ".geom");
		}
		else {
			success = this->shader.load(absolutePath.string());
		}

		if(!success) {
			ofLogWarning("ofxAssets::Shader") << "Failed to load " << absolutePath.string();
		}
	}
	
	//----------
	ofShader & Shader::get() {
		return this->shader;
	}
}