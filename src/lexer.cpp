#include "lexer.h"
#include <stack>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "common.h"

extern bool errorFlag;

using namespace std;

Lex::Lex(Lexem _type, int _posInFile) :type(_type), posInFile(_posInFile) {}

Lex::Lex(Lexem _type, int _posInFile, const std::string & s) : type(_type), posInFile(_posInFile), sInfo(s) {}

Lex::Lex(Lexem _type, int _posInFile, int i) : type(_type), posInFile(_posInFile), iInfo(i) {}

Lex::Lex(Lexem _type, int _posInFile, double d) : type(_type), posInFile(_posInFile), dInfo(d) {}

bool Lex::operator==(Lexem l) const { return type == l; }
LexException::LexException(std::string _str, int _index) : str(_str), index(_index) {
	errorFlag = true;
};

Lexer::Lexer(const std::string & text) :index(0) {
	ds = vector<char>(text.begin(), text.end());
	if (ds.size() == 0) {
		throw LexException("empty or not existing file", 0);
	}
	ds.push_back('\n');
	ds.push_back('\n');
}

inline char & Lexer::n() {
	if (index >= ds.size()) {
		throw LexException("unexpected end of file!", ds.size() - 1);
	}
	return ds[index];
}

inline std::string Lexer::n(int count) {
	if (index + count >= ds.size()) {
		throw LexException("unexpected end of file!", ds.size() - 1);
	}
	return string(ds.begin() + index, ds.begin() + index + count);
}

bool Lexer::skipSpace() {
	if (index < ds.size() && isspace(n())) {
		while (index < ds.size() && isspace(n()))index++;
		return true;
	}
	return false;
}

bool Lexer::skipComment() {
	if ((index < ds.size() - 1) && n(2) == "//") {
		while (index < ds.size() && n() != '\n')index++;
		index++;
		return true;
	}
	return false;
}

bool Lexer::skipMultiLineComment() {
	if ((index < ds.size() - 1) && n(2) == "/*") {
		while ((index < ds.size() - 1) && n(2) != "*/")index++;
		index += 2;
		return true;
	}
	return false;
}

void Lexer::skip() {
	while (skipSpace() || skipComment() || skipMultiLineComment());
}

bool Lexer::nextIs(const std::string & s) {
	return (index + s.size() < ds.size()) && (string(ds.begin() + index, ds.begin() + index + s.size()) == s);
}

bool Lexer::checkAll(std::vector<std::pair<const std::string, Lexem>> l) {
	for (auto p : l) {
		if (nextIs(p.first)) { // TODO fix the ability to write keywords together.
			res.push_back({ p.second, index });
			index += p.first.size();
			return true;
		}
	}
	return false;
}

double Lexer::getInt() { // TODO rename to float or IntFloat
	string str;
	while (index < ds.size() && (isdigit(n()) || n() == '.')) {
		str += n();
		index++;
	}
	return stod(str);
}

std::string Lexer::getWord() {
	string str;
	while (index < ds.size() && (isalpha(n()) || isdigit(n()) || n() == '_')) {
		str += n();
		index++;
	}
	return str;
}

std::vector<Lex> Lexer::proc() {
	skip();
	for (; index < ds.size() - 1; skip()) {
		if ((res.size() >= 2) &&
			((res.end() - 1)->type == FLHS) &&
			((res.end() - 2)->type == NAME) &&
			((res.end() - 2)->sInfo == "CRPL")) {
			string str;
			int t = index;
			while (ds[index] != '}') {
				str += ds[index];
				index++;
			}
			(res.end() - 1)->sInfo = str;
			res.push_back({ FRHS, index });
			index++;
			continue;
		}
		if (checkAll(LexString)) continue;
		if (isdigit(n())) {
			int t = index;
			string str;
			while (index < ds.size() && (isdigit(n()) || n() == '.')) { str += n(); index++; }
			if (any_of(str.begin(), str.end(), [](char ch) {return ch == '.'; })) {
				if (count_if(str.begin(), str.end(), [](char ch) {return ch == '.'; }) > 1) {
					throw LexException("Wrong format of float number", index);
				}
				res.push_back({ DOUBLE, t, stod(str) });
			}
			else
				res.push_back({ INT, t, stoi(str) });
			continue;
		}
		if (isalpha(n()) || n() == '_') {
			int t = index;
			res.push_back({ NAME, t, getWord() });
			continue;
		}
		if (n() == '\"') {
			int t = index;
			string str;
			index++;
			while (index<ds.size() && (n() != '\"')) {
				str += n();
				index++;
			}
			index++;
			res.push_back({ STRING, t, str });
			continue;
		}
		if (n() == '#') { // rewrite all the stuff. TODO
			index++;
			if (!nextIs("include"))
				throw LexException("unknown directive", index);
			skip();
			index += string("include").size();
			skip();
			if (n() != '\"')
				throw LexException("expected \'\"'", index);
			index++;

			string fileName;
			while (n() != '\"') {
				fileName += n();
				index++;
			}
			if (!fileExists(fileName.c_str()))throw LexException("can't open file", index);
			if (n() != '\"')
				throw LexException("expected \'\"'", index);
			index++;

			ds.insert(ds.begin() + index + 1, istreambuf_iterator<char>(ifstream(fileName)), istreambuf_iterator<char>());
			continue;
		}
		throw LexException("unexpected symbol \"" + string() + n() + "\"!", index);
	}
	return res;
}
