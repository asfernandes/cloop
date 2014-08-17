unit CalcPascalApi;

interface

uses Classes;

type
	Calculator_disposePtr = procedure(this: Pointer); cdecl;
	Calculator_sumPtr = function(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl;
	Calculator_getMemoryPtr = function(this: Pointer): Integer; cdecl;
	Calculator_setMemoryPtr = procedure(this: Pointer; n: Integer); cdecl;
	Calculator_sumAndStorePtr = procedure(this: Pointer; n1: Integer; n2: Integer); cdecl;
	Calculator2_multiplyPtr = function(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl;

	CalculatorVTable = class
{$ifndef FPC}
		dummy: PtrInt;
{$endif}
		version: PtrInt;
		dispose: Calculator_disposePtr;
		sum: Calculator_sumPtr;
		getMemory: Calculator_getMemoryPtr;
		setMemory: Calculator_setMemoryPtr;
		sumAndStore: Calculator_sumAndStorePtr;
	end;

	Calculator = class
{$ifndef FPC}
		dummy: PtrInt;
{$endif}
		vTable: CalculatorVTable;
		procedure dispose();
		function sum(n1: Integer; n2: Integer): Integer;
		function getMemory(): Integer;
		procedure setMemory(n: Integer);
		procedure sumAndStore(n1: Integer; n2: Integer);
	end;

	Calculator2VTable = class(CalculatorVTable)
		multiply: Calculator2_multiplyPtr;
	end;

	Calculator2 = class(Calculator)
		function multiply(n1: Integer; n2: Integer): Integer;
	end;

implementation

procedure Calculator.dispose();
begin
	CalculatorVTable(vTable).dispose(Self);
end;

function Calculator.sum(n1: Integer; n2: Integer): Integer;
begin
	Result := CalculatorVTable(vTable).sum(Self, n1, n2);
end;

function Calculator.getMemory(): Integer;
begin
	Result := CalculatorVTable(vTable).getMemory(Self);
end;

procedure Calculator.setMemory(n: Integer);
begin
	CalculatorVTable(vTable).setMemory(Self, n);
end;

procedure Calculator.sumAndStore(n1: Integer; n2: Integer);
begin
	CalculatorVTable(vTable).sumAndStore(Self, n1, n2);
end;

function Calculator2.multiply(n1: Integer; n2: Integer): Integer;
begin
	Result := Calculator2VTable(vTable).multiply(Self, n1, n2);
end;

end.
