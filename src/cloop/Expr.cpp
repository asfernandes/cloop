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

#include "Expr.h"
#include "Parser.h"
#include <stdio.h>

using std::string;


//--------------------------------------


IntLiteralExpr::IntLiteralExpr(int value, bool hex)
	: value(value), hex(hex)
{
}

string IntLiteralExpr::generate(Language language, const string& prefix)
{
	char buffer[128];

	if (language == LANGUAGE_JSON)		// TODO: Does json support hex constants?
		sprintf(buffer, "{ \"type\": \"int-literal\", \"value\": %d }", value);
	else
	{
		if (hex)
			sprintf(buffer, "%s%x", language == LANGUAGE_PASCAL ? "$" : "0x", value);
		else
			sprintf(buffer, "%d", value);
	}

	return buffer;
}


//--------------------------------------


BooleanLiteralExpr::BooleanLiteralExpr(bool value)
	: value(value)
{
}

string BooleanLiteralExpr::generate(Language language, const string& prefix)
{
	if (language == LANGUAGE_JSON)
	{
		char buffer[64];
		sprintf(buffer, "{ \"type\": \"boolean-literal\", \"value\": %s }",
			(value ? "true" : "false"));
		return buffer;
	}
	else
		return value ? "true" : "false";
}


//--------------------------------------


NegateExpr::NegateExpr(Expr* expr)
	: expr(expr)
{
}

std::string NegateExpr::generate(Language language, const string& prefix)
{
	if (language == LANGUAGE_JSON)
		return "{ \"type\": \"-\", \"args\": [ " + expr->generate(language, prefix) + " ] }";
	else
		return "-" + expr->generate(language, prefix);
}


//--------------------------------------


ConstantExpr::ConstantExpr(Interface* interface, string name)
	: interface(interface),
	  name(name)
{
}

string ConstantExpr::generate(Language language, const string& prefix)
{
	string retPrefix;

	switch (language)
	{
		case LANGUAGE_C:
			retPrefix = prefix + interface->name + "_";
			break;

		case LANGUAGE_CPP:
			retPrefix = prefix + interface->name + "::";
			break;

		case LANGUAGE_PASCAL:
			retPrefix = prefix + interface->name + "Impl" + ".";
			break;

		case LANGUAGE_JAVA:
			retPrefix = prefix + interface->name + "Intf.";
			break;

		case LANGUAGE_JSON:
			return "{ \"type\": \"constant\", \"interface\": \"" + interface->name +
				"\", \"name\": \"" + name + "\" }";
	}

	return retPrefix + name;
}


//--------------------------------------


BitwiseOrExpr::BitwiseOrExpr(Expr* expr1, Expr* expr2)
	: expr1(expr1),
	  expr2(expr2)
{
}

string BitwiseOrExpr::generate(Language language, const string& prefix)
{
	if (language == LANGUAGE_JSON)
	{
		return "{ \"type\": \"|\", \"args\": [ " +
			expr1->generate(language, prefix) + ", " +
			expr2->generate(language, prefix) + " ] }";
	}
	else
	{
		return expr1->generate(language, prefix) +
			(language == LANGUAGE_PASCAL ? " or " : " | ") +
			expr2->generate(language, prefix);
	}
}
