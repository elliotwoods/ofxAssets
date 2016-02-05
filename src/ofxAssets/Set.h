#pragma once

#include "BaseAsset.h"
#include "Font.h"
#include "Image.h"
#include "Shader.h"
#include "Sound.h"

#include <map>
#include <string>
#include <memory>
#include <regex>

#include "ofFileUtils.h"

using namespace std;

namespace ofxAssets {
	template<typename Type>
	class Set : public map<string, shared_ptr<Type>>{
	public:
		Set();
		
		void addDirectory(const filesystem::path &, const Namespace & targetNamespace);
		
		bool has(const string & name) const;
		vector<string> getNames() const;
		shared_ptr<Type> operator[](const string & name) const;
		
		static void transformName(string &, const Namespace &);
	protected:
		shared_ptr<Type> blank;
		regex mask;
	};
}