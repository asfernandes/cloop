#include "Lexer.h"
#include <stdexcept>

using std::runtime_error;
using std::string;


//--------------------------------------


Lexer::Lexer(const string& filename)
	: filename(filename),
	  line(1),
	  column(1)
{
	in = fopen(filename.c_str(), "r");

	if (!in)
		throw runtime_error(string("Input file not found: ") + filename + ".");
}

Lexer::~Lexer()
{
	fclose(in);
}

Token& Lexer::getToken(Token& token)
{
	if (!tokens.empty())
	{
		token = tokens.top();
		tokens.pop();
		return token;
	}

	token.text = "";

	Char ch;
	skip(ch);

	token.line = ch.line;
	token.column = ch.column;

	if (ch.c == -1)
	{
		token.type = Token::TYPE_EOF;
		return token;
	}
	else if ((ch.c >= 'a' && ch.c <= 'z') || (ch.c >= 'A' && ch.c <= 'Z') || ch.c == '_')
	{
		while ((ch.c >= 'a' && ch.c <= 'z') || (ch.c >= 'A' && ch.c <= 'Z') || ch.c == '_' ||
			   (ch.c >= '0' && ch.c <= '9'))
		{
			token.text += ch.c;
			getChar(ch);
		}

		ungetChar(ch);

		if (token.text == "const")
			token.type = Token::TYPE_CONST;
		else if (token.text == "exception")
			token.type = Token::TYPE_EXCEPTION;
		else if (token.text == "interface")
			token.type = Token::TYPE_INTERFACE;
		else if (token.text == "void")
			token.type = Token::TYPE_VOID;
		else if (token.text == "int")
			token.type = Token::TYPE_INT;
		else
			token.type = Token::TYPE_IDENTIFIER;
	}
	else if (ch.c >= '0' && ch.c <= '9')
	{
		token.type = Token::TYPE_INT_LITERAL;

		while (ch.c >= '0' && ch.c <= '9')
		{
			token.text += ch.c;
			getChar(ch);
		}

		ungetChar(ch);
	}
	else
	{
		token.type = static_cast<Token::Type>(ch.c);
		token.text = ch.c;
	}

	return token;
}

void Lexer::pushToken(const Token& token)
{
	tokens.push(token);
}

void Lexer::skip(Char& ch)	// skip spaces and comments
{
	while (true)
	{
		while (getChar(ch).c == ' ' || ch.c == '\t' || ch.c == '\r' || ch.c == '\n')
			;

		// check for comments

		if (ch.c != '/')
			return;

		Char firstCh = ch;
		getChar(ch);

		switch (ch.c)
		{
			case '*':
			{
				bool inComment = true;

				while (inComment)
				{
					while (getChar(ch).c != '*' && ch.c != -1)
						;

					if (ch.c == -1)
					{
						char buffer[1024];
						sprintf(buffer, "%s:%i:%i: error: Unterminated comment.",
							filename.c_str(),
							firstCh.line, firstCh.column);
						throw runtime_error(buffer);
					}
					else
					{
						getChar(ch);

						if (ch.c == '/')
							inComment = false;
						else
							ungetChar(ch);
					}
				}

				break;
			}

			case '/':
				while (getChar(ch).c != '\n' && ch.c != -1)
					;

				break;

			default:	// not a comment
				ch = firstCh;
				break;
		}
	}

	// should never be here
}

Lexer::Char& Lexer::getChar(Char& ch)
{
	ch.c = fgetc(in);
	ch.line = line;
	ch.column = column;

	if (ch.c == '\n')
	{
		++line;
		column = 1;
	}
	else
		++column;

	return ch;
}

void Lexer::ungetChar(const Char& ch)
{
	ungetc(ch.c, in);
	line = ch.line;
	column = ch.column;
}
