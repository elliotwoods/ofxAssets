#include "Set.h"

#include "Font.h"
#include "Image.h"
#include "Shader.h"
#include "Sound.h"

namespace fs = std::filesystem;

namespace ofxAssets {
#pragma mark Set
	//----------
	template<typename Type>
	Set<Type>::Set() {
		this->blank = make_shared<Type>();
		this->mask = Type().getMask();
	}

	//----------
	template<typename Type>
	void Set<Type>::addDirectory(const fs::path & path, const Namespace & targetNamespace) {
		try {
			if (fs::is_directory(path)) {
				for (const auto & entry : fs::directory_iterator(path)) {
					const auto & path = entry.path();
					
					if (fs::is_directory(path)) {
						//it's a subfolder if so traverse it
						auto innerNamespace = targetNamespace;
						innerNamespace.push_back(path.filename().string());
						this->addDirectory(path, innerNamespace);
					}
					else {
						//it's an asset
						auto extension = ofToLower(path.extension().string());
						if (regex_match(extension, this->mask)) {
							//matches the extension mask
							string name = path.stem().string();
							
							//get the true asset name
							transformName(name, targetNamespace);
							
							//if we already have it ignore it
							if(this->has(name)) {
								continue;
							}
							
							//create the asset
							auto asset = make_shared<Type>();
							asset->setPath(path);
							asset->reload();
							
							//register the asset
							pair<string, shared_ptr<Type>> inserter(name, asset);
							this->insert(inserter);
						}
					}
				}
			}
		}
		catch(...)
		{
			ofLogError("ofxAssets") << "Adding directory [" << path.string() << "] failed";
		}
	}
	
	//----------
	template<typename Type>
	bool Set<Type>::has(const string & name) const {
		return this->find(name) != this->end();
	}

	//----------
	template<typename Type>
	vector<string> Set<Type>::getNames() const {
		vector<string> names;
		for(const auto & asset : * this) {
			names.push_back(asset.first);
		}
		return names;
	}

	//----------
	template<typename Type>
	shared_ptr<Type> Set<Type>::operator[](const string & name) const {
		auto findAsset = this->find(name);
		if(findAsset == this->end()) {
			ofLogWarning("ofxAssets::Set") << "Cannot get asset [" << name << "] of type [" << typeid(Type).name() << "]";
			return this->blank;
		}
		else {
			return findAsset->second;
		}
	}
	
	//----------
	template<typename Type>
	void Set<Type>::transformName(string & name, const Namespace & _namespace) {
		string flatNamespace = "";
		for (auto level : _namespace) {
			flatNamespace += level + "::";
		}
		name = flatNamespace + name;
	}

	//----------
	template class Set<Font>;
	template class Set<Image>;
	template class Set<Shader>;
	template class Set<Sound>;
}