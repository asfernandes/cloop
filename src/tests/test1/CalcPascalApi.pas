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

procedure CalculatorImpl_disposeDispatcher(this: Pointer); cdecl;
begin
	CalculatorImpl(this).dispose();
end;

function CalculatorImpl_sumDispatcher(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl;
begin
	Result := CalculatorImpl(this).sum(n1, n2);
end;

function CalculatorImpl_getMemoryDispatcher(this: Pointer): Integer; cdecl;
begin
	Result := CalculatorImpl(this).getMemory();
end;

procedure CalculatorImpl_setMemoryDispatcher(this: Pointer; n: Integer); cdecl;
begin
	CalculatorImpl(this).setMemory(n);
end;

procedure CalculatorImpl_sumAndStoreDispatcher(this: Pointer; n1: Integer; n2: Integer); cdecl;
begin
	CalculatorImpl(this).sumAndStore(n1, n2);
end;

var
	CalculatorImpl_vTable: CalculatorVTable;

constructor CalculatorImpl.create;
begin
	vTable := CalculatorImpl_vTable;
end;

procedure Calculator2Impl_disposeDispatcher(this: Pointer); cdecl;
begin
	Calculator2Impl(this).dispose();
end;

function Calculator2Impl_sumDispatcher(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl;
begin
	Result := Calculator2Impl(this).sum(n1, n2);
end;

function Calculator2Impl_getMemoryDispatcher(this: Pointer): Integer; cdecl;
begin
	Result := Calculator2Impl(this).getMemory();
end;

procedure Calculator2Impl_setMemoryDispatcher(this: Pointer; n: Integer); cdecl;
begin
	Calculator2Impl(this).setMemory(n);
end;

procedure Calculator2Impl_sumAndStoreDispatcher(this: Pointer; n1: Integer; n2: Integer); cdecl;
begin
	Calculator2Impl(this).sumAndStore(n1, n2);
end;

function Calculator2Impl_multiplyDispatcher(this: Pointer; n1: Integer; n2: Integer): Integer; cdecl;
begin
	Result := Calculator2Impl(this).multiply(n1, n2);
end;

var
	Calculator2Impl_vTable: Calculator2VTable;

constructor Calculator2Impl.create;
begin
	vTable := Calculator2Impl_vTable;
end;

initialization
	CalculatorImpl_vTable := CalculatorVTable.create;
	CalculatorImpl_vTable.version := 5;
	CalculatorImpl_vTable.dispose := @CalculatorImpl_disposeDispatcher;
	CalculatorImpl_vTable.sum := @CalculatorImpl_sumDispatcher;
	CalculatorImpl_vTable.getMemory := @CalculatorImpl_getMemoryDispatcher;
	CalculatorImpl_vTable.setMemory := @CalculatorImpl_setMemoryDispatcher;
	CalculatorImpl_vTable.sumAndStore := @CalculatorImpl_sumAndStoreDispatcher;

	Calculator2Impl_vTable := Calculator2VTable.create;
	Calculator2Impl_vTable.version := 6;
	Calculator2Impl_vTable.dispose := @Calculator2Impl_disposeDispatcher;
	Calculator2Impl_vTable.sum := @Calculator2Impl_sumDispatcher;
	Calculator2Impl_vTable.getMemory := @Calculator2Impl_getMemoryDispatcher;
	Calculator2Impl_vTable.setMemory := @Calculator2Impl_setMemoryDispatcher;
	Calculator2Impl_vTable.sumAndStore := @Calculator2Impl_sumAndStoreDispatcher;
	Calculator2Impl_vTable.multiply := @Calculator2Impl_multiplyDispatcher;

finalization
	CalculatorImpl_vTable.destroy;
	Calculator2Impl_vTable.destroy;

end.