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

local Ammo;     // proplist: { definition = amount }
local Items;    // proplist: { definition = amount }
local Grenades; // proplist: { definition = amount }
