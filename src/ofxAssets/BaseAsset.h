#pragma once

#include <string>
#include <vector>
#include <regex>
#include "ofFileUtils.h"

using namespace std;

namespace ofxAssets {
	typedef vector<string> Namespace;
	
	class BaseAsset {
	public:
		virtual regex getMask() const = 0;
		virtual void reload() = 0;
		
		void setPath(const filesystem::path &);
		const filesystem::path & getPath() const;

		string getFilename() const;
	protected:
		filesystem::path path;
	};
}