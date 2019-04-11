#pragma once
#include <string>
#include "lexer.h"


bool fileExists(const char* name);

std::string toLower(const std::string& s);

std::string to_string(Lexem l);
