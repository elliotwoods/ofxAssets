#include "Shader.h"

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
		
		auto success = this->shader.load(absolutePath.string());
		
		if(!success) {
			ofLogWarning("ofxAssets::Shader") << "Failed to load " << absolutePath.string();
		}
	}
	
	//----------
	ofShader & Shader::get() {
		return this->shader;
	}
}