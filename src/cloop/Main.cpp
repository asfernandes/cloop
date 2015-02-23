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
	if (argc < 4)
		throw runtime_error("Invalid command line parameters.");

	string inFilename(argv[1]);
	string outFormat(argv[2]);
	string outFilename(argv[3]);

	Lexer lexer(inFilename);

	Parser parser(&lexer);
	parser.parse();

	auto_ptr<Generator> generator;

	if (outFormat == "c++")
	{
		if (argc < 7)
			throw runtime_error("Invalid command line parameters for C++ output.");

		string headerGuard(argv[4]);
		string className(argv[5]);
		string prefix(argv[6]);

		generator.reset(new CppGenerator(outFilename, prefix, &parser, headerGuard, className));
	}
	else if (outFormat == "c-header")
	{
		if (argc < 6)
			throw runtime_error("Invalid command line parameters for C header output.");

		string headerGuard(argv[4]);
		string prefix(argv[5]);

		generator.reset(new CHeaderGenerator(outFilename, prefix, &parser, headerGuard));
	}
	else if (outFormat == "c-impl")
	{
		if (argc < 6)
			throw runtime_error("Invalid command line parameters for C implementation output.");

		string includeFilename(argv[4]);
		string prefix(argv[5]);

		generator.reset(new CImplGenerator(outFilename, prefix, &parser, includeFilename));
	}
	else if (outFormat == "pascal")
	{
		if (argc < 5)
			throw runtime_error("Invalid command line parameters for Pascal output.");

		string unitName(argv[4]);
		string additionalUses(argc > 5 ? argv[5] : "");
		string interfaceFile(argc > 6 ? argv[6] : "");
		string implementationFile(argc > 7 ? argv[7] : "");
		string exceptionClass(argc > 8 ? argv[8] : "");
		string prefix;	//// TODO:

		generator.reset(new PascalGenerator(outFilename, prefix, &parser, unitName,
			additionalUses, interfaceFile, implementationFile, exceptionClass));
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
