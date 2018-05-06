/*
	Prototypes.c

	Functions for dealing with prototypes.
	
	@author Marky
 */
 
// Gets an array that contains all prototypes of a value.
global func GetPrototypes(properties)
{
	var current = properties;
	var prototypes = [];
	while (true)
	{
		var prototype = GetPrototype(current);
		
		PushBack(prototypes, prototype);
		
		if (prototype == Global)
		{
			return prototypes;
		}
		
		current = prototype;
	}
}
