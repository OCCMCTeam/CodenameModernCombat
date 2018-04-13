#include Library_Firearm // Basic functionality
#include Library_AmmoManager // Provide functions for managing ammo
// FIXME: Ammo does not seem to work at the moment :( GetAmmo(GetFiremode()) always returns 0, might have to do with the inlude order
// #include Plugin_Firearm_AmmoLogic // For having an ammo logic in general
// FIXME: Reloading broke with the library refactoring; will just take ammo from the container for now
//#include Plugin_Firearm_ReloadFromAmmoSource // For ammo transfer between weapon and Clonk
//#include Plugin_Firearm_ReloadProgressBar // Temporary include, because the weapon needs to reload and there is no proper HUD yet

/* --- Basics and status --- */

local Collectible = true;

/* --- Ammo handling --- */

public func GetAmmoSource(ammo)
{
	return AMMO_Source_Container;
	// FIXME: if you want to have magazines and reloading, use this: return AMMO_Source_Local; // Has an internal ammo counter
}

public func GetAmmoContainer() 
{
	return Contained(); // FIXME: Take ammo from this container; Remove this function once reloading works again
}

public func GetAmmoReloadContainer() // FIXME: Is not called at the moment, because the reloading functions are not included
{
	return Contained(); // Reload from this container
}

/* --- Reloading --- */

public func NeedsReload(object user, proplist firemode)
{
	return !HasAmmo(firemode);
}

/* --- Cooldown --- */

public func NeedsRecovery(object user, proplist firemode)
{
	return !NeedsReload(user, firemode); // No recovery necessary when reloading, so that reload can happen instantly
}

