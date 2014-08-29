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

#include "Parser.h"
#include "Expr.h"
#include <stdexcept>
#include <stdlib.h>

using std::map;
using std::pair;
using std::runtime_error;
using std::string;
using std::vector;


//--------------------------------------


Parser::Parser(Lexer* lexer)
	: exceptionInterface(NULL),
	  lexer(lexer),
	  interface(NULL)
{
}

void Parser::parse()
{
	interface = NULL;

	while (true)
	{
		bool exception = false;
		lexer->getToken(token);

		if (token.type == Token::TYPE_EOF)
			break;
		else if (token.type == TOKEN('['))
		{
			getToken(token, Token::TYPE_EXCEPTION);	// This is the only attribute we allow now.
			exception = true;
			getToken(token, TOKEN(']'));
		}
		else
			lexer->pushToken(token);

		getToken(token, Token::TYPE_INTERFACE);

		interface = new Interface();
		interfaces.push_back(interface);

		interface->name = getToken(token, Token::TYPE_IDENTIFIER).text;
		interfacesByName.insert(pair<string, Interface*>(interface->name, interface));

		if (exception)
			exceptionInterface = interface;

		if (lexer->getToken(token).type == TOKEN(':'))
		{
			string superName = getToken(token, Token::TYPE_IDENTIFIER).text;
			map<string, Interface*>::iterator it = interfacesByName.find(superName);

			if (it == interfacesByName.end())
				error(token, string("Super interface '") + superName + "' not found.");

			interface->super = it->second;
		}
		else
			lexer->pushToken(token);

		getToken(token, TOKEN('{'));

		while (lexer->getToken(token).type != TOKEN('}'))
		{
			lexer->pushToken(token);
			parseItem();
		}
	}
}

void Parser::parseItem()
{
	Type type(parseType());
	string name(getToken(token, Token::TYPE_IDENTIFIER).text);

	if (type.isConst)
	{
		if (lexer->getToken(token).type == TOKEN('='))
		{
			type.isConst = false;
			parseConstant(type, name);
			return;
		}
		else
			lexer->pushToken(token);
	}

	getToken(token, TOKEN('('));
	parseMethod(type, name);
}

void Parser::parseConstant(const Type& type, const string& name)
{
	Constant* constant = new Constant();
	interface->constants.push_back(constant);

	constant->type = type;
	constant->name = name;
	constant->expr = parseExpr();

	getToken(token, TOKEN(';'));
}

void Parser::parseMethod(const Type& returnType, const string& name)
{
	Method* method = new Method();
	interface->methods.push_back(method);

	method->returnType = returnType;
	method->name = name;

	if (lexer->getToken(token).type != TOKEN(')'))
	{
		lexer->pushToken(token);

		while (true)
		{
			Parameter* parameter = new Parameter();
			method->parameters.push_back(parameter);

			parameter->type = parseType();
			parameter->name = getToken(token, Token::TYPE_IDENTIFIER).text;

			lexer->getToken(token);
			lexer->pushToken(token);

			if (token.type == TOKEN(')'))
				break;

			getToken(token, TOKEN(','));
		}

		getToken(token, TOKEN(')'));
	}

	if (lexer->getToken(token).type == Token::TYPE_CONST)
		method->isConst = true;
	else
		lexer->pushToken(token);

	getToken(token, TOKEN(';'));
}

Expr* Parser::parseExpr()
{
	return parseLogicalExpr();
}

Expr* Parser::parseLogicalExpr()
{
	Expr* expr = parsePrimaryExpr();

	if (lexer->getToken(token).type == TOKEN('|'))
		expr = new BitwiseOrExpr(expr, parseExpr());
	else
		lexer->pushToken(token);

	return expr;
}

Expr* Parser::parsePrimaryExpr()
{
	lexer->getToken(token);

	switch (token.type)
	{
		case Token::TYPE_INT_LITERAL:
			return new IntLiteralExpr(atoi(token.text.c_str()));

		case Token::TYPE_IDENTIFIER:
			return new ConstantExpr(interface, token.text);

		default:
			syntaxError(token);
			return NULL;	// warning
	}
}

Token& Parser::getToken(Token& token, Token::Type expected, bool allowEof)
{
	lexer->getToken(token);

	if (token.type != expected && !(allowEof && token.type == Token::TYPE_EOF))
		syntaxError(token);

	return token;
}

Type Parser::parseType()
{
	Type type;
	lexer->getToken(type.token);

	if (type.token.type == Token::TYPE_CONST)
	{
		type.isConst = true;
		lexer->getToken(type.token);
	}

	switch (type.token.type)
	{
		case Token::TYPE_VOID:
		case Token::TYPE_INT:
		case Token::TYPE_IDENTIFIER:
			break;

		default:
			error(type.token, string("Syntax error at '") +
				type.token.text + "'. Expected a type.");
			break;
	}

	Token token2;
	lexer->getToken(token2);
	if (token2.type == TOKEN('*'))
		type.isPointer = true;
	else
		lexer->pushToken(token2);

	return type;
}

void Parser::syntaxError(const Token& token)
{
	error(token, string("Syntax error at '") + token.text + "'.");
}

void Parser::error(const Token& token, const string& msg)
{
	char buffer[1024];
	sprintf(buffer, "%s:%i:%i: error: %s",
		lexer->filename.c_str(), token.line, token.column, msg.c_str());
	throw runtime_error(buffer);
}
