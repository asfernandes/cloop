library PascalLibrary;

uses CalcPascalApi, PascalClasses;


//--------------------------------------

// Library entry point


function createFactory: Factory; cdecl;
begin
	Result := MyFactoryImpl.create();
end;

exports
	createFactory;

end.
