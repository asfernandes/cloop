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

#include "CalcCApi.h"
#include <malloc.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#define DLL_EXPORT __declspec(dllexport)
#else
#include <dlfcn.h>
#define DLL_EXPORT
#endif


//--------------------------------------

// StatusImpl


struct StatusImpl
{
	void* cloopDummy;
	struct StatusVTable* vtable;
	int code;
};

static void StatusImpl_dispose(struct Status* self)
{
	free(self);
}

static int StatusImpl_getCode(const struct Status* self)
{
	return ((struct StatusImpl*) self)->code;
}

static void StatusImpl_setCode(struct Status* self, int code)
{
	((struct StatusImpl*) self)->code = code;
}

struct Status* StatusImpl_create()
{
	static struct StatusVTable vtable = {
		{NULL},
		Status_VERSION,
		StatusImpl_dispose,
		StatusImpl_getCode,
		StatusImpl_setCode
	};

	struct StatusImpl* impl = malloc(sizeof(struct StatusImpl));
	impl->vtable = &vtable;
	impl->code = 0;

	return (struct Status*) impl;
}


//--------------------------------------

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

static int CalculatorImpl_sum(const struct Calculator* self, struct Status* status, int n1, int n2)
{
	if (n1 + n2 > 1000)
	{
		Status_setCode(status, Status_ERROR_1);
		return 0;
	}
	else
		return n1 + n2;
}

static int CalculatorImpl_getMemory(const struct Calculator* self)
{
	return ((struct CalculatorImpl*) self)->memory;
}

static void CalculatorImpl_setMemory(struct Calculator* self, int n)
{
	((struct CalculatorImpl*) self)->memory = n;
}

static void CalculatorImpl_sumAndStore(struct Calculator* self, struct Status* status,
	int n1, int n2)
{
	Calculator_setMemory(self, Calculator_sum(self, status, n1, n2));
}

struct Calculator* CalculatorImpl_create()
{
	static struct CalculatorVTable vtable = {
		{NULL},
		Calculator_VERSION,
		CalculatorImpl_dispose,
		CalculatorImpl_sum,
		CalculatorImpl_getMemory,
		CalculatorImpl_setMemory,
		CalculatorImpl_sumAndStore
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

static int Calculator2Impl_sum(const struct Calculator2* self, struct Status* status,
	int n1, int n2)
{
	if (n1 + n2 > 1000)
	{
		Status_setCode(status, Status_ERROR_1);
		return 0;
	}
	else
		return n1 + n2;
}

static int Calculator2Impl_getMemory(const struct Calculator2* self)
{
	return ((struct Calculator2Impl*) self)->memory;
}

static void Calculator2Impl_setMemory(struct Calculator2* self, int n)
{
	((struct Calculator2Impl*) self)->memory = n;
}

static void Calculator2Impl_sumAndStore(struct Calculator2* self, struct Status* status,
	int n1, int n2)
{
	Calculator2_setMemory(self, Calculator2_sum(self, status, n1, n2));
}

static int Calculator2Impl_multiply(const struct Calculator2* self, struct Status* status,
	int n1, int n2)
{
	return n1 * n2;
}

static void Calculator2Impl_copyMemory(struct Calculator2* self, const struct Calculator* calculator)
{
	Calculator2_setMemory(self, Calculator_getMemory(calculator));
}

static void Calculator2Impl_copyMemory2(struct Calculator2* self, const int* address)
{
	Calculator2_setMemory(self, *address);
}

struct Calculator2* Calculator2Impl_create()
{
	static struct Calculator2VTable vtable = {
		{NULL},
		Calculator2_VERSION,
		Calculator2Impl_dispose,
		Calculator2Impl_sum,
		Calculator2Impl_getMemory,
		Calculator2Impl_setMemory,
		Calculator2Impl_sumAndStore,
		Calculator2Impl_multiply,
		Calculator2Impl_copyMemory,
		Calculator2Impl_copyMemory2
	};

	struct Calculator2Impl* impl = malloc(sizeof(struct Calculator2Impl));
	impl->vtable = &vtable;
	impl->memory = 0;

	return (struct Calculator2*) impl;
}


//--------------------------------------

// BrokenCalculatorImpl


static int BrokenCalculatorImpl_sum(const struct Calculator* self, struct Status* status,
	int n1, int n2)
{
	return CalculatorImpl_sum(self, status, n1, n2) + 1;
}

struct Calculator* BrokenCalculatorImpl_create()
{
	static struct CalculatorVTable vtable = {
		{NULL},
		Calculator_VERSION,
		CalculatorImpl_dispose,
		BrokenCalculatorImpl_sum,
		CalculatorImpl_getMemory,
		CalculatorImpl_setMemory,
		CalculatorImpl_sumAndStore
	};

	struct CalculatorImpl* impl = malloc(sizeof(struct CalculatorImpl));
	impl->vtable = &vtable;
	impl->memory = 0;

	return (struct Calculator*) impl;
}


//--------------------------------------

// FactoryImpl


static void FactoryImpl_dispose(struct Factory* self)
{
	free(self);
}

static struct Status* FactoryImpl_createStatus(struct Factory* self)
{
	return StatusImpl_create();
}

static struct Calculator* FactoryImpl_createCalculator(struct Factory* self, struct Status* status)
{
	return CalculatorImpl_create();
}

static struct Calculator2* FactoryImpl_createCalculator2(struct Factory* self,
	struct Status* status)
{
	return Calculator2Impl_create();
}

static struct Calculator* FactoryImpl_createBrokenCalculator(struct Factory* self,
	struct Status* status)
{
	return BrokenCalculatorImpl_create();
}

struct Factory* FactoryImpl_create()
{
	static struct FactoryVTable vtable = {
		{NULL},
		Factory_VERSION,
		FactoryImpl_dispose,
		FactoryImpl_createStatus,
		FactoryImpl_createCalculator,
		FactoryImpl_createCalculator2,
		FactoryImpl_createBrokenCalculator
	};

	struct Factory* impl = malloc(sizeof(struct Factory));
	impl->vtable = &vtable;

	return impl;
}


//--------------------------------------

// Library entry point


DLL_EXPORT struct Factory* createFactory()
{
	return FactoryImpl_create();
}


//--------------------------------------


static void test(struct Factory* (*createFactory)())
{
	struct Factory* factory = createFactory();
	struct Status* status = (struct Status*) StatusImpl_create();
	struct Calculator* calculator;
	struct Calculator2* calculator2;
	int sum, code, address;

	calculator = Factory_createCalculator(factory, status);

	address = 40;

	Calculator_sumAndStore(calculator, status, 1, 22);
	printf("%d\n", Calculator_getMemory(calculator));	// 23

	Calculator_setMemory(calculator, Calculator_sum(calculator, status, 2, 33));
	printf("%d\n", Calculator_getMemory(calculator));	// 35

	calculator2 = Factory_createCalculator2(factory, status);

	Calculator2_copyMemory(calculator2, calculator);
	printf("%d\n", Calculator2_getMemory(calculator2));	// 35

	Calculator2_copyMemory2(calculator2, &address);
	printf("%d\n", Calculator2_getMemory(calculator2));	// 40

	Calculator_dispose(calculator);
	calculator = (struct Calculator*) calculator2;

	Calculator_sumAndStore(calculator, status, 1, 22);
	printf("%d\n", Calculator_getMemory(calculator));	// 23

	Calculator_setMemory(calculator, Calculator_sum(calculator, status, 2, 33));
	printf("%d\n", Calculator_getMemory(calculator));	// 35

	Calculator2_sumAndStore(calculator2, status, 1, 22);
	printf("%d\n", Calculator2_getMemory(calculator2));	// 23

	Calculator2_setMemory(calculator2, Calculator2_multiply(calculator2, status, 2, 33));
	printf("%d\n", Calculator2_getMemory(calculator2));	// 66

	Calculator_dispose(calculator);

	calculator = Factory_createBrokenCalculator(factory, status);

	Calculator_sumAndStore(calculator, status, 1, 22);
	printf("%d\n", Calculator_getMemory(calculator));	// 24

	Calculator_setMemory(calculator, Calculator_sum(calculator, status, 2, 33));
	printf("%d\n", Calculator_getMemory(calculator));	// 36

	sum = Calculator_sum(calculator, status, 600, 600);
	code = Status_getCode(status);
	if (code != 0)
		printf("exception %d\n", code);	// exception 1
	else
		printf("%d\n", sum);

	Calculator_dispose(calculator);

	Status_dispose(status);
	Factory_dispose(factory);

	printf("\n");
}

int main(int argc, char* argv[])
{
#ifdef WIN32
	HMODULE library = LoadLibrary(argv[1]);
#else
	void* library = dlopen(argv[1], RTLD_LAZY);
#endif

	struct Factory* (*createFactory)();

#ifdef WIN32
	createFactory = (struct Factory* (*)()) GetProcAddress(library, "createFactory");
#else
	createFactory = dlsym(library, "createFactory");
#endif

	test(createFactory);

#ifdef WIN32
	FreeLibrary(library);
#else
	dlclose(library);
#endif

	return 0;
}
