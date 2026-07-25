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
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <stdexcept>

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::map;
using std::runtime_error;
using std::size_t;
using std::string;
using std::unique_ptr;


namespace
{
	struct CommandLineOptions
	{
		map<string, string> values;

		bool has(const char* name) const
		{
			return values.find(name) != values.end();
		}

		const string& get(const char* name) const
		{
			return values.at(name);
		}
	};
}  // namespace


//--------------------------------------


static string usage()
{
	return "Usage: cloop --input input-file --output-format format --output output-file [options]\n"
		   "Formats: c-header, c-impl, c++, pascal, jna, json\n"
		   "Common options:\n"
		   "  --input FILE\n"
		   "  --output-format FORMAT\n"
		   "  --output FILE\n"
		   "C++ options:\n"
		   "  --header-guard NAME --class-name NAME --prefix PREFIX\n"
		   "C header options:\n"
		   "  --header-guard NAME --prefix PREFIX [--macro]\n"
		   "C implementation options:\n"
		   "  --include-file FILE --prefix PREFIX\n"
		   "Pascal options:\n"
		   "  --unit-name NAME [--uses USES] [--interface-file FILE]\n"
		   "  [--implementation-file FILE] [--exception-class NAME]\n"
		   "  [--prefix PREFIX] [--functions-file FILE]\n"
		   "JNA options:\n"
		   "  --class-name NAME --exception-class NAME --prefix PREFIX\n"
		   "Other options:\n"
		   "  --help\n";
}

static bool isKnownOption(const string& name)
{
	static const char* const options[] = {
		"--input",
		"--output-format",
		"--output",
		"--header-guard",
		"--class-name",
		"--prefix",
		"--macro",
		"--include-file",
		"--unit-name",
		"--uses",
		"--interface-file",
		"--implementation-file",
		"--exception-class",
		"--functions-file",
		"--help",
	};

	for (const char* option : options)
	{
		if (name == option)
			return true;
	}

	return false;
}

static void addOption(CommandLineOptions& options, const string& name, const string& value)
{
	if (options.has(name.c_str()))
		throw runtime_error("Repeated option " + name + ".");

	options.values[name] = value;
}

static void parseCommandLine(int argc, const char* argv[], CommandLineOptions& options)
{
	for (int i = 1; i < argc; ++i)
	{
		string argument(argv[i]);

		if (argument == "--help")
		{
			addOption(options, argument, "");
			continue;
		}

		if (argument.size() < 3 || argument.compare(0, 2, "--") != 0)
			throw runtime_error("Unexpected command line argument '" + argument + "'.\n" + usage());

		const auto equals = argument.find('=');
		const auto name = argument.substr(0, equals);

		if (!isKnownOption(name))
			throw runtime_error("Unknown option " + name + ".\n\n" + usage());

		if (name == "--macro")
		{
			if (equals != string::npos && argument.substr(equals + 1) != "macro")
				throw runtime_error("Option --macro does not take a value.");

			if (equals == string::npos && i + 1 < argc && string(argv[i + 1]) == "macro")
				++i;

			addOption(options, name, "macro");
			continue;
		}

		if (name == "--help")
			throw runtime_error("Option --help does not take a value.");

		string value;

		if (equals == string::npos)
		{
			if (i + 1 >= argc)
				throw runtime_error("Missing value for option " + name + ".");

			value = argv[++i];
		}
		else
			value = argument.substr(equals + 1);

		addOption(options, name, value);
	}
}

static string requiredOption(const CommandLineOptions& options, const char* name)
{
	if (!options.has(name) || options.get(name).empty())
		throw runtime_error("Missing required option " + string(name) + ".\n\n" + usage());

	return options.get(name);
}

static void checkAllowedOptions(
	const CommandLineOptions& options, const char* const allowed[], size_t count, const string& format)
{
	for (const auto& option : options.values)
	{
		bool allowedOption = false;

		for (size_t i = 0; i < count; ++i)
		{
			if (option.first == allowed[i])
			{
				allowedOption = true;
				break;
			}
		}

		if (!allowedOption && option.first != "--help")
			throw runtime_error("Option " + option.first + " is not valid for " + format + " output.");
	}
}

static void run(int argc, const char* argv[])
{
	CommandLineOptions options;
	parseCommandLine(argc, argv, options);

	if (options.has("--help"))
	{
		cout << usage();
		return;
	}

	const auto inFilename = requiredOption(options, "--input");
	const auto outFormat = requiredOption(options, "--output-format");
	const auto outFilename = requiredOption(options, "--output");

	Lexer lexer(inFilename);

	Parser parser(&lexer);
	parser.parse();

	unique_ptr<Generator> generator;

	if (outFormat == "c++")
	{
		static const char* const allowed[] = {
			"--input",
			"--output-format",
			"--output",
			"--header-guard",
			"--class-name",
			"--prefix",
		};

		checkAllowedOptions(options, allowed, sizeof(allowed) / sizeof(allowed[0]), outFormat);

		const auto headerGuard = requiredOption(options, "--header-guard");
		const auto className = requiredOption(options, "--class-name");
		const auto prefix = requiredOption(options, "--prefix");

		generator.reset(new CppGenerator(outFilename, prefix, &parser, headerGuard, className));
	}
	else if (outFormat == "c-header")
	{
		static const char* const allowed[] = {
			"--input",
			"--output-format",
			"--output",
			"--header-guard",
			"--prefix",
			"--macro",
		};

		checkAllowedOptions(options, allowed, sizeof(allowed) / sizeof(allowed[0]), outFormat);

		const auto headerGuard = requiredOption(options, "--header-guard");
		const auto prefix = requiredOption(options, "--prefix");
		const auto macro = options.has("--macro") ? options.get("--macro") : string();

		generator.reset(new CHeaderGenerator(outFilename, prefix, &parser, headerGuard, macro));
	}
	else if (outFormat == "c-impl")
	{
		static const char* const allowed[] = {
			"--input",
			"--output-format",
			"--output",
			"--include-file",
			"--prefix",
		};

		checkAllowedOptions(options, allowed, sizeof(allowed) / sizeof(allowed[0]), outFormat);

		const auto includeFilename = requiredOption(options, "--include-file");
		const auto prefix = requiredOption(options, "--prefix");

		generator.reset(new CImplGenerator(outFilename, prefix, &parser, includeFilename));
	}
	else if (outFormat == "pascal")
	{
		static const char* const allowed[] = {
			"--input",
			"--output-format",
			"--output",
			"--unit-name",
			"--uses",
			"--interface-file",
			"--implementation-file",
			"--exception-class",
			"--prefix",
			"--functions-file",
		};

		checkAllowedOptions(options, allowed, sizeof(allowed) / sizeof(allowed[0]), outFormat);

		generator.reset(new PascalGenerator(outFilename, options.has("--prefix") ? options.get("--prefix") : string(),
			&parser, requiredOption(options, "--unit-name"), options.has("--uses") ? options.get("--uses") : string(),
			options.has("--interface-file") ? options.get("--interface-file") : string(),
			options.has("--implementation-file") ? options.get("--implementation-file") : string(),
			options.has("--exception-class") ? options.get("--exception-class") : string(),
			options.has("--functions-file") ? options.get("--functions-file") : string()));
	}
	else if (outFormat == "jna")
	{
		static const char* const allowed[] = {
			"--input",
			"--output-format",
			"--output",
			"--class-name",
			"--exception-class",
			"--prefix",
		};

		checkAllowedOptions(options, allowed, sizeof(allowed) / sizeof(allowed[0]), outFormat);

		const auto className = requiredOption(options, "--class-name");
		const auto exceptionClass = requiredOption(options, "--exception-class");
		const auto prefix = requiredOption(options, "--prefix");

		generator.reset(new JnaGenerator(outFilename, prefix, &parser, className, exceptionClass));
	}
	else if (outFormat == "json")
	{
		static const char* const allowed[] = {
			"--input",
			"--output-format",
			"--output",
		};

		checkAllowedOptions(options, allowed, sizeof(allowed) / sizeof(allowed[0]), outFormat);

		generator.reset(new JsonGenerator(outFilename, &parser));
	}
	else
		throw runtime_error("Invalid output format '" + outFormat + "'.");

	generator->generate();
}


//--------------------------------------


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
