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

#ifndef CLOOP_GENERATOR_H
#define CLOOP_GENERATOR_H

#include "Parser.h"
#include <set>
#include <string>


#define DUMMY_VTABLE	1
#define DUMMY_INSTANCE	1


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


class FileGenerator : public Generator
{
public:
	FileGenerator(const std::string& filename);
	virtual ~FileGenerator();

protected:
	FILE* out;
};


class CBasedGenerator : public FileGenerator
{
protected:
	CBasedGenerator(const std::string& filename, bool cPlusPlus);

protected:
	std::string convertType(const Type& type);

private:
	bool cPlusPlus;
};


class CppGenerator : public CBasedGenerator
{
public:
	CppGenerator(const std::string& filename, Parser* parser, const std::string& headerGuard,
		const std::string& className);

public:
	virtual void generate();

private:
	Parser* parser;
	std::string headerGuard;
	std::string className;
};


class CHeaderGenerator : public CBasedGenerator
{
public:
	CHeaderGenerator(const std::string& filename, Parser* parser, const std::string& headerGuard);

public:
	virtual void generate();

private:
	Parser* parser;
	std::string headerGuard;
};


class CImplGenerator : public CBasedGenerator
{
public:
	CImplGenerator(const std::string& filename, Parser* parser, const std::string& includeFilename);

public:
	virtual void generate();

private:
	Parser* parser;
	std::string includeFilename;
};


class PascalGenerator : public FileGenerator
{
public:
	PascalGenerator(const std::string& filename, Parser* parser, const std::string& unitName);

public:
	virtual void generate();

private:
	std::string convertParameter(const Parameter& parameter);
	std::string convertType(const Type& type);

private:
	Parser* parser;
	std::string unitName;
	std::set<std::string> pointerTypes;
};


#endif	// CLOOP_GENERATOR_H
