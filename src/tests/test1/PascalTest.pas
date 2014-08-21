program PascalTest;

uses SysUtils, DynLibs, CalcPascalApi, PascalClasses;

type
	CreateCalculatorPtr = function(): Calculator; cdecl;
	CreateCalculator2Ptr = function(): Calculator2; cdecl;

var
	lib: TLibHandle;
	createCalculator: CreateCalculatorPtr;
	createCalculator2: CreateCalculator2Ptr;
	createBrokenCalculator: CreateCalculatorPtr;
	stat: Status;
	calc: Calculator;
	calc2: Calculator2;
begin
	lib := LoadLibrary(ParamStr(1));

	createCalculator := GetProcedureAddress(lib, 'createCalculator');
	createCalculator2 := GetProcedureAddress(lib, 'createCalculator2');
	createBrokenCalculator := GetProcedureAddress(lib, 'createBrokenCalculator');

	stat := MyStatusImpl.create;

	calc := createCalculator();

	calc.sumAndStore(stat, 1, 22);
	WriteLn(calc.getMemory());	// 23

	calc.setMemory(calc.sum(stat, 2, 33));
	WriteLn(calc.getMemory());	// 35

	calc2 := createCalculator2();

	calc2.copyMemory(calc);
	WriteLn(calc2.getMemory());	// 35

	calc.dispose();
	calc := calc2;

	calc.sumAndStore(stat, 1, 22);
	WriteLn(calc.getMemory());	// 23

	calc.setMemory(calc.sum(stat, 2, 33));
	WriteLn(calc.getMemory());	// 35

	calc2.sumAndStore(stat, 1, 22);
	WriteLn(calc2.getMemory());	// 23

	calc2.setMemory(calc2.multiply(stat, 2, 33));
	WriteLn(calc2.getMemory());	// 66

	calc.dispose();

	calc := createBrokenCalculator();

	calc.sumAndStore(stat, 1, 22);
	WriteLn(calc.getMemory());	// 24

	calc.setMemory(calc.sum(stat, 2, 33));
	WriteLn(calc.getMemory());	// 36

	//// FIXME:
	try
		WriteLn(calc.sum(stat, 600, 600));
	except
		on e: Exception do
			WriteLn('exception ', stat.getCode());	// exception 1
	end;

	calc.dispose();

	stat.dispose();

	WriteLn;

	UnloadLibrary(lib);
end.
