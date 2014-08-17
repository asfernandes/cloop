program PascalTest;

uses DynLibs, CalcPascalApi;

type
	CreateCalculatorPtr = function(): Calculator; cdecl;
	CreateCalculator2Ptr = function(): Calculator2; cdecl;

var
	lib: TLibHandle;
	createCalculator: CreateCalculatorPtr;
	createCalculator2: CreateCalculator2Ptr;
	createBrokenCalculator: CreateCalculatorPtr;
	calc: Calculator;
	calc2: Calculator2;
begin
	lib := LoadLibrary(ParamStr(1));

	createCalculator := GetProcedureAddress(lib, 'createCalculator');
	createCalculator2 := GetProcedureAddress(lib, 'createCalculator2');
	createBrokenCalculator := GetProcedureAddress(lib, 'createBrokenCalculator');

	calc := createCalculator();

	calc.sumAndStore(1, 22);
	WriteLn(calc.getMemory());	// 23

	calc.setMemory(calc.sum(2, 33));
	WriteLn(calc.getMemory());	// 35

	calc.dispose();

	calc2 := createCalculator2();
	calc := calc2;

	calc.sumAndStore(1, 22);
	WriteLn(calc.getMemory());	// 23

	calc.setMemory(calc.sum(2, 33));
	WriteLn(calc.getMemory());	// 35

	calc2.sumAndStore(1, 22);
	WriteLn(calc2.getMemory());	// 23

	calc2.setMemory(calc2.multiply(2, 33));
	WriteLn(calc2.getMemory());	// 66

	calc.dispose();

	calc := createBrokenCalculator();

	calc.sumAndStore(1, 22);
	WriteLn(calc.getMemory());	// 24

	calc.setMemory(calc.sum(2, 33));
	WriteLn(calc.getMemory());	// 36

	calc.dispose();

	WriteLn;

	UnloadLibrary(lib);
end.
