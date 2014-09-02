/*
 *  The contents of this file are subject to the Initial
 *  Developer's Public License Version 1.0 (the "License");
 *  you may not use this file except in compliance with the
 *  License. You may obtain a copy of the License at
 *  http://www.ibphoenix.com/main.nfs?a=ibphoenix&page=ibp_idpl.
 *
 *  Software distributed under the License is distributed AS IS,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied.
 *  See the License for the specific language governing rights
 *  and limitations under the License.
 *
 *  The Original Code was created by Adriano dos Santos Fernandes.
 *
 *  Copyright (c) 2014 Adriano dos Santos Fernandes <adrianosf at gmail.com>
 *  and all contributors signed below.
 *
 *  All Rights Reserved.
 *  Contributor(s): ______________________________________.
 */

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
		  isPointer(false),
		  isStruct(false)
	{
	}

	Token token;
	bool isConst;
	bool isPointer;
	bool isStruct;
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


class Struct
{
public:
	std::string name;
};


class Parser
{
public:
	Parser(Lexer* lexer);

	void parse();
	void parseInterface(bool exception);
	void parseStruct();
	void parseItem();
	void parseConstant(const Type& type, const std::string& name);
	void parseMethod(const Type& returnType, const std::string& name);

	Expr* parseExpr();
	Expr* parseLogicalExpr();
	Expr* parseUnaryExpr();
	Expr* parsePrimaryExpr();

private:
	void checkType(Type& type);

	Token& getToken(Token& token, Token::Type expected, bool allowEof = false);

	Type parseType();

	void syntaxError(const Token& token);
	void error(const Token& token, const std::string& msg);

public:
	std::vector<Interface*> interfaces;
	std::map<std::string, Interface*> interfacesByName;
	std::map<std::string, Struct*> structsByName;
	Interface* exceptionInterface;

private:
	Lexer* lexer;
	Token token;
	Interface* interface;
};


#endif	// CLOOP_PARSER_H
