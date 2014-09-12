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

#include "CalcCppApi.h"
#include <assert.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#define DLL_EXPORT __declspec(dllexport)
#else
#include <dlfcn.h>
#define DLL_EXPORT
#endif


class CalcPolice
{
public:
	template <unsigned V, typename T>
	static inline void checkVersion(T*)
	{
	}

	template <typename T>
	static inline T* upgrade(T* o)
	{
		//// TODO:
		return o;
	}

	static void checkException(CalcApi<CalcPolice>::Status* status);
	static void catchException(CalcApi<CalcPolice>::Status* status);
};


typedef CalcApi<CalcPolice> calc;


class CalcException
{
public:
	CalcException(calc::Status* status)
		: code(status ? status->getCode() : -1)
	{
	}

	CalcException(int code)
		: code(code)
	{
	}

public:
	int code;
};


void CalcPolice::checkException(CalcApi<CalcPolice>::Status* status)
{
	assert(status);

	if (status->getCode() != 0)
		throw CalcException(status);
}


void CalcPolice::catchException(CalcApi<CalcPolice>::Status* status)
{
	try
	{
		throw;
	}
	catch (const CalcException& e)
	{
		assert(status);
		status->setCode(e.code);
	}
	catch (...)
	{
		assert(false);
	}
}


//--------------------------------------

// StatusImpl


class StatusImpl : public calc::StatusImpl<StatusImpl>
{
public:
	StatusImpl()
		: code(0)
	{
	}

	virtual void dispose()
	{
		delete this;
	}

	virtual int getCode() const
	{
		return code;
	}

	virtual void setCode(int code)
	{
		this->code = code;
	}

private:
	int code;
};


//--------------------------------------

// CalculatorImpl


class CalculatorImpl : public calc::CalculatorImpl<CalculatorImpl>
{
public:
	CalculatorImpl()
		: memory(0)
	{
	}

	virtual void dispose()
	{
		delete this;
	}

	virtual int sum(calc::Status* status, int n1, int n2) const
	{
		if (n1 + n2 > 1000)
			throw CalcException(calc::Status::ERROR_1);
		else
			return n1 + n2;
	}

	virtual int getMemory() const
	{
		return memory;
	}

	virtual void setMemory(int n)
	{
		memory = n;
	}

	virtual void sumAndStore(calc::Status* status, int n1, int n2)
	{
		setMemory(sum(status, n1, n2));
	}

private:
	int memory;
};


//--------------------------------------

// Calculator2Impl


class Calculator2Impl : public calc::Calculator2Impl<Calculator2Impl>
{
public:
	Calculator2Impl()
		: memory(0)
	{
	}

	virtual void dispose()
	{
		delete this;
	}

	virtual int sum(calc::Status* status, int n1, int n2) const
	{
		if (n1 + n2 > 1000)
			throw CalcException(calc::Status::ERROR_1);
		else
			return n1 + n2;
	}

	virtual int getMemory() const
	{
		return memory;
	}

	virtual void setMemory(int n)
	{
		memory = n;
	}

	virtual void sumAndStore(calc::Status* status, int n1, int n2)
	{
		setMemory(sum(status, n1, n2));
	}

	virtual int multiply(calc::Status* status, int n1, int n2) const
	{
		return n1 * n2;
	}

	virtual void copyMemory(const calc::Calculator* calculator)
	{
		setMemory(calculator->getMemory());
	}

	virtual void copyMemory2(const int* address)
	{
		setMemory(*address);
	}

private:
	int memory;
};


//--------------------------------------

// BrokenCalculatorImpl


class BrokenCalculatorImpl : public calc::CalculatorBaseImpl<BrokenCalculatorImpl, CalculatorImpl>
{
public:
	virtual int sum(calc::Status* status, int n1, int n2) const
	{
		return CalculatorImpl::sum(status, n1, n2) + 1;
	}
};


//--------------------------------------

// FactoryImpl


class FactoryImpl : public calc::FactoryImpl<FactoryImpl>
{
public:
	virtual void dispose()
	{
		delete this;
	}

	virtual calc::Status* createStatus()
	{
		return new StatusImpl();
	}

	virtual calc::Calculator* createCalculator(calc::Status* status)
	{
		return new CalculatorImpl();
	}

	virtual calc::Calculator2* createCalculator2(calc::Status* status)
	{
		return new Calculator2Impl();
	}

	virtual calc::Calculator* createBrokenCalculator(calc::Status* status)
	{
		return new BrokenCalculatorImpl();
	}
};


//--------------------------------------

// Library entry point


extern "C" DLL_EXPORT calc::Factory* createFactory()
{
	return new FactoryImpl();
}


//--------------------------------------


static void test(calc::Factory* (*createFactory)())
{
	calc::Factory* factory = createFactory();
	StatusImpl status;

	calc::Calculator* calculator = factory->createCalculator(&status);

	calculator->sumAndStore(&status, 1, 22);
	printf("%d\n", calculator->getMemory());	// 23

	calculator->setMemory(calculator->sum(&status, 2, 33));
	printf("%d\n", calculator->getMemory());	// 35

	calc::Calculator2* calculator2 = factory->createCalculator2(&status);

	calculator2->copyMemory(calculator);
	printf("%d\n", calculator2->getMemory());	// 35

	int address = 40;
	calculator2->copyMemory2(&address);
	printf("%d\n", calculator2->getMemory());	// 40

	calculator->dispose();
	calculator = calculator2;

	calculator->sumAndStore(&status, 1, 22);
	printf("%d\n", calculator->getMemory());	// 23

	calculator->setMemory(calculator->sum(&status, 2, 33));
	printf("%d\n", calculator->getMemory());	// 35

	calculator2->sumAndStore(&status, 1, 22);
	printf("%d\n", calculator2->getMemory());	// 23

	calculator2->setMemory(calculator2->multiply(&status, 2, 33));
	printf("%d\n", calculator2->getMemory());	// 66

	calculator->dispose();

	calculator = factory->createBrokenCalculator(&status);

	calculator->sumAndStore(&status, 1, 22);
	printf("%d\n", calculator->getMemory());	// 24

	calculator->setMemory(calculator->sum(&status, 2, 33));
	printf("%d\n", calculator->getMemory());	// 36

	try
	{
		printf("%d\n", calculator->sum(&status, 600, 600));
	}
	catch (const CalcException& e)
	{
		printf("exception %d\n", e.code);	// exception 1
	}

	calculator->dispose();
	factory->dispose();

	printf("\n");
}

#ifdef WIN32
template <typename T>
static void loadSymbol(HMODULE library, const char* name, T& symbol)
{
	symbol = (T) GetProcAddress(library, name);
}
#else
template <typename T>
static void loadSymbol(void* library, const char* name, T& symbol)
{
	symbol = (T) dlsym(library, name);
}
#endif

int main(int argc, char* argv[])
{
	calc::Factory* (*createFactory)();

#ifdef WIN32
	HMODULE library = LoadLibrary(argv[1]);
#else
	void* library = dlopen(argv[1], RTLD_LAZY);
#endif

	loadSymbol(library, "createFactory", createFactory);
	test(createFactory);

#ifdef WIN32
	FreeLibrary(library);
#else
	dlclose(library);
#endif

	return 0;
}
