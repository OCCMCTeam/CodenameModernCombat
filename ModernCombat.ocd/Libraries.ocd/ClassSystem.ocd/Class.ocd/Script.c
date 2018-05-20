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
local Items;    // proplist: { definition = amount }
local Grenades; // proplist: { definition = amount }

// Abilities
local ImprovesAmmoEquipment = false;	// bool: Reload ammo bag, and further options?
local ImprovesMedicalEquipment = false; // bool: Reload medical bag, and further options?

/* --- Functions --- */

// Providing the properties with a certain name is OK for the equipment stuff,
// because it is likely to be accessed in few places only: The respawn menu
// and the part where the crew actually spawns and gets the equipment.
//
// With abilities however, a function seems better, because modifications and
// lots of items are expected to use it, so if we decide to refactor something
// internally the function call is more safe in regards of detecting errors.


public func ClassImprovesAmmoEquipment()
{
	return ImprovesAmmoEquipment;
}

public func ClassImprovesMedicalEquipment()
{
	return ImprovesMedicalEquipment;
}

public func ClassHasAbilities()
{
	return ClassImprovesAmmoEquipment()
	    || ClassImprovesMedicalEquipment();
}
