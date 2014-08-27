#include "CalcCppApi.h"
#include <assert.h>
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


extern "C" calc::Factory* createFactory()
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

template <typename T>
static void loadSymbol(void* library, const char* name, T& symbol)
{
	symbol = (T) dlsym(library, name);
}

int main(int argc, char* argv[])
{
	void* library = dlopen(argv[1], RTLD_LAZY);

	calc::Factory* (*createFactory)();
	loadSymbol(library, "createFactory", createFactory);

	test(createFactory);

	dlclose(library);

	return 0;
}
