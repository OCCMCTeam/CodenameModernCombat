#include CMC_Library_Class

/* --- Inventory --- */

local Ammo = {
	CMC_Ammo_Bullets = 90,
	CMC_Ammo_Missiles = 3,
};

local Items = {
	Item1 = { Type = CMC_Weapon_MachineGun    , Amount = 1,},
	Item2 = { Type = CMC_Weapon_RocketLauncher, Amount = 1,},
	Item3 = { Type = CMC_Tool_BlowTorch       , Amount = 1,},
	Item4 = { Type = CMC_Tool_Ammo_Bag        , Amount = 1,}, // FIXME - Has to be denied if the rule "no ammo" is active
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
