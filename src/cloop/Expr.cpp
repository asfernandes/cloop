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
