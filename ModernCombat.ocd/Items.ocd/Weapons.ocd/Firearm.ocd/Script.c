#include Library_Firearm // Basic functionality
#include Library_AmmoManager // Provide functions for managing ammo
#include Plugin_Firearm_AmmoLogic // For having an ammo logic in general
#include Plugin_Firearm_ReloadFromAmmoSource // For ammo transfer between weapon and Clonk
#include Plugin_Firearm_ReloadProgressBar // Temporary include, because the weapon needs to reload and there is no proper HUD yet

/* --- Basics and status --- */

local Collectible = true;

/* --- Ammo handling --- */

public func GetAmmoSource(ammo)
{
	return AMMO_Source_Local; // Has an internal ammo counter
}

public func GetAmmoReloadContainer()
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

