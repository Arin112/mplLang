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

std::string to_string(Lexem l) {
	if (l == INT) {
		return "INT";
	}
	else if (l == NAME) {
		return "NAME";
	}
	else if (l == STRING) {
		return "STRING";
	}
	else if (l == DOUBLE) {
		return "DOUBLE";
	}
	else {
		for (auto i : LexString) {
			if (l == i.second) {
				return i.first;
			}
		}
	}
	throw "can't be";
}