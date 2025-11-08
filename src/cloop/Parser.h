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
#include "Action.h"
#include <map>
#include <string>
#include <vector>


class Expr;


class BaseType
{
public:
	enum class Type
	{
		INTERFACE,
		STRUCT,
		TYPEDEF,
		BOOLEAN,
	};

protected:
	explicit BaseType(Type type)
		: type(type)
	{
	}

	virtual ~BaseType() = default;

public:
	Type type;
	std::string name;
};


class TypeRef final
{
public:
	explicit TypeRef() = default;

public:
	bool valueIsPointer();

public:
	Token token;
	bool isConst = false;
	bool isPointer = false;
	BaseType::Type type = BaseType::Type::INTERFACE;
};


class Parameter final
{
public:
	std::string name;
	TypeRef typeRef;
};


class Constant final
{
public:
	std::string name;
	TypeRef typeRef;
	Expr* expr = nullptr;
};


class Method final
{
public:
	explicit Method() = default;

	std::string name;
	TypeRef returnTypeRef;
	std::vector<Parameter*> parameters;
	Expr* notImplementedExpr = nullptr;
	Action* notImplementedAction = nullptr;
	Action* stubAction = nullptr;
	unsigned version = 0;
	bool isConst = false;
	std::string onErrorFunction;
	std::string statusName;
};


class Interface final : public BaseType
{
public:
	explicit Interface()
		: BaseType(Type::INTERFACE)
	{
	}

public:
	Interface* super = nullptr;
	std::vector<Constant*> constants;
	std::vector<Method*> methods;
	unsigned version = 1;
};


class Struct final : public BaseType
{
public:
	explicit Struct()
		: BaseType(Type::STRUCT)
	{
	}
};


class Boolean final : public BaseType
{
public:
	explicit Boolean()
		: BaseType(Type::BOOLEAN)
	{
	}
};


class Typedef final : public BaseType
{
public:
	explicit Typedef()
		: BaseType(Type::TYPEDEF)
	{
	}
};


class Parser final
{
public:
	explicit Parser(Lexer* lexer);

	void parse();
	void parseInterface(bool exception);
	void parseStruct();
	void parseTypedef();
	void parseBoolean();
	void parseItem();
	void parseConstant(const TypeRef& typeRef, const std::string& name);
	void parseMethod(const TypeRef& returnTypeRef, const std::string& name, Expr* notImplementedExpr,
		const std::string& onErrorFunction, Action* notImplementedAction, Action* stubAction);

	Expr* parseExpr();
	Expr* parseLogicalExpr();
	Expr* parseUnaryExpr();
	Expr* parsePrimaryExpr();

	Action* parseAction(DefAction::DefType dt);
	Action* parseIfThenElseAction(DefAction::DefType dt);
	Action* parseCallAction();
	Action* parseDefAction(DefAction::DefType dt);

private:
	void checkType(TypeRef& typeRef);

	Token& getToken(Token& token, Token::Type expected, bool allowEof = false);

	TypeRef parseTypeRef();

	[[noreturn]] void syntaxError(const Token& token);
	[[noreturn]] void error(const Token& token, const std::string& msg);

public:
	std::vector<Interface*> interfaces;
	std::map<std::string, BaseType*> typesByName;
	Interface* exceptionInterface = nullptr;

private:
	Lexer* lexer;
	Token token;
	Interface* interface = nullptr;
};


#endif  // CLOOP_PARSER_H
