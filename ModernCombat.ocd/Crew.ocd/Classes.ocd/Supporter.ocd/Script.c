#include CMC_Library_Class

/* --- Inventory --- */

local Ammo = {
	CMC_Ammo_Bullets = 90,
	CMC_Ammo_Missiles = 3,
};

local Items = {
	CMC_Weapon_MachineGun = 1,
	//CMC_Weapon_RocketLauncher = 1,
	//CMC_Tool_BlowTorch = 1,
	CMC_Tool_Ammo_Bag = 1, // FIXME - Has to be denied if the rule "no ammo" is active
};

local Grenades = {
	CMC_Grenade_Stun = 2,
	CMC_Grenade_Smoke = 1,
};

/* --- Abilities --- */

local Abilities = [CMC_Ability_ImproveAmmoEquipment];

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
