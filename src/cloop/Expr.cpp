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


IntLiteralExpr::IntLiteralExpr(int value)
	: value(value)
{
}

string IntLiteralExpr::generate(Language language)
{
	char buffer[64];
	sprintf(buffer, "%d", value);
	return buffer;
}


//--------------------------------------


NegateExpr::NegateExpr(Expr* expr)
	: expr(expr)
{
}

std::string NegateExpr::generate(Language language)
{
	return "-" + expr->generate(language);
}


//--------------------------------------


ConstantExpr::ConstantExpr(Interface* interface, string name)
	: interface(interface),
	  name(name)
{
}

string ConstantExpr::generate(Language language)
{
	//// TODO: LANGUAGE_PASCAL
	return (language == LANGUAGE_C ? interface->name + "_" : "") + name;
}


//--------------------------------------


BitwiseOrExpr::BitwiseOrExpr(Expr* expr1, Expr* expr2)
	: expr1(expr1),
	  expr2(expr2)
{
}

string BitwiseOrExpr::generate(Language language)
{
	return expr1->generate(language) + " | " + expr2->generate(language);
}
