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
	: value(value),
	  hex(hex)
{
}

string IntLiteralExpr::generate(Language language, const string& prefix)
{
	char buffer[128];

	if (language == Language::JSON)  // TODO: Does json support hex constants?
	{
		snprintf(buffer, sizeof(buffer), "{ \"type\": \"int-literal\", \"value\": %d }", value);
		return buffer;
	}

	if (hex)
		snprintf(buffer, sizeof(buffer), "%s%x", language == Language::PASCAL ? "$" : "0x", value);
	else
		snprintf(buffer, sizeof(buffer), "%d", value);

	return buffer;
}


//--------------------------------------


BooleanLiteralExpr::BooleanLiteralExpr(bool value)
	: value(value)
{
}

string BooleanLiteralExpr::generate(Language language, const string& prefix)
{
	if (language == Language::JSON)
	{
		char buffer[64];
		sprintf(buffer, "{ \"type\": \"boolean-literal\", \"value\": %s }", (value ? "true" : "false"));
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
	if (language == Language::JSON)
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

	if (interface)
	{
		switch (language)
		{
			case Language::C:
				retPrefix = prefix + interface->name + "_";
				break;

			case Language::CPP:
				retPrefix = prefix + interface->name + "::";
				break;

			case Language::PASCAL:
				retPrefix = prefix + interface->name + ".";
				break;

			case Language::JAVA:
				retPrefix = prefix + interface->name + "Intf.";
				break;

			case Language::JSON:
				return "{ \"type\": \"constant\", \"interface\": \"" + interface->name + "\", \"name\": \"" + name +
					"\" }";
		}
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
	if (language == Language::JSON)
	{
		return "{ \"type\": \"|\", \"args\": [ " + expr1->generate(language, prefix) + ", " +
			expr2->generate(language, prefix) + " ] }";
	}
	else
	{
		return expr1->generate(language, prefix) + (language == Language::PASCAL ? " or " : " | ") +
			expr2->generate(language, prefix);
	}
}
