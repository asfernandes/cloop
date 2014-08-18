library PascalLibrary;

uses CalcPascalApi;

type
	MyCalculatorImpl = class(CalculatorImpl)
		constructor create;

		procedure dispose(); override;
		function sum(n1: Integer; n2: Integer): Integer; override;
		function getMemory(): Integer; override;
		procedure setMemory(n: Integer); override;
		procedure sumAndStore(n1: Integer; n2: Integer); override;

	private
		memory: Integer;
	end;

	MyCalculator2Impl = class(Calculator2Impl)
		constructor create;

		procedure dispose(); override;
		function sum(n1: Integer; n2: Integer): Integer; override;
		function getMemory(): Integer; override;
		procedure setMemory(n: Integer); override;
		procedure sumAndStore(n1: Integer; n2: Integer); override;
		function multiply(n1: Integer; n2: Integer): Integer; override;

	private
		memory: Integer;
	end;

	MyBrokenCalculatorImpl = class(MyCalculatorImpl)
		function sum(n1: Integer; n2: Integer): Integer; override;
	end;


//--------------------------------------

// MyCalculatorImpl


constructor MyCalculatorImpl.create;
begin
	inherited;
	memory := 0;
end;

procedure MyCalculatorImpl.dispose();
begin
	self.destroy();
end;

function MyCalculatorImpl.sum(n1: Integer; n2: Integer): Integer;
begin
	Result := n1 + n2;
end;

function MyCalculatorImpl.getMemory(): Integer;
begin
	Result := memory;
end;

procedure MyCalculatorImpl.setMemory(n: Integer);
begin
	memory := n;
end;

procedure MyCalculatorImpl.sumAndStore(n1: Integer; n2: Integer);
begin
	setMemory(sum(n1, n2));
end;


//--------------------------------------

// MyCalculator2Impl


constructor MyCalculator2Impl.create;
begin
	inherited;
	memory := 0;
end;

procedure MyCalculator2Impl.dispose();
begin
	self.destroy();
end;

function MyCalculator2Impl.sum(n1: Integer; n2: Integer): Integer;
begin
	Result := n1 + n2;
end;

function MyCalculator2Impl.getMemory(): Integer;
begin
	Result := memory;
end;

procedure MyCalculator2Impl.setMemory(n: Integer);
begin
	memory := n;
end;

procedure MyCalculator2Impl.sumAndStore(n1: Integer; n2: Integer);
begin
	setMemory(sum(n1, n2));
end;

function MyCalculator2Impl.multiply(n1: Integer; n2: Integer): Integer;
begin
	Result := n1 * n2;
end;


//--------------------------------------

// MyBrokenCalculatorImpl


function MyBrokenCalculatorImpl.sum(n1: Integer; n2: Integer): Integer;
begin
	Result := inherited sum(n1, n2) + 1;
end;


//--------------------------------------

// Library entry points


function createCalculator: Calculator; cdecl;
begin
	Result := MyCalculatorImpl.create();
end;

function createCalculator2: Calculator2; cdecl;
begin
	Result := MyCalculator2Impl.create();
end;

function createBrokenCalculator: Calculator; cdecl;
begin
	Result := MyBrokenCalculatorImpl.create();
end;

exports
	createCalculator,
	createCalculator2,
	createBrokenCalculator;

end.
