#ifndef CLOOP_PARSER_H
#define CLOOP_PARSER_H

#include "Lexer.h"
#include <map>
#include <string>
#include <vector>


class Expr;


class Type
{
public:
	Type()
		: isConst(false),
		  isPointer(false)
	{
	}

	Token token;
	bool isConst;
	bool isPointer;
};


class Parameter
{
public:
	std::string name;
	Type type;
};


class Constant
{
public:
	std::string name;
	Type type;
	Expr* expr;
};


class Method
{
public:
	Method()
		: isConst(false)
	{
	}

	std::string name;
	Type returnType;
	std::vector<Parameter*> parameters;
	bool isConst;
};


class Interface
{
public:
	Interface()
		: super(NULL)
	{
	}

	std::string name;
	Interface* super;
	std::vector<Constant*> constants;
	std::vector<Method*> methods;
};


class Parser
{
public:
	Parser(Lexer* lexer);

	void parse();
	void parseItem();
	void parseConstant(const Type& type, const std::string& name);
	void parseMethod(const Type& returnType, const std::string& name);

	Expr* parseExpr();
	Expr* parseLogicalExpr();
	Expr* parsePrimaryExpr();
	Expr* parseLiteralExpr();

private:
	Token& getToken(Token& token, Token::Type expected, bool allowEof = false);

	Type parseType();

	void syntaxError(const Token& token);
	void error(const Token& token, const std::string& msg);

public:
	std::vector<Interface*> interfaces;
	std::map<std::string, Interface*> interfacesByName;
	Interface* exceptionInterface;

private:
	Lexer* lexer;
	Token token;
	Interface* interface;
};


#endif	// CLOOP_PARSER_H
