#pragma once
#include <vector>
#include <utility>
#include <string>

enum Lexem {
	NAME, COMMA, LHS, RHS, FLHS, FRHS, KLHS, KRHS, SEMICOLON, COLON, INT, DOUBLE,
	//     ','  '('  ')'  '{'   '}'    '['   ']'    ';'       ':' 

	/*  STRUCT, UNION,      PUBLIC, PRIVATE, PROTECTED, ENUM,*/ MUL, PLUS, MINUS, DIV,
	// 'struct''union'     'public''private''protected''enum'  '*'   '+'   '-'   '/' 

	/**/MOD, LT, GT, LTE, GTE, EQ, NEQ, EQ0, NEQ0, ASSIGN, WHILE, IF,
	//  '%' '<' '>' '<=' '>=' '==''!=' '==0' '!=0'  '='   'while' 'if'

	/**/ONCE, ELSE, OR, AND, POW, STRING, DO, DPOINT, LOOP, DOLLAR,
	// 'once''else''||' '&&' '^'         'do' '..'   'loop' '$'

	ASSIGNADD, ASSIGNSUB, ASSIGNMUL, ASSIGNDIV, ASSIGNMOD, ASSIGNCONCAT
	// '+='      '-='        '*='       '/='      '%='
};

static const std::vector<std::pair<const std::string, Lexem>> LexString = {
	{ ",", COMMA }, { "(", LHS }, { ")", RHS }, { "{", FLHS }, { "}", FRHS },
	{ "[", KLHS }, { "]", KRHS }, { ";", SEMICOLON }, { ":", COLON },
	// { "struct", STRUCT }, { "union", UNION }, { "public", PUBLIC },
	// { "private", PRIVATE }, { "protected", PROTECTED }, { "enum", ENUM },
	{ "+=", ASSIGNADD }, { "-=", ASSIGNSUB },
	{ "*=", ASSIGNMUL }, { "/=", ASSIGNDIV }, { "%=", ASSIGNMOD }, { "$=", ASSIGNCONCAT },
	{ "*", MUL }, { "+", PLUS }, { "-", MINUS }, { "/", DIV }, { "%", MOD },
	{ "<=", LTE }, { ">=", GTE }, { "<", LT }, { ">", GT },
	{ "==", EQ }, { "!=", NEQ }, { "=", ASSIGN },
	{ "while", WHILE }, { "if", IF }, { "once", ONCE}, { "else", ELSE },
	{ "||", OR }, { "&&", AND }, { "^", POW }, { "do" , DO }, { "..", DPOINT },
	{ "loop", LOOP}, { "$", DOLLAR }
};

std::string to_string(Lexem l);

class Lex { // TODO rewrite to std::variant
public:
	Lexem type;
	std::string sInfo;
	int posInFile;
	int iInfo;
	double dInfo;
	Lex(Lexem _type, int _posInFile);
	Lex(Lexem _type, int _posInFile, const std::string &s);
	Lex(Lexem _type, int _posInFile, int i);
	Lex(Lexem _type, int _posInFile, double d);

	bool operator==(Lexem l)const;
};

class LexException {
public:
	std::string str;
	int index;
	LexException(std::string _str, int _index);
};

class Lexer {
	std::vector<char> ds;
	std::vector<Lex> res;
	int index;
public:
	Lexer(const std::string & text);
	inline char& n();

	inline std::string n(int count);

	bool skipSpace();

	bool skipComment();

	bool skipMultiLineComment();

	void skip();

	bool nextIs(const std::string & s);
	bool checkAll(std::vector<std::pair<const std::string, Lexem>> l);

	double getInt();

	std::string getWord();

	std::vector<Lex> proc();
};