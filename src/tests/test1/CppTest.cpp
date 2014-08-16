#include "CalcCppApi.h"
#include <stdio.h>
#include <dlfcn.h>


class ManualPolice
{
public:
	template <unsigned V, typename T>
	static inline void checkVersion(T*)
	{
	}

	template <typename T>
	static inline T* upgrade(T* o)
	{
		return o;
	}
};


typedef CalcApi<ManualPolice> calc;


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

	virtual int sum(int n1, int n2)
	{
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

	virtual void sumAndStore(int n1, int n2)
	{
		setMemory(sum(n1, n2));
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

	virtual int sum(int n1, int n2)
	{
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

	virtual void sumAndStore(int n1, int n2)
	{
		setMemory(sum(n1, n2));
	}

	virtual int multiply(int n1, int n2)
	{
		return n1 * n2;
	}

private:
	int memory;
};


//--------------------------------------

// BrokenCalculatorImpl


class BrokenCalculatorImpl : public calc::CalculatorBaseImpl<BrokenCalculatorImpl, CalculatorImpl>
{
public:
	virtual int sum(int n1, int n2)
	{
		return CalculatorImpl::sum(n1, n2) + 1;
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
	calc::Calculator* calculator = createCalculator();

	calculator->sumAndStore(1, 22);
	printf("%d\n", calculator->getMemory());	// 23

	calculator->setMemory(calculator->sum(2, 33));
	printf("%d\n", calculator->getMemory());	// 35

	calculator->dispose();

	calc::Calculator2* calculator2 = createCalculator2();
	calculator = calculator2;

	calculator->sumAndStore(1, 22);
	printf("%d\n", calculator->getMemory());	// 23

	calculator->setMemory(calculator->sum(2, 33));
	printf("%d\n", calculator->getMemory());	// 35

	calculator2->sumAndStore(1, 22);
	printf("%d\n", calculator2->getMemory());	// 23

	calculator2->setMemory(calculator2->multiply(2, 33));
	printf("%d\n", calculator2->getMemory());	// 66

	calculator->dispose();

	calculator = createBrokenCalculator();

	calculator->sumAndStore(1, 22);
	printf("%d\n", calculator->getMemory());	// 24

	calculator->setMemory(calculator->sum(2, 33));
	printf("%d\n", calculator->getMemory());	// 36

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
