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

	CalculatorImpl = class(Calculator)
		constructor create;

		procedure dispose(); virtual; abstract;
		function sum(n1: Integer; n2: Integer): Integer; virtual; abstract;
		function getMemory(): Integer; virtual; abstract;
		procedure setMemory(n: Integer); virtual; abstract;
		procedure sumAndStore(n1: Integer; n2: Integer); virtual; abstract;

	private
		class var vTableImpl: CalculatorVTable;
		class procedure disposeDispatcher(this: Pointer); cdecl; static;
		class function sumDispatcher(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl; static;
		class function getMemoryDispatcher(this: Pointer): Integer; cdecl; static;
		class procedure setMemoryDispatcher(this: Pointer; n: Integer); cdecl; static;
		class procedure sumAndStoreDispatcher(this: Pointer; n1: Integer; n2: Integer); cdecl; static;
	end;

	Calculator2VTable = class(CalculatorVTable)
		multiply: Calculator2_multiplyPtr;
	end;

	Calculator2 = class(Calculator)
		function multiply(n1: Integer; n2: Integer): Integer;
	end;

	Calculator2Impl = class(Calculator2)
		constructor create;

		procedure dispose(); virtual; abstract;
		function sum(n1: Integer; n2: Integer): Integer; virtual; abstract;
		function getMemory(): Integer; virtual; abstract;
		procedure setMemory(n: Integer); virtual; abstract;
		procedure sumAndStore(n1: Integer; n2: Integer); virtual; abstract;
		function multiply(n1: Integer; n2: Integer): Integer; virtual; abstract;

	private
		class var vTableImpl: Calculator2VTable;
		class procedure disposeDispatcher(this: Pointer); cdecl; static;
		class function sumDispatcher(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl; static;
		class function getMemoryDispatcher(this: Pointer): Integer; cdecl; static;
		class procedure setMemoryDispatcher(this: Pointer; n: Integer); cdecl; static;
		class procedure sumAndStoreDispatcher(this: Pointer; n1: Integer; n2: Integer); cdecl; static;
		class function multiplyDispatcher(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl; static;
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

constructor CalculatorImpl.create;
var
	newVTable: CalculatorVTable;
begin
	if (vTableImpl = nil) then
	begin
		newVTable := CalculatorVTable.create;
		newVTable.version := 0;
		newVTable.dispose := @CalculatorImpl.disposeDispatcher;
		newVTable.sum := @CalculatorImpl.sumDispatcher;
		newVTable.getMemory := @CalculatorImpl.getMemoryDispatcher;
		newVTable.setMemory := @CalculatorImpl.setMemoryDispatcher;
		newVTable.sumAndStore := @CalculatorImpl.sumAndStoreDispatcher;
		vTableImpl := newVTable;
	end;

	vTable := vTableImpl;
end;

class procedure CalculatorImpl.disposeDispatcher(this: Pointer); cdecl; static;
begin
	CalculatorImpl(this).dispose();
end;

class function CalculatorImpl.sumDispatcher(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl; static;
begin
	Result := CalculatorImpl(this).sum(n1, n2);
end;

class function CalculatorImpl.getMemoryDispatcher(this: Pointer): Integer; cdecl; static;
begin
	Result := CalculatorImpl(this).getMemory();
end;

class procedure CalculatorImpl.setMemoryDispatcher(this: Pointer; n: Integer); cdecl; static;
begin
	CalculatorImpl(this).setMemory(n);
end;

class procedure CalculatorImpl.sumAndStoreDispatcher(this: Pointer; n1: Integer; n2: Integer); cdecl; static;
begin
	CalculatorImpl(this).sumAndStore(n1, n2);
end;

constructor Calculator2Impl.create;
var
	newVTable: Calculator2VTable;
begin
	if (vTableImpl = nil) then
	begin
		newVTable := Calculator2VTable.create;
		newVTable.version := 0;
		newVTable.dispose := @Calculator2Impl.disposeDispatcher;
		newVTable.sum := @Calculator2Impl.sumDispatcher;
		newVTable.getMemory := @Calculator2Impl.getMemoryDispatcher;
		newVTable.setMemory := @Calculator2Impl.setMemoryDispatcher;
		newVTable.sumAndStore := @Calculator2Impl.sumAndStoreDispatcher;
		newVTable.multiply := @Calculator2Impl.multiplyDispatcher;
		vTableImpl := newVTable;
	end;

	vTable := vTableImpl;
end;

class procedure Calculator2Impl.disposeDispatcher(this: Pointer); cdecl; static;
begin
	Calculator2Impl(this).dispose();
end;

class function Calculator2Impl.sumDispatcher(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl; static;
begin
	Result := Calculator2Impl(this).sum(n1, n2);
end;

class function Calculator2Impl.getMemoryDispatcher(this: Pointer): Integer; cdecl; static;
begin
	Result := Calculator2Impl(this).getMemory();
end;

class procedure Calculator2Impl.setMemoryDispatcher(this: Pointer; n: Integer); cdecl; static;
begin
	Calculator2Impl(this).setMemory(n);
end;

class procedure Calculator2Impl.sumAndStoreDispatcher(this: Pointer; n1: Integer; n2: Integer); cdecl; static;
begin
	Calculator2Impl(this).sumAndStore(n1, n2);
end;

class function Calculator2Impl.multiplyDispatcher(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl; static;
begin
	Result := Calculator2Impl(this).multiply(n1, n2);
end;

end.
