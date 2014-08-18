#ifndef CALC_C_API_H
#define CALC_C_API_H

#include <stdint.h>

#ifndef CLOOP_EXTERN_C
#ifdef __cplusplus
#define CLOOP_EXTERN_C extern "C"
#else
#define CLOOP_EXTERN_C
#endif
#endif


#define Calculator_VERSION 5

struct Calculator;

struct CalculatorVTable
{
	void* cloopDummy[1];
	uintptr_t version;
	void (*dispose)(struct Calculator* self);
	int (*sum)(struct Calculator* self, int n1, int n2);
	int (*getMemory)(struct Calculator* self);
	void (*setMemory)(struct Calculator* self, int n);
	void (*sumAndStore)(struct Calculator* self, int n1, int n2);
};

struct Calculator
{
	void* cloopDummy[1];
	struct CalculatorVTable* vtable;
};

CLOOP_EXTERN_C void Calculator_dispose(struct Calculator* self);

CLOOP_EXTERN_C int Calculator_sum(struct Calculator* self, int n1, int n2);

CLOOP_EXTERN_C int Calculator_getMemory(struct Calculator* self);

CLOOP_EXTERN_C void Calculator_setMemory(struct Calculator* self, int n);

CLOOP_EXTERN_C void Calculator_sumAndStore(struct Calculator* self, int n1, int n2);

#define Calculator2_VERSION 6

struct Calculator2;

struct Calculator2VTable
{
	void* cloopDummy[1];
	uintptr_t version;
	void (*dispose)(struct Calculator2* self);
	int (*sum)(struct Calculator2* self, int n1, int n2);
	int (*getMemory)(struct Calculator2* self);
	void (*setMemory)(struct Calculator2* self, int n);
	void (*sumAndStore)(struct Calculator2* self, int n1, int n2);
	int (*multiply)(struct Calculator2* self, int n1, int n2);
};

struct Calculator2
{
	void* cloopDummy[1];
	struct Calculator2VTable* vtable;
};

CLOOP_EXTERN_C void Calculator2_dispose(struct Calculator2* self);

CLOOP_EXTERN_C int Calculator2_sum(struct Calculator2* self, int n1, int n2);

CLOOP_EXTERN_C int Calculator2_getMemory(struct Calculator2* self);

CLOOP_EXTERN_C void Calculator2_setMemory(struct Calculator2* self, int n);

CLOOP_EXTERN_C void Calculator2_sumAndStore(struct Calculator2* self, int n1, int n2);

CLOOP_EXTERN_C int Calculator2_multiply(struct Calculator2* self, int n1, int n2);

#endif	// CALC_C_API_H
