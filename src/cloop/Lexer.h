#ifndef CLOOP_LEXER_H
#define CLOOP_LEXER_H

#include <stack>
#include <string>
#include <stdio.h>


#define TOKEN(c)	static_cast< ::Token::Type>(c)


struct Token
{
	enum Type
	{
		TYPE_EOF = 256,
		TYPE_IDENTIFIER,
		// literals
		TYPE_INT_LITERAL,
		// keywords
		TYPE_CONST,
		TYPE_EXCEPTION,
		TYPE_INTERFACE,
		// types
		TYPE_VOID,
		TYPE_INT
	};

	Type type;
	std::string text;
	unsigned line;
	unsigned column;
};


class Lexer
{
private:
	struct Char
	{
		int c;
		unsigned line;
		unsigned column;
	};

public:
	Lexer(const std::string& filename);
	~Lexer();

public:
	Token& getToken(Token& token);
	void pushToken(const Token& token);

private:
	void skip(Char& ch);
	Char& getChar(Char& ch);
	void ungetChar(const Char& ch);

public:
	const std::string filename;

private:
	FILE* in;
	unsigned line, column;
	std::stack<Token> tokens;
};


#endif	// CLOOP_LEXER_H
