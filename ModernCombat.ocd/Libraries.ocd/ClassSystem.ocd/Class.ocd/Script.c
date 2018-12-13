/**
	Basic library for Clonk classes

	@author Marky
*/

/* --- Safety check --- */

func Definition()
{
	this->AssertSamePropertyNames(CMC_Library_Class, ["Name", "Description"]);
}

/* --- Properties --- */

// Equipment
local Ammo;     // proplist: { definition = amount }
local Items;    // proplist: { somename = { Type = definition, Amount = amount}} - different logic necessary, because the properties are sorted alphabetically. Common scheme is, for example: Item1, Item2, etc.
local Grenades; // proplist: { definition = amount }

// Abilities
local Abilities; // array: ability definitions, for icon, name, description

/* --- Functions --- */

// Providing the properties with a certain name is OK for the equipment stuff,
// because it is likely to be accessed in few places only: The respawn menu
// and the part where the crew actually spawns and gets the equipment.
//
// With abilities however, a function seems better, because modifications and
// lots of items are expected to use it, so if we decide to refactor something
// internally the function call is more safe in regards of detecting errors.

public func HasAbility(id ability)
{
	if (Abilities)
	{
		return IsValueInArray(Abilities, ability);
	}
	return false;
}
