#include "common.h"
#include <fstream>
#include <cctype>
#include <algorithm>

bool fileExists(const char* name) {
	std::ifstream f(name);
	return f.good();
}

std::string toLower(const std::string & s) {
	std::string tStr = s;
	std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
	return tStr;
}
