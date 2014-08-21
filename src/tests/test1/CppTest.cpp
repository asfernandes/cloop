#include "CalcCppApi.h"
#include <stdio.h>
#include <dlfcn.h>


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
};


typedef CalcApi<CalcPolice> calc;


class CalcException
{
public:
	CalcException(calc::Status* status)
		: code(status ? status->getCode() : -1)
	{
	}

public:
	int code;
};


void CalcPolice::checkException(CalcApi<CalcPolice>::Status* status)
{
	if (status->getCode() != 0)
		throw CalcException(status);
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

	virtual int getCode()
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

	virtual int sum(calc::Status* status, int n1, int n2)
	{
		if (n1 + n2 > 1000)
		{
			status->setCode(1);
			return 0;
		}
		else
			return n1 + n2;
	}

	virtual int getMemory()
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

	virtual int sum(calc::Status* status, int n1, int n2)
	{
		if (n1 + n2 > 1000)
		{
			status->setCode(1);
			return 0;
		}
		else
			return n1 + n2;
	}

	virtual int getMemory()
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

	virtual int multiply(calc::Status* status, int n1, int n2)
	{
		return n1 * n2;
	}

	virtual void copyMemory(calc::Calculator* calculator)
	{
		setMemory(calculator->getMemory());
	}

private:
	int memory;
};


//--------------------------------------

// BrokenCalculatorImpl


class BrokenCalculatorImpl : public calc::CalculatorBaseImpl<BrokenCalculatorImpl, CalculatorImpl>
{
public:
	virtual int sum(calc::Status* status, int n1, int n2)
	{
		return CalculatorImpl::sum(status, n1, n2) + 1;
	}
};


//--------------------------------------

// Library entry points


extern "C" calc::Calculator* createCalculator()
{
	return new CalculatorImpl();
}

extern "C" calc::Calculator2* createCalculator2()
{
	return new Calculator2Impl();
}

extern "C" calc::Calculator* createBrokenCalculator()
{
	return new BrokenCalculatorImpl();
}


//--------------------------------------


static void test(calc::Calculator* (*createCalculator)(),
	calc::Calculator2* (*createCalculator2)(),
	calc::Calculator* (*createBrokenCalculator)())
{
	StatusImpl status;

	calc::Calculator* calculator = createCalculator();

	calculator->sumAndStore(&status, 1, 22);
	printf("%d\n", calculator->getMemory());	// 23

	calculator->setMemory(calculator->sum(&status, 2, 33));
	printf("%d\n", calculator->getMemory());	// 35

	calc::Calculator2* calculator2 = createCalculator2();

	calculator2->copyMemory(calculator);
	printf("%d\n", calculator2->getMemory());	// 35

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

	calculator = createBrokenCalculator();

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

	printf("\n");
}

template <typename T>
static void loadSymbol(void* library, const char* name, T& symbol)
{
	symbol = (T) dlsym(library, name);
}

int main(int argc, char* argv[])
{
	void* library = dlopen(argv[1], RTLD_LAZY);

	calc::Calculator* (*createCalculator)();
	loadSymbol(library, "createCalculator", createCalculator);

	calc::Calculator2* (*createCalculator2)();
	loadSymbol(library, "createCalculator2", createCalculator2);

	calc::Calculator* (*createBrokenCalculator)();
	loadSymbol(library, "createBrokenCalculator", createBrokenCalculator);

	test(createCalculator, createCalculator2, createBrokenCalculator);

	dlclose(library);

	return 0;
}
