#include "Lexer.h"
#include "Parser.h"
#include "Expr.h"
#include "Generator.h"
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

using std::auto_ptr;
using std::cerr;
using std::endl;
using std::exception;
using std::string;
using std::runtime_error;


//--------------------------------------


static void run(int argc, const char* argv[])
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
	catch (exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}
}
