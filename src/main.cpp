#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <execution>

#include "lexer.h"
#include "common.h"

#include "stdfuncs.hpp"

bool errorFlag = false; // true if any error occurred during the translation

using namespace std;

string errOut(const string &s, int index) {
	int i1 = index, i2 = index;
	while (i1 >= 0 && s[i1] != '\n' && s[i1] != '\t'&& index - i1 < 30)i1--;
	if (i1 < 0)i1++;
	if (s[i1] == '\n' || s[i1] == '\t')i1++;
	while (i2 < s.size() && s[i2] != '\n' && i2 - index < 30)i2++;
	if (i1 > i2)i1 = i2;
	string str(s.begin() + i1, s.begin() + i2);
	str += '\n';
	int ti = i2, num = 0;
	while (ti >= 0)if (s[ti--] == '\n')num++;
	str = "In line #" + to_string(num) + "\n" + str;
	for (int i = 0; i < index - i1; i++) {
		str += ' ';
	}
	str += '^';
	return str;
}

class ParsException {
public:
	string what;
	int pos;
	ParsException(string what_, int where_) :what(what_), pos(where_) {
		errorFlag = true;
	}
};

enum fType { CRPL, MPL, /*DEBUG,*/ INLINE, NOSPACEFORWARD };

class mplFunction {
public:
	string body, name;
	// int cntIn, cntOut;
	bool used;
	vector<fType> type;
	mplFunction(string body_, string name_, vector<fType> type_) :body(body_), name(name_), used(false), type(type_) {}
	mplFunction() : used(false)/*:cntIn(-1), cntOut(-1)*/ {  }
	mplFunction(string s) :body(s), name(s), type{ INLINE, CRPL }, used(false){}
	bool operator==(fType t) { return any_of(type.begin(), type.end(), [&](fType it) {return t == it; }); }
};

class Parser {
	string fileOut;
	const vector<Lex> lexems;
	vector<mplFunction> funcs;
	int index;
	string *text;
	string curIndent;
public:
	Parser(const vector<Lex>& v, string *text_) :lexems(v), index(0), text(text_), curIndent("") {
		for (auto s : stdFuncs) {
			funcs.push_back({ s });
		}
		funcs.push_back({ "\"\n\"" , "endl", {INLINE, CRPL} });
		funcs.push_back({ "" , "stack", {INLINE, CRPL} });
		funcs.push_back({ "<-!", "refRead", {INLINE, CRPL} });
		funcs.push_back({ "->!", "refWrite", {INLINE, CRPL} });
		funcs.push_back({ "-?!", "refExists", {INLINE, CRPL} });
		funcs.push_back({ "--?", "refDelete", {INLINE, CRPL} });

		funcs.push_back({ "-?", "exists", {INLINE, CRPL, NOSPACEFORWARD} });
		funcs.push_back({ "--", "delete", {INLINE, CRPL, NOSPACEFORWARD} });
	}
	inline Lex n() {
		if (index >= lexems.size())throw ParsException("unexpected end of file", (lexems.end() - 1)->posInFile);
		return lexems[index];
	}

	Lex eat(Lexem l) {
		if (lexems[index] == l) {
			if (l == FLHS)indUp();
			if (l == FRHS)indDown();
			index++;
			return lexems[index - 1];
		}
		else {
			throw ParsException("expected " + to_string(l) + " but get " + to_string(n().type), n().posInFile);
		}
	}

	string ind() {
		return curIndent;
	}

	void indUp() {
		curIndent += "    ";
	}

	void indDown() {
		if (curIndent.size() >= 4)curIndent.erase(curIndent.end() - 4, curIndent.end());
	}

	void addIJK() {
		auto present = [&](string str) {return any_of(listExceptions.begin(), listExceptions.end(), [&](auto it) {return toLower(it) == toLower(str); }); };
		auto add = [&](string str) {listExceptions.push_back(str); };
		if (present("i")) {
			if (present("j")) {
				if (present("k")) {
					add("k");
					// warn("using 4 or more nested loops is not recomended");
				}
				else {
					add("k");
				}
			}
			else {
				add("j");
			}
		}
		else {
			add("i");
		}
	}

	void removeIJK() {
		auto present = [&](string str) {return any_of(listExceptions.begin(), listExceptions.end(), [&](auto it) {return toLower(it) == toLower(str); }); };
		auto rem = [&](string str) {auto p = find(listExceptions.begin(), listExceptions.end(), str); if (p != listExceptions.end())listExceptions.erase(p); };
		if (present("k")) {
			rem("k");
		}
		else if (present("j")) {
			rem("j");
		}
		else if(present("i")) {
			rem("i");
		}
		else {
			throw "can't be";
		}
	}

	void proc() {
		try {
			all();
		}
		catch (ParsException e) {
			cerr << "fatal error during file parse\n" <<
				errOut(*text, e.pos) << endl << e.what << endl << endl;
			return;
		}

		if (!any_of(funcs.begin(), funcs.end(), [](auto it) {return it.name == "main"; })) {
			cerr << "No main function defined!!" << endl;
		}

		auto m = *find_if(funcs.begin(), funcs.end(), [](auto it) {return it.name == "main"; });
		if (m == INLINE) {
			cout << m.body << endl;
		}
		else {
			cout << "@main\n" << endl;
		}
		for (auto it : funcs) {
			if (it == INLINE || !it.used)continue;
			cout << ":" + it.name + "\n" + it.body << endl;
		}
		if (!any_of(funcs.begin(), funcs.end(), [](auto it) {return it.name == "main"; })) {
			throw std::logic_error("No main function defined!!");
		}
	}

	void all() {

		while (index < lexems.size()) {
			if (n() == DOLLAR) {
				eat(DOLLAR);
				cout << "$";
				cout << eat(NAME).sInfo;
				eat(COLON);
				cout << ":";
				if (n() == INT || n() == DOUBLE) {
					cout << num();
				}
				else if (n() == STRING) {
					cout << eat(STRING).sInfo;
				}
				else {
					throw ParsException("Number or string expected!", n().posInFile);
				}
				cout << "\n";
			}
			else {
				if (any_of(funcs.begin(), funcs.end(), [&](auto it) {return it.name == n().sInfo; }))
					throw ParsException("redefinition is not acceptable", n().posInFile);
				funcs.push_back(fdef());
			}
		}
	}

	mplFunction fdef() {
		mplFunction f;
		//if (n() == INT)f.cntOut = eat(INT).iInfo;
		f.name = eat(NAME).sInfo;
		eat(LHS);
		//if (n() == INT)f.cntIn = eat(INT).iInfo;
		eat(RHS);

		f.type = ftypeList();

		if (any_of(f.type.begin(), f.type.end(), [](fType t) {return t == CRPL; }) && f.type.back() != CRPL)
			throw ParsException("CRPL function type should be last type in type list.", n().posInFile);

		string tStr = eat(FLHS).sInfo;
		if (any_of(f.type.begin(), f.type.end(), [](fType t) {return t == INLINE; })) {
			curIndent = "";
		}
		if (f.type.back() == CRPL) {
			f.body = tStr;
		}
		else {
			f.body = fbody();
		}
		eat(FRHS);
		return f;
	}

	vector <fType> ftypeList() {
		vector <fType> v;
		auto tofType = [&](string s) {
			if (s == "CRPL")return CRPL;
			if (s == "inline")return INLINE;
			//if (s == "debug")return DEBUG;
			throw ParsException("unknown function type \"" + s + "\", expected CRPL or inline"/*", or debug"*/, lexems[index - 1].posInFile);
		};
		if (n() == NAME) {
			string str = eat(NAME).sInfo;
			v.push_back(tofType(str));
		}
		while (n() == COMMA) {
			eat(COMMA);
			string str = eat(NAME).sInfo;
			v.push_back(tofType(str));
		}
		if (v.empty())v.push_back(MPL);
		return v;
	}

	string fbody() {
		string str;
		while (!(n() == FRHS)) {
			str += foper();
		}
		return str;
	}

	string foper() {
		if (n() == WHILE) {
			return whileExpr();
		}
		else if (n() == IF) {
			return ifExpr();
		}
		else if (n() == ONCE) {
			return onceExpr();
		}
		else if (n() == DO) {
			return doExpr();
		}
		else if (n() == KLHS) {
			string str = ind() + multiAssign();
			eat(SEMICOLON);
			return str + "\n";
		}
		else if (n() == FOR) {
			return forExpr();
		}
		else if (!(n() == SEMICOLON)) {
			string str = ind() + expr();
			eat(SEMICOLON);
			return str + "\n";
		}
		else {
			eat(SEMICOLON);
			return "\n";
		}
	}

	string weakBody() {
		string str;
		if (n() == FLHS) {
			eat(FLHS);
			str += fbody();
			eat(FRHS);
		}
		else {
			indUp();
			str += foper();
			indDown();
		}
		return str;
	}

	string whileExpr() {
		string str = ind() + "while ";
		eat(WHILE);
		eat(LHS);
		str += expr();
		eat(RHS);
		str += " repeat\n";
		str += weakBody();
		str += ind() + "endwhile\n";
		return str;
	}

	string ifExpr() {
		string str;
		eat(IF);
		eat(LHS);
		str += ind() + expr() + " if\n";
		eat(RHS);
		str += weakBody();
		str += Else();
		return str;

	}

	string Else() {
		if (n() == ELSE) {
			eat(ELSE);
			string str = ind() + "else\n";
			str += weakBody();
			str += ind() + "endif\n";
			return str;
		}
		else {
			return ind() + "endif\n";
		}
	}

	string onceExpr() {
		string str = ind() + "once\n";
		eat(ONCE);
		str += weakBody();
		str += ind() + "endonce\n";
		return str;
	}

	string doExpr() {
		addIJK();
		eat(DO);
		eat(LHS);
		string tStr = expr();

		if (n() == DPOINT) {
			eat(DPOINT);
			tStr = ind() + expr() + " " + tStr + " do\n";
		}
		else {
			tStr = ind() + tStr + " 0" + " do\n";
		}

		eat(RHS);
		tStr += weakBody();

		tStr += ind() + "loop\n";
		removeIJK();

		return tStr;
	}

	string forExpr() {
		eat(FOR);
		eat(LHS);
		string sOnce, sIf, sAfter;
		if (!(n() == SEMICOLON)) {
			if(!(n()==KLHS))
				sOnce = expr();
			else sOnce = multiAssign();
			eat(SEMICOLON);
		}
		else eat(SEMICOLON);
		if (!(n() == SEMICOLON)) {
			sIf = expr();
			eat(SEMICOLON);
		}else eat(SEMICOLON);
		if (!(n() == RHS)) {
			if (!(n() == KLHS))
				sAfter = expr();
			else sAfter = multiAssign();
		}
		eat(RHS);

		string tStr;
		if (sOnce.size())tStr += ind() + sOnce + "\n";
		tStr += ind() + "while " + (sIf.size() ? sIf : "True") + " repeat\n";
		tStr += weakBody();
		indUp();
		if(sAfter.size())tStr += ind() + sAfter + "\n";
		indDown();
		tStr += ind() + "loop\n";
		return tStr;
	}

	string multiAssign() {
		eat(KLHS);
		vector<string> names;
		names.push_back(eat(NAME).sInfo);

		while (n() == COMMA) {
			eat(COMMA);
			names.push_back(eat(NAME).sInfo);
		}
		eat(KRHS);
		eat(ASSIGN);
		vector<string> exprs;
		exprs.push_back(expr());
		while (n() == COMMA) {
			eat(COMMA);
			exprs.push_back(expr());
		}

		string res;
		for (auto s : exprs) {
			res += s + " ";
		}
		for (int i = names.size() - 1; i >= 0; i--) {
			res += "->" + names[i] + (i ? " " : "");
		}
		return res;
	}

	string expr() {
		string tStr = ternarExpr();
		vector<Lexem> list = { ASSIGNADD , ASSIGNSUB , ASSIGNMUL, ASSIGNDIV, ASSIGNMOD, ASSIGNCONCAT };
		if (n() == ASSIGN) {
			eat(ASSIGN);
			if (tStr.size() >= 2 && tStr[0] == '<' && tStr[1] == '-') {
				tStr.erase(0, 2);
			}
			if (!all_of(tStr.begin(), tStr.end(), [](char ch) {return isalnum(ch) || ch == '_'; }))
				throw ParsException("to the left of the sign, only the variable name can be", lexems[index - 1].posInFile);

			return ternarExpr() + " ->" + tStr;
		}
		else if (any_of(list.begin(), list.end(), [&](auto it) {return n() == it; })) {
			if (tStr.size() >= 2 && tStr[0] == '<' && tStr[1] == '-') {
				tStr.erase(0, 2);
			}
			if (!all_of(tStr.begin(), tStr.end(), [](char ch) {return isalnum(ch) || ch == '_'; }))
				throw ParsException("to the left of the sign, only the variable name can be", lexems[index - 1].posInFile);
			// tStr = "->" + tStr;
			// string st = concatExpr();
			if (n() == ASSIGNADD) {
				eat(ASSIGNADD);
				return "<-" + tStr + " " + ternarExpr() + " add ->" + tStr;
			}
			else if (n() == ASSIGNSUB) {
				eat(ASSIGNSUB);
				return "<-" + tStr + " " + ternarExpr() + " sub ->" + tStr;
			}
			else if (n() == ASSIGNMUL) {
				eat(ASSIGNMUL);
				return "<-" + tStr + " " + ternarExpr() + " mul ->" + tStr;
			}
			else if (n() == ASSIGNDIV) {
				eat(ASSIGNDIV);
				return "<-" + tStr + " " + ternarExpr() + " div ->" + tStr;
			}
			else if (n() == ASSIGNMOD) {
				eat(ASSIGNMOD);
				return "<-" + tStr + " " + ternarExpr() + " mod ->" + tStr;
			}
			else if (n() == ASSIGNCONCAT) {
				eat(ASSIGNCONCAT);
				return "<-" + tStr + " " + ternarExpr() + " concat ->" + tStr;
			}
			else {
				throw "can't be";
			}
		}
		return tStr;
	}

	string ternarExpr() {
		string str = concatExpr();
		if (n() == QMARK) {
			eat(QMARK);
			str += " if";
			if (n() == KLHS) {
				str += " " + multiAssign();
			}
			else {
				str += " " + expr();
			}
			eat(COLON);
			str += " else ";
			if (n() == KLHS) {
				str += multiAssign();
			}
			else {
				str += expr();
			}
			str += " endif";
		}
		return str;
	}

	string concatExpr() {
		string str = cmpORexpr();
		while (n() == DOLLAR) {
			eat(DOLLAR);
			str += " " + cmpORexpr() + " concat";
		}
		return str;
	}

	string cmpORexpr() {
		string str = cmpANDexpr();
		while (n() == OR) {
			eat(OR);
			str += " " + cmpANDexpr() + " or";
		}
		return str;
	}

	string cmpANDexpr() {
		string str = cmpEQexpr();
		while (n() == AND) {
			eat(AND);
			str += " " + cmpEQexpr() + " and";
		}
		return str;
	}

	string cmpEQexpr() {
		string str = cmpLGTexpr();
		while (n() == EQ || n() == NEQ) {
			string tStr = (n() == EQ) ? (eat(EQ), " eq") : (eat(NEQ), " neq");
			str += " " + cmpLGTexpr() + tStr;
		}
		return str;
	}

	string cmpLGTexpr() {
		string str = addExpr();
		while (n() == LT || n() == GT || n() == LTE || n() == GTE) {
			string tStr;
			if (n() == LT) {
				tStr = " lt";
				eat(LT);
			}
			else if (n() == GT) {
				tStr = " gt";
				eat(GT);
			}
			else if (n() == LTE) {
				tStr = " lte";
				eat(LTE);
			}
			else {
				tStr = " gte";
				eat(GTE);
			}
			str += " " + addExpr() + tStr;
		}
		return str;
	}

	string addExpr() {
		string str = mulExpr();
		while (n() == PLUS || n() == MINUS) {
			string tStr = (n() == PLUS) ? (eat(PLUS), " add") : (eat(MINUS), " sub");
			str += " " + mulExpr() + tStr;
		}
		return str;
	}

	string mulExpr() {
		string str = powExpr();
		while (n() == MUL || n() == DIV || n() == MOD) {
			string tStr;
			if (n() == MUL) {
				tStr = " mul";
				eat(MUL);
			}
			else if (n() == DIV) {
				tStr = " div";
				eat(DIV);
			}
			else {
				tStr = " mod";
				eat(MOD);
			}
			str += " " + powExpr() + tStr;
		}
		return str;
	}

	string powExpr() {
		string str = signedAtom();
		while (n() == POW) {
			eat(POW);
			str += " " + signedAtom() + " pow";
		}
		return str;
	}

	string signedAtom() {
		string str;
		if (n() == PLUS) {
			eat(PLUS);
			str = signedAtom();
		}
		else if (n() == MINUS) {
			eat(MINUS);
			str = signedAtom() + " neg";
		}
		else {
			return atom();
		}
		return str;
	}

	string atom() {
		if (n() == INT || n() == DOUBLE) {
			return num();
		}
		else if (n() == STRING) {
			return "\"" + eat(STRING).sInfo + "\"";
		}
		else if (n() == NAME) {
			if ((index + 1 < lexems.size()) && lexems[index + 1] == LHS) {
				return func();
			}
			else {
				string t = eat(NAME).sInfo;
				if (!any_of(listExceptions.begin(), listExceptions.end(), [&](auto it) {return toLower(it) == toLower(t); }))
					return "<-" + t;
				else
					return t;
			}
		}
		else if (n() == LHS) {
			eat(LHS);
			string tStr = expr();
			eat(RHS);
			return tStr;
		}
		return "";
		//throw ParsException("expected int|string|var name|function name but get \'" + to_string(n().type)+"\'", n().posInFile);
	}

	string num() {
		if (n() == DOUBLE) {
			char buf[100];
			double d = eat(DOUBLE).dInfo;
			sprintf(buf, "%lf", d);
			return string(buf);
		}
		else {
			return to_string(eat(INT).iInfo);
		}
	}

	string func() {
		string name = eat(NAME).sInfo;
		if (!any_of(funcs.begin(), funcs.end(), [=](mplFunction it) { return toLower(it.name) == toLower(name); })) {
			throw ParsException("undefined function", lexems[index - 1].posInFile);
		}
		mplFunction &f = *find_if(funcs.begin(), funcs.end(), [&](mplFunction it) {return toLower(it.name) == toLower(name); });
		f.used = true;
		eat(LHS);
		string tStr = exprArgList();
		eat(RHS);
		if (f == NOSPACEFORWARD) {
			if (tStr.size() >= 2 && tStr[0] == '<' && tStr[1] == '-') {
				tStr.erase(0, 2);
			}
			return f.body + tStr;
		}
		else {
			if (f == INLINE)
				return tStr + (tStr.size() ? " " : "") + f.body;
			else
				return tStr + (tStr.size() ? " @" : "@") + name;
		}
	}

	string exprArgList() {
		string str;
		if (!(n() == RHS)) {
			str += expr();
			while (n() == COMMA) {
				eat(COMMA);
				str += " " + expr();
			}
		}
		return str;
	}
};

void proc(const string &in, const string &out) {
	cerr << "translating " << in << "..." << endl;
	string text = string(istreambuf_iterator<char>(ifstream(in)), istreambuf_iterator<char>());
	if (text == "")throw std::logic_error("can't open file or file is empty");
	try {
		Lexer lexer(text);
		const vector<Lex> lexems = lexer.proc();
		freopen(out.c_str(), "w", stdout);

		Parser parser(lexems, &text);
		parser.proc();

		fclose(stdout);

		//cerr << "translation of " << in << " done, result writed to " << out << endl;
	}
	catch (LexException e) {
		cerr << "fatal error : " << e.str << endl << errOut(text, e.index) << endl;
		cerr << "press any key to exit..." << endl;
		cin.get();
	}
}

int main(int argc, char *argv[]) try {
	if (argc < 2) {
		if (!fileExists("test.mpl"))
			throw std::logic_error("no input file");
		proc("test.mpl", "test.crpl");
	}
	else
		for (int i = 1; i < argc; i++) {
			string resName = string(argv[i]);
			if (!fileExists(argv[i]))throw std::logic_error("can't open file " + string(argv[i]));
			auto pos = resName.rfind('.');
			if (pos == string::npos)throw std::logic_error("file must have .mpl extension");
			pos++;
			if (string(resName.begin() + pos, resName.end()) != "mpl")
				throw std::logic_error("file must have .mpl extension");

			resName.erase(resName.begin() + pos, resName.end());
			resName += "crpl";
			proc(argv[i], resName);
		}

	if (errorFlag) {
		cerr << "press any key to exit";
		cin.get();
	}
	return 0;
}
catch (std::logic_error e) {
	cerr << "fatal error: " << e.what() << endl;
	cerr << "press any key to exit";
	cin.get();
	return -1;
}
catch (...) {
	cerr << "unknown error, please send input files to\n"
		"github.com/Arin112/mplLang/issues\n"
		"" << endl;
	cerr << "press any key to exit";
	cin.get();
	return -1;
}