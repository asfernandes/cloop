#include "CalcCApi.h"
#include <malloc.h>
#include <stdio.h>
#include <dlfcn.h>


// CalculatorImpl


struct CalculatorImpl
{
	void* cloopDummy;
	struct CalculatorVTable* vtable;
	int memory;
};

static void CalculatorImpl_dispose(struct Calculator* self)
{
	free(self);
}

static int CalculatorImpl_sum(struct Calculator* self, int n1, int n2)
{
	return n1 + n2;
}

static int CalculatorImpl_getMemory(struct Calculator* self)
{
	return ((struct CalculatorImpl*) self)->memory;
}

static void CalculatorImpl_setMemory(struct Calculator* self, int n)
{
	((struct CalculatorImpl*) self)->memory = n;
}

static void CalculatorImpl_sumAndStore(struct Calculator* self, int n1, int n2)
{
	Calculator_setMemory(self, Calculator_sum(self, n1, n2));
}

struct Calculator* CalculatorImpl_create()
{
	static struct CalculatorVTable vtable = {
		.version = Calculator_VERSION,
		.dispose = CalculatorImpl_dispose,
		.sum = CalculatorImpl_sum,
		.getMemory = CalculatorImpl_getMemory,
		.setMemory = CalculatorImpl_setMemory,
		.sumAndStore = CalculatorImpl_sumAndStore
	};

	struct CalculatorImpl* impl = malloc(sizeof(struct CalculatorImpl));
	impl->vtable = &vtable;
	impl->memory = 0;

	return (struct Calculator*) impl;
}


//--------------------------------------

// Calculator2Impl


struct Calculator2Impl
{
	void* cloopDummy;
	struct Calculator2VTable* vtable;
	int memory;
};

static void Calculator2Impl_dispose(struct Calculator2* self)
{
	free(self);
}

static int Calculator2Impl_sum(struct Calculator2* self, int n1, int n2)
{
	return n1 + n2;
}

static int Calculator2Impl_getMemory(struct Calculator2* self)
{
	return ((struct Calculator2Impl*) self)->memory;
}

static void Calculator2Impl_setMemory(struct Calculator2* self, int n)
{
	((struct Calculator2Impl*) self)->memory = n;
}

static void Calculator2Impl_sumAndStore(struct Calculator2* self, int n1, int n2)
{
	Calculator2_setMemory(self, Calculator2_sum(self, n1, n2));
}

static int Calculator2Impl_multiply(struct Calculator2* self, int n1, int n2)
{
	return n1 * n2;
}

struct Calculator2* Calculator2Impl_create()
{
	static struct Calculator2VTable vtable = {
		.version = Calculator2_VERSION,
		.dispose = Calculator2Impl_dispose,
		.sum = Calculator2Impl_sum,
		.getMemory = Calculator2Impl_getMemory,
		.setMemory = Calculator2Impl_setMemory,
		.sumAndStore = Calculator2Impl_sumAndStore,
		.multiply = Calculator2Impl_multiply,
	};

	struct Calculator2Impl* impl = malloc(sizeof(struct Calculator2Impl));
	impl->vtable = &vtable;
	impl->memory = 0;

	return (struct Calculator2*) impl;
}


//--------------------------------------

// BrokenCalculatorImpl


static int BrokenCalculatorImpl_sum(struct Calculator* self, int n1, int n2)
{
	return CalculatorImpl_sum(self, n1, n2) + 1;
}

struct Calculator* BrokenCalculatorImpl_create()
{
	static struct CalculatorVTable vtable = {
		.version = Calculator_VERSION,
		.dispose = CalculatorImpl_dispose,
		.sum = BrokenCalculatorImpl_sum,
		.getMemory = CalculatorImpl_getMemory,
		.setMemory = CalculatorImpl_setMemory,
		.sumAndStore = CalculatorImpl_sumAndStore
	};

	struct CalculatorImpl* impl = malloc(sizeof(struct CalculatorImpl));
	impl->vtable = &vtable;
	impl->memory = 0;

	return (struct Calculator*) impl;
}


//--------------------------------------

// Library entry points


struct Calculator* createCalculator()
{
	return CalculatorImpl_create();
}

struct Calculator2* createCalculator2()
{
	return Calculator2Impl_create();
}

struct Calculator* createBrokenCalculator()
{
	return BrokenCalculatorImpl_create();
}


//--------------------------------------


static void test(struct Calculator* (*createCalculator)(),
	struct Calculator2* (*createCalculator2)(),
	struct Calculator* (*createBrokenCalculator)())
{
	struct Calculator* calculator = createCalculator();

	Calculator_sumAndStore(calculator, 1, 22);
	printf("%d\n", Calculator_getMemory(calculator));	// 23

	Calculator_setMemory(calculator, Calculator_sum(calculator, 2, 33));
	printf("%d\n", Calculator_getMemory(calculator));	// 35

	Calculator_dispose(calculator);

	struct Calculator2* calculator2 = createCalculator2();
	calculator = (struct Calculator*) calculator2;

	Calculator_sumAndStore(calculator, 1, 22);
	printf("%d\n", Calculator_getMemory(calculator));	// 23

	Calculator_setMemory(calculator, Calculator_sum(calculator, 2, 33));
	printf("%d\n", Calculator_getMemory(calculator));	// 35

	Calculator2_sumAndStore(calculator2, 1, 22);
	printf("%d\n", Calculator2_getMemory(calculator2));	// 23

	Calculator2_setMemory(calculator2, Calculator2_multiply(calculator2, 2, 33));
	printf("%d\n", Calculator2_getMemory(calculator2));	// 66

	Calculator_dispose(calculator);

	calculator = createBrokenCalculator();

	Calculator_sumAndStore(calculator, 1, 22);
	printf("%d\n", Calculator_getMemory(calculator));	// 24

	Calculator_setMemory(calculator, Calculator_sum(calculator, 2, 33));
	printf("%d\n", Calculator_getMemory(calculator));	// 36

	Calculator_dispose(calculator);

	printf("\n");
}

int main(int argc, char* argv[])
{
	void* library = dlopen(argv[1], RTLD_LAZY);

	struct Calculator* (*createCalculator)() = dlsym(library, "createCalculator");
	struct Calculator2* (*createCalculator2)() = dlsym(library, "createCalculator2");
	struct Calculator* (*createBrokenCalculator)() = dlsym(library, "createBrokenCalculator");

	test(createCalculator, createCalculator2, createBrokenCalculator);

	dlclose(library);

	return 0;
}