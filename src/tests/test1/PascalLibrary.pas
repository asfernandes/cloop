library PascalLibrary;

uses CalcPascalApi, PascalClasses;


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
