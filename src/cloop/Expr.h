#ifndef CLOOP_EXPR_H
#define CLOOP_EXPR_H

#include <string>


class Interface;


enum Language
{
	LANGUAGE_C,
	LANGUAGE_CPP,
	LANGUAGE_PASCAL
};


class Expr
{
public:
	virtual ~Expr()
	{
	}

public:
	virtual std::string generate(Language language) = 0;
};


class IntLiteralExpr : public Expr
{
public:
	IntLiteralExpr(int value);

public:
	virtual std::string generate(Language language);

private:
	int value;
};


class ConstantExpr : public Expr
{
public:
	ConstantExpr(Interface* interface, std::string name);

public:
	virtual std::string generate(Language language);

private:
	Interface* interface;
	std::string name;
};


class BitwiseOrExpr : public Expr
{
public:
	BitwiseOrExpr(Expr* expr1, Expr* expr2);

public:
	virtual std::string generate(Language language);

private:
	Expr* expr1;
	Expr* expr2;
};


#endif	// CLOOP_EXPR_H
