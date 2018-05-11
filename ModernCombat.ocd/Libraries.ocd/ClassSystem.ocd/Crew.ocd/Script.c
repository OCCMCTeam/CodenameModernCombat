/**
	Interface for Clonks
	
	Clonks that want to use the class system should include this library.
	
	@author Marky
*/

local cmc_crew_class = nil;

public func GetCrewClass()
{
	return cmc_crew_class;
}

public func SetCrewClass(id class)
{
	cmc_crew_class = class;
	this->~OnSetCrewClass(class);
}
