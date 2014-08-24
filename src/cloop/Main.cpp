#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>
#include <stdio.h>

using std::auto_ptr;
using std::deque;
using std::map;
using std::pair;
using std::stack;
using std::string;
using std::vector;
using std::runtime_error;
using std::cerr;
using std::endl;


#define DUMMY_VTABLE	1
#define DUMMY_INSTANCE	1
#define TOKEN(c)	static_cast< ::Token::Type>(c)


class Token
{
public:
	enum Type
	{
		TYPE_EOF = 256,
		TYPE_IDENTIFIER,
		// keywords
		TYPE_EXCEPTION,
		TYPE_INTERFACE,
		// types
		TYPE_VOID,
		TYPE_INT
	};

public:
	Type type;
	string text;
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
	Lexer(const string& filename)
		: filename(filename),
		  line(1),
		  column(1)
	{
		in = fopen(filename.c_str(), "r");

		if (!in)
			throw runtime_error(string("Input file not found: ") + filename + ".");
	}

	~Lexer()
	{
		fclose(in);
	}

public:
	Token& getToken(Token& token)
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

			if (token.text == "exception")
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
		else
		{
			token.type = static_cast<Token::Type>(ch.c);
			token.text = ch.c;
		}

		return token;
	}

	void pushToken(const Token& token)
	{
		tokens.push(token);
	}

private:
	void skip(Char& ch)	// skip spaces and comments
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

	Char& getChar(Char& ch)
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

	void ungetChar(const Char& ch)
	{
		ungetc(ch.c, in);
		line = ch.line;
		column = ch.column;
	}

public:
	const string filename;

private:
	FILE* in;
	unsigned line, column;
	stack<Token> tokens;
};


struct Parameter
{
	string name;
	Token type;
};


struct Method
{
	string name;
	Token returnType;
	vector<Parameter*> parameters;
};


struct Interface
{
	Interface()
		: super(NULL)
	{
	}

	string name;
	Interface* super;
	vector<Method*> methods;
};


class Parser
{
public:
	Parser(Lexer* lexer)
		: exceptionInterface(NULL),
		  lexer(lexer)
	{
	}

	void parse()
	{
		Token token;

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

			Interface* interface = new Interface();
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

				Method* method = new Method();
				interface->methods.push_back(method);

				method->returnType = parseType(token);
				method->name = getToken(token, Token::TYPE_IDENTIFIER).text;
				getToken(token, TOKEN('('));

				if (lexer->getToken(token).type != TOKEN(')'))
				{
					lexer->pushToken(token);

					while (true)
					{
						Parameter* parameter = new Parameter();
						method->parameters.push_back(parameter);

						parameter->type = parseType(token);
						parameter->name = getToken(token, Token::TYPE_IDENTIFIER).text;

						lexer->getToken(token);
						lexer->pushToken(token);

						if (token.type == TOKEN(')'))
							break;

						getToken(token, TOKEN(','));
					}

					getToken(token, TOKEN(')'));
				}

				getToken(token, TOKEN(';'));
			}
		}
	}

private:
	Token& getToken(Token& token, Token::Type expected, bool allowEof = false)
	{
		lexer->getToken(token);

		if (token.type != expected && !(allowEof && token.type == Token::TYPE_EOF))
			error(token, string("Syntax error at '") + token.text + "'.");

		return token;
	}

	Token& parseType(Token& token)
	{
		lexer->getToken(token);

		switch (token.type)
		{
			case Token::TYPE_VOID:
			case Token::TYPE_INT:
			case Token::TYPE_IDENTIFIER:
				break;

			default:
				error(token, string("Syntax error at '") + token.text + "'. Expected a type.");
		}

		return token;
	}

	void error(const Token& token, const string& msg)
	{
		char buffer[1024];
		sprintf(buffer, "%s:%i:%i: error: %s",
			lexer->filename.c_str(), token.line, token.column, msg.c_str());
		throw runtime_error(buffer);
	}

public:
	vector<Interface*> interfaces;
	map<string, Interface*> interfacesByName;
	Interface* exceptionInterface;

private:
	Lexer* lexer;
};


class Generator
{
public:
	virtual ~Generator()
	{
	}

	virtual void generate() = 0;

protected:
	static const char* const AUTOGEN_MSG;
};

const char* const Generator::AUTOGEN_MSG =
	"This file was autogenerated by cloop - Cross Language Object Oriented Programming";


class FileGenerator : public Generator
{
public:
	FileGenerator(const string& filename)
	{
		out = fopen(filename.c_str(), "w+");
	}

	virtual ~FileGenerator()
	{
		fclose(out);
	}

protected:
	FILE* out;
};


class CBasedGenerator : public FileGenerator
{
protected:
	CBasedGenerator(const string& filename, bool cPlusPlus)
		: FileGenerator(filename),
		  cPlusPlus(cPlusPlus)
	{
	}

protected:
	string convertType(const Token& token)
	{
		switch (token.type)
		{
			case Token::TYPE_IDENTIFIER:
				return string(cPlusPlus ? "" : "struct ") + token.text + "*";

			default:
				return token.text;
		}
	}

private:
	bool cPlusPlus;
};


class CppGenerator : public CBasedGenerator
{
public:
	CppGenerator(const string& filename, Parser* parser, const string& headerGuard,
			const string& className)
		: CBasedGenerator(filename, true),
		  parser(parser),
		  headerGuard(headerGuard),
		  className(className)
	{
	}

public:
	virtual void generate()
	{
		fprintf(out, "// %s\n\n", AUTOGEN_MSG);

		fprintf(out, "#ifndef %s\n", headerGuard.c_str());
		fprintf(out, "#define %s\n\n", headerGuard.c_str());
		fprintf(out, "#include <stdint.h>\n\n\n");

		fprintf(out, "template <typename Policy>\n");
		fprintf(out, "class %s\n", className.c_str());
		fprintf(out, "{\n");
		fprintf(out, "public:\n");

		fprintf(out, "\t// Interfaces declarations\n\n");

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;

			deque<Method*> methods;

			for (Interface* p = interface; p; p = p->super)
				methods.insert(methods.begin(), p->methods.begin(), p->methods.end());

			if (!interface->super)
				fprintf(out, "\tclass %s\n", interface->name.c_str());
			else
			{
				fprintf(out, "\tclass %s : public %s\n",
					interface->name.c_str(), interface->super->name.c_str());
			}

			fprintf(out, "\t{\n");
			fprintf(out, "\tprotected:\n");

			if (!interface->super)
			{
				fprintf(out, "\t\tstruct VTable\n");
				fprintf(out, "\t\t{\n");
				fprintf(out, "\t\t\tvoid* cloopDummy[%d];\n", DUMMY_VTABLE);
				fprintf(out, "\t\t\tuintptr_t version;\n");
			}
			else
			{
				fprintf(out, "\t\tstruct VTable : public %s::VTable\n",
					interface->super->name.c_str());
				fprintf(out, "\t\t{\n");
			}

			for (vector<Method*>::iterator j = interface->methods.begin();
				 j != interface->methods.end();
				 ++j)
			{
				Method* method = *j;

				fprintf(out, "\t\t\t%s (*%s)(%s* self",
					convertType(method->returnType).c_str(), method->name.c_str(),
					interface->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					fprintf(out, ", %s %s",
						convertType(parameter->type).c_str(), parameter->name.c_str());
				}

				fprintf(out, ");\n");
			}

			fprintf(out, "\t\t};\n");
			fprintf(out, "\n");

			if (!interface->super)
			{
				fprintf(out, "\tprotected:\n");
				fprintf(out, "\t\tvoid* cloopDummy[%d];\n", DUMMY_INSTANCE);
				fprintf(out, "\t\tVTable* cloopVTable;\n");
				fprintf(out, "\n");
			}

			fprintf(out, "\tpublic:\n");
			fprintf(out, "\t\tstatic const int VERSION = %d;\n", (int) methods.size());

			unsigned methodNumber = (interface->super ? interface->super->methods.size() : 0);
			for (vector<Method*>::iterator j = interface->methods.begin();
				 j != interface->methods.end();
				 ++j)
			{
				Method* method = *j;

				fprintf(out, "\n");
				fprintf(out, "\t\t%s %s(",
					convertType(method->returnType).c_str(), method->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					if (k != method->parameters.begin())
						fprintf(out, ", ");

					fprintf(out, "%s %s",
						convertType(parameter->type).c_str(), parameter->name.c_str());
				}

				fprintf(out, ")\n");
				fprintf(out, "\t\t{\n");
				fprintf(out, "\t\t\tPolicy::template checkVersion<%d>(this);\n", ++methodNumber);

				fprintf(out, "\t\t\t");

				if (method->returnType.type != Token::TYPE_VOID)
				{
					fprintf(out, "%s ret = ", convertType(method->returnType).c_str());
					//// TODO: Policy::upgrade
				}

				fprintf(out, "static_cast<VTable*>(this->cloopVTable)->%s(this",
					method->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;
					fprintf(out, ", %s", parameter->name.c_str());
				}

				fprintf(out, ");\n");

				if (!method->parameters.empty() &&
					parser->exceptionInterface &&
					method->parameters.front()->type.text == parser->exceptionInterface->name)
				{
					fprintf(out, "\t\t\tPolicy::checkException(%s);\n",
						method->parameters.front()->name.c_str());
				}

				if (method->returnType.type != Token::TYPE_VOID)
					fprintf(out, "\t\t\treturn ret;\n");

				fprintf(out, "\t\t}\n");
			}

			fprintf(out, "\t};\n\n");
		}

		fprintf(out, "\t// Interfaces implementations\n");

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;

			deque<Method*> methods;

			for (Interface* p = interface; p; p = p->super)
				methods.insert(methods.begin(), p->methods.begin(), p->methods.end());

			fprintf(out, "\n");
			fprintf(out, "\ttemplate <typename Name, typename Base>\n");
			fprintf(out, "\tclass %sBaseImpl : public Base\n", interface->name.c_str());
			fprintf(out, "\t{\n");
			fprintf(out, "\tpublic:\n");
			fprintf(out, "\t\t%sBaseImpl()\n", interface->name.c_str());
			fprintf(out, "\t\t{\n");
			fprintf(out, "\t\t\tstatic struct VTableImpl : Base::VTable\n");
			fprintf(out, "\t\t\t{\n");
			fprintf(out, "\t\t\t\tVTableImpl()\n");
			fprintf(out, "\t\t\t\t{\n");
			fprintf(out, "\t\t\t\t\tthis->version = Base::VERSION;\n");

			for (deque<Method*>::iterator j = methods.begin(); j != methods.end(); ++j)
			{
				Method* method = *j;

				fprintf(out, "\t\t\t\t\tthis->%s = &Name::cloop%sDispatcher;\n",
					method->name.c_str(), method->name.c_str());
			}

			fprintf(out, "\t\t\t\t}\n");
			fprintf(out, "\t\t\t} vTable;\n");
			fprintf(out, "\n");

			fprintf(out, "\t\t\tthis->cloopVTable = &vTable;\n");
			fprintf(out, "\t\t}\n");

			// We generate all bases dispatchers so indirect overrides work. At the same time, we
			// inherit from all bases impls, so pure virtual methods are introduced and required to
			// be overriden in the user's implementation.

			for (Interface* p = interface; p; p = p->super)
			{
				for (vector<Method*>::iterator j = p->methods.begin(); j != p->methods.end(); ++j)
				{
					Method* method = *j;

					fprintf(out, "\n");
					fprintf(out, "\t\tstatic %s cloop%sDispatcher(%s* self",
						convertType(method->returnType).c_str(), method->name.c_str(),
						p->name.c_str());

					for (vector<Parameter*>::iterator k = method->parameters.begin();
						 k != method->parameters.end();
						 ++k)
					{
						Parameter* parameter = *k;

						fprintf(out, ", %s %s",
							convertType(parameter->type).c_str(), parameter->name.c_str());
					}

					fprintf(out, ") throw()\n");
					fprintf(out, "\t\t{\n");
					fprintf(out, "\t\t\ttry\n");
					fprintf(out, "\t\t\t{\n");

					fprintf(out, "\t\t\t\t");

					if (method->returnType.type != Token::TYPE_VOID)
						fprintf(out, "return ");

					fprintf(out, "static_cast<Name*>(self)->Name::%s(", method->name.c_str());

					for (vector<Parameter*>::iterator k = method->parameters.begin();
						 k != method->parameters.end();
						 ++k)
					{
						Parameter* parameter = *k;

						if (k != method->parameters.begin())
							fprintf(out, ", ");

						fprintf(out, "%s", parameter->name.c_str());
					}

					fprintf(out, ");\n");

					Parameter* exceptionParameter =
						(!method->parameters.empty() &&
						 parser->exceptionInterface &&
						 method->parameters.front()->type.text == parser->exceptionInterface->name
						) ? method->parameters.front() : NULL;

					fprintf(out, "\t\t\t}\n");
					fprintf(out, "\t\t\tcatch (...)\n");
					fprintf(out, "\t\t\t{\n");
					fprintf(out, "\t\t\t\tPolicy::catchException(%s);\n",
						(exceptionParameter ? exceptionParameter->name.c_str() : "0"));

					if (method->returnType.type != Token::TYPE_VOID)
					{
						fprintf(out, "\t\t\t\treturn static_cast<%s>(0);\n",
							convertType(method->returnType).c_str());
					}

					fprintf(out, "\t\t\t}\n");

					fprintf(out, "\t\t}\n");
				}
			}

			fprintf(out, "\t};\n\n");

			if (!interface->super)
			{
				fprintf(out, "\ttemplate <typename Name, typename Base = %s>\n",
					interface->name.c_str());
			}
			else
			{
				string base;
				unsigned baseCount = 0;

				for (Interface* p = interface->super; p; p = p->super)
				{
					base += p->name + "Impl<Name, ";
					++baseCount;
				}

				base += interface->name;

				while (baseCount-- > 0)
					base += "> ";

				fprintf(out, "\ttemplate <typename Name, typename Base = %s>\n", base.c_str());
			}

			fprintf(out, "\tclass %sImpl : public %sBaseImpl<Name, Base>\n",
				interface->name.c_str(), interface->name.c_str());
			fprintf(out, "\t{\n");
			fprintf(out, "\tpublic:\n");
			fprintf(out, "\t\tvirtual ~%sImpl()\n", interface->name.c_str());
			fprintf(out, "\t\t{\n");
			fprintf(out, "\t\t}\n");
			fprintf(out, "\n");

			for (vector<Method*>::iterator j = interface->methods.begin();
				 j != interface->methods.end();
				 ++j)
			{
				Method* method = *j;

				fprintf(out, "\t\tvirtual %s %s(",
					convertType(method->returnType).c_str(), method->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					if (k != method->parameters.begin())
						fprintf(out, ", ");

					fprintf(out, "%s %s",
						convertType(parameter->type).c_str(), parameter->name.c_str());
				}

				fprintf(out, ") = 0;\n");
			}

			fprintf(out, "\t};\n");
		}

		fprintf(out, "};\n\n\n");

		fprintf(out, "#endif\t// %s\n", headerGuard.c_str());
	}

private:
	Parser* parser;
	string headerGuard;
	string className;
};


class CHeaderGenerator : public CBasedGenerator
{
public:
	CHeaderGenerator(const string& filename, Parser* parser, const string& headerGuard)
		: CBasedGenerator(filename, false),
		  parser(parser),
		  headerGuard(headerGuard)
	{
	}

public:
	virtual void generate()
	{
		fprintf(out, "/* %s */\n\n", AUTOGEN_MSG);

		fprintf(out, "#ifndef %s\n", headerGuard.c_str());
		fprintf(out, "#define %s\n\n", headerGuard.c_str());
		fprintf(out, "#include <stdint.h>\n\n");

		fprintf(out, "#ifndef CLOOP_EXTERN_C\n");
		fprintf(out, "#ifdef __cplusplus\n");
		fprintf(out, "#define CLOOP_EXTERN_C extern \"C\"\n");
		fprintf(out, "#else\n");
		fprintf(out, "#define CLOOP_EXTERN_C\n");
		fprintf(out, "#endif\n");
		fprintf(out, "#endif\n\n\n");

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;

			deque<Method*> methods;

			for (Interface* p = interface; p; p = p->super)
				methods.insert(methods.begin(), p->methods.begin(), p->methods.end());

			fprintf(out, "#define %s_VERSION %d\n\n",
				interface->name.c_str(), (int) methods.size());

			fprintf(out, "struct %s;\n\n", interface->name.c_str());

			fprintf(out, "struct %sVTable\n", interface->name.c_str());
			fprintf(out, "{\n");
			fprintf(out, "\tvoid* cloopDummy[%d];\n", DUMMY_VTABLE);
			fprintf(out, "\tuintptr_t version;\n");

			for (deque<Method*>::iterator j = methods.begin(); j != methods.end(); ++j)
			{
				Method* method = *j;

				fprintf(out, "\t%s (*%s)(struct %s* self",
					convertType(method->returnType).c_str(), method->name.c_str(),
					interface->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					fprintf(out, ", %s %s", convertType(parameter->type).c_str(),
						parameter->name.c_str());
				}

				fprintf(out, ");\n");
			}

			fprintf(out, "};\n\n");

			fprintf(out, "struct %s\n", interface->name.c_str());
			fprintf(out, "{\n");
			fprintf(out, "\tvoid* cloopDummy[%d];\n", DUMMY_INSTANCE);
			fprintf(out, "\tstruct %sVTable* vtable;\n", interface->name.c_str());
			fprintf(out, "};\n\n");

			for (deque<Method*>::iterator j = methods.begin(); j != methods.end(); ++j)
			{
				Method* method = *j;

				fprintf(out, "CLOOP_EXTERN_C %s %s_%s(struct %s* self",
					convertType(method->returnType).c_str(),
					interface->name.c_str(),
					method->name.c_str(),
					interface->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					fprintf(out, ", %s %s",
						convertType(parameter->type).c_str(), parameter->name.c_str());
				}

				fprintf(out, ");\n");
			}

			fprintf(out, "\n");
		}

		fprintf(out, "\n");
		fprintf(out, "#endif\t// %s\n", headerGuard.c_str());
	}

private:
	Parser* parser;
	string headerGuard;
};


class CImplGenerator : public CBasedGenerator
{
public:
	CImplGenerator(const string& filename, Parser* parser, const string& includeFilename)
		: CBasedGenerator(filename, false),
		  parser(parser),
		  includeFilename(includeFilename)
	{
	}

public:
	virtual void generate()
	{
		fprintf(out, "/* %s */\n\n", AUTOGEN_MSG);

		fprintf(out, "#include \"%s\"\n\n\n", includeFilename.c_str());

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;

			deque<Method*> methods;

			for (Interface* p = interface; p; p = p->super)
				methods.insert(methods.begin(), p->methods.begin(), p->methods.end());

			for (deque<Method*>::iterator j = methods.begin(); j != methods.end(); ++j)
			{
				Method* method = *j;

				fprintf(out, "CLOOP_EXTERN_C %s %s_%s(struct %s* self",
					convertType(method->returnType).c_str(),
					interface->name.c_str(),
					method->name.c_str(),
					interface->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					fprintf(out, ", %s %s",
						convertType(parameter->type).c_str(), parameter->name.c_str());
				}

				fprintf(out, ")\n");
				fprintf(out, "{\n");
				fprintf(out, "\t");

				if (method->returnType.type != Token::TYPE_VOID)
					fprintf(out, "return ");

				fprintf(out, "self->vtable->%s(self", method->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;
					fprintf(out, ", %s", parameter->name.c_str());
				}

				fprintf(out, ");\n");
				fprintf(out, "}\n\n");
			}
		}
	}

private:
	Parser* parser;
	string includeFilename;
};


class PascalGenerator : public FileGenerator
{
public:
	PascalGenerator(const string& filename, Parser* parser, const string& unitName)
		: FileGenerator(filename),
		  parser(parser),
		  unitName(unitName)
	{
	}

public:
	virtual void generate()
	{
		fprintf(out, "{ %s }\n\n", AUTOGEN_MSG);

		fprintf(out, "unit %s;\n\n", unitName.c_str());
		fprintf(out, "interface\n\n");
		fprintf(out, "uses Classes;\n\n");
		fprintf(out, "type\n");

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;
			fprintf(out, "\t%s = class;\n", interface->name.c_str());
		}

		fprintf(out, "\n");

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;

			for (vector<Method*>::iterator j = interface->methods.begin();
				 j != interface->methods.end();
				 ++j)
			{
				Method* method = *j;

				fprintf(out, "\t%s_%sPtr = %s(this: %s",
					interface->name.c_str(), method->name.c_str(),
					(method->returnType.type == Token::TYPE_VOID ? "procedure" : "function"),
					interface->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					fprintf(out, "; %s: %s",
						parameter->name.c_str(), convertType(parameter->type).c_str());
				}

				fprintf(out, ")");

				if (method->returnType.type != Token::TYPE_VOID)
					fprintf(out, ": %s", convertType(method->returnType).c_str());

				fprintf(out, "; cdecl;\n");
			}
		}

		fprintf(out, "\n");

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;

			fprintf(out, "\t%sVTable = class", interface->name.c_str());

			if (interface->super)
				fprintf(out, "(%sVTable)", interface->super->name.c_str());

			fprintf(out, "\n");

			if (!interface->super)
			{
				fprintf(out, "{$ifndef FPC}\n");
				fprintf(out, "\t\tdummy: PtrInt;\n");
				fprintf(out, "{$endif}\n");
				fprintf(out, "\t\tversion: PtrInt;\n");
			}

			for (vector<Method*>::iterator j = interface->methods.begin();
				 j != interface->methods.end();
				 ++j)
			{
				Method* method = *j;

				fprintf(out, "\t\t%s: %s_%sPtr;\n", method->name.c_str(), interface->name.c_str(),
					method->name.c_str());
			}

			fprintf(out, "\tend;\n\n");

			fprintf(out, "\t%s = class", interface->name.c_str());

			if (interface->super)
				fprintf(out, "(%s)", interface->super->name.c_str());

			fprintf(out, "\n");

			if (!interface->super)
			{
				fprintf(out, "{$ifndef FPC}\n");
				fprintf(out, "\t\tdummy: PtrInt;\n");
				fprintf(out, "{$endif}\n");
				fprintf(out, "\t\tvTable: %sVTable;\n", interface->name.c_str());
			}

			for (vector<Method*>::iterator j = interface->methods.begin();
				 j != interface->methods.end();
				 ++j)
			{
				Method* method = *j;

				fprintf(out, "\t\t%s %s(",
					(method->returnType.type == Token::TYPE_VOID ? "procedure" : "function"),
					method->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					if (k != method->parameters.begin())
						fprintf(out, "; ");

					fprintf(out, "%s: %s",
						parameter->name.c_str(), convertType(parameter->type).c_str());
				}

				fprintf(out, ")");

				if (method->returnType.type != Token::TYPE_VOID)
					fprintf(out, ": %s", convertType(method->returnType).c_str());

				fprintf(out, ";\n");
			}

			fprintf(out, "\tend;\n\n");

			fprintf(out, "\t%sImpl = class(%s)\n",
				interface->name.c_str(), interface->name.c_str());
			fprintf(out, "\t\tconstructor create;\n\n");

			deque<Method*> methods;

			for (Interface* p = interface; p; p = p->super)
				methods.insert(methods.begin(), p->methods.begin(), p->methods.end());

			for (deque<Method*>::iterator j = methods.begin(); j != methods.end(); ++j)
			{
				Method* method = *j;

				fprintf(out, "\t\t%s %s(",
					(method->returnType.type == Token::TYPE_VOID ? "procedure" : "function"),
					method->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					if (k != method->parameters.begin())
						fprintf(out, "; ");

					fprintf(out, "%s: %s",
						parameter->name.c_str(), convertType(parameter->type).c_str());
				}

				fprintf(out, ")");

				if (method->returnType.type != Token::TYPE_VOID)
					fprintf(out, ": %s", convertType(method->returnType).c_str());

				fprintf(out, "; virtual; abstract;\n");
			}

			fprintf(out, "\tend;\n\n");
		}

		fprintf(out, "implementation\n\n");

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;

			for (vector<Method*>::iterator j = interface->methods.begin();
				 j != interface->methods.end();
				 ++j)
			{
				Method* method = *j;

				fprintf(out, "%s %s.%s(",
					(method->returnType.type == Token::TYPE_VOID ? "procedure" : "function"),
					interface->name.c_str(),
					method->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					if (k != method->parameters.begin())
						fprintf(out, "; ");

					fprintf(out, "%s: %s",
						parameter->name.c_str(), convertType(parameter->type).c_str());
				}

				fprintf(out, ")");

				if (method->returnType.type != Token::TYPE_VOID)
					fprintf(out, ": %s", convertType(method->returnType).c_str());

				fprintf(out, ";\n");
				fprintf(out, "begin\n");
				fprintf(out, "\t");

				if (method->returnType.type != Token::TYPE_VOID)
					fprintf(out, "Result := ");

				fprintf(out, "%sVTable(vTable).%s(Self",
					interface->name.c_str(), method->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;
					fprintf(out, ", %s", parameter->name.c_str());
				}

				fprintf(out, ");\n");
				fprintf(out, "end;\n\n");
			}
		}

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;

			deque<Method*> methods;

			for (Interface* p = interface; p; p = p->super)
				methods.insert(methods.begin(), p->methods.begin(), p->methods.end());

			for (deque<Method*>::iterator j = methods.begin(); j != methods.end(); ++j)
			{
				Method* method = *j;

				fprintf(out, "%s %sImpl_%sDispatcher(this: %s",
					(method->returnType.type == Token::TYPE_VOID ? "procedure" : "function"),
					interface->name.c_str(),
					method->name.c_str(),
					interface->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					fprintf(out, "; %s: %s",
						parameter->name.c_str(), convertType(parameter->type).c_str());
				}

				fprintf(out, ")");

				if (method->returnType.type != Token::TYPE_VOID)
					fprintf(out, ": %s", convertType(method->returnType).c_str());

				fprintf(out, "; cdecl;\n");
				fprintf(out, "begin\n");
				fprintf(out, "\t");

				if (method->returnType.type != Token::TYPE_VOID)
					fprintf(out, "Result := ");

				fprintf(out, "%sImpl(this).%s(", interface->name.c_str(), method->name.c_str());

				for (vector<Parameter*>::iterator k = method->parameters.begin();
					 k != method->parameters.end();
					 ++k)
				{
					Parameter* parameter = *k;

					if (k != method->parameters.begin())
						fprintf(out, ", ");

					fprintf(out, "%s", parameter->name.c_str());
				}

				fprintf(out, ");\n");
				fprintf(out, "end;\n\n");
			}

			fprintf(out, "var\n");
			fprintf(out, "\t%sImpl_vTable: %sVTable;\n\n",
				interface->name.c_str(), interface->name.c_str());

			fprintf(out, "constructor %sImpl.create;\n", interface->name.c_str());
			fprintf(out, "begin\n");
			fprintf(out, "\tvTable := %sImpl_vTable;\n", interface->name.c_str());
			fprintf(out, "end;\n\n");
		}

		fprintf(out, "initialization\n");

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;

			deque<Method*> methods;

			for (Interface* p = interface; p; p = p->super)
				methods.insert(methods.begin(), p->methods.begin(), p->methods.end());

			fprintf(out, "\t%sImpl_vTable := %sVTable.create;\n",
				interface->name.c_str(), interface->name.c_str());
			fprintf(out, "\t%sImpl_vTable.version := %d;\n",
				interface->name.c_str(), (int) methods.size());

			for (deque<Method*>::iterator j = methods.begin(); j != methods.end(); ++j)
			{
				Method* method = *j;

				fprintf(out, "\t%sImpl_vTable.%s := @%sImpl_%sDispatcher;\n",
					interface->name.c_str(),
					method->name.c_str(),
					interface->name.c_str(),
					method->name.c_str());
			}

			fprintf(out, "\n");
		}

		fprintf(out, "finalization\n");

		for (vector<Interface*>::iterator i = parser->interfaces.begin();
			 i != parser->interfaces.end();
			 ++i)
		{
			Interface* interface = *i;
			fprintf(out, "\t%sImpl_vTable.destroy;\n", interface->name.c_str());
		}

		fprintf(out, "\n");
		fprintf(out, "end.\n");
	}

private:
	string convertType(const Token& token)
	{
		switch (token.type)
		{
			case Token::TYPE_INT:
				return "Integer";

			default:
				return token.text;
		}
	}

private:
	Parser* parser;
	string unitName;
};


void run(int argc, const char* argv[])
{
	string inFilename(argv[1]);
	string outFormat(argv[2]);
	string outFilename(argv[3]);

	if (argc < 4)
		throw runtime_error("Invalid command line parameters.");

	Lexer lexer(inFilename);

	Parser parser(&lexer);
	parser.parse();

	auto_ptr<Generator> generator;

	if (outFormat == "c++")
	{
		if (argc < 6)
			throw runtime_error("Invalid command line parameters for C++ output.");

		string headerGuard(argv[4]);
		string className(argv[5]);

		generator.reset(new CppGenerator(outFilename, &parser, headerGuard, className));
	}
	else if (outFormat == "c-header")
	{
		if (argc < 5)
			throw runtime_error("Invalid command line parameters for C header output.");

		string headerGuard(argv[4]);

		generator.reset(new CHeaderGenerator(outFilename, &parser, headerGuard));
	}
	else if (outFormat == "c-impl")
	{
		if (argc < 5)
			throw runtime_error("Invalid command line parameters for C implementation output.");

		string includeFilename(argv[4]);

		generator.reset(new CImplGenerator(outFilename, &parser, includeFilename));
	}
	else if (outFormat == "pascal")
	{
		if (argc < 5)
			throw runtime_error("Invalid command line parameters for Pascal output.");

		string unitName(argv[4]);

		generator.reset(new PascalGenerator(outFilename, &parser, unitName));
	}
	else
		throw runtime_error("Invalid output format.");

	generator->generate();
}


int main(int argc, const char* argv[])
{
	try
	{
		run(argc, argv);
		return 0;
	}
	catch (std::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}
}
