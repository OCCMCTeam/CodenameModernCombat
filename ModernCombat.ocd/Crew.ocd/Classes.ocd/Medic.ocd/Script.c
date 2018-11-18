#include CMC_Library_Class

/* --- Inventory --- */

local Ammo = {
	CMC_Ammo_Bullets = 90,
};

local Items = {
	Item1 = { Type = CMC_Weapon_SubmachineGun, Amount = 1,},
	Item2 = { Type = CMC_Weapon_Pistol       , Amount = 1,},
	Item3 = { Type = CMC_Tool_Defibrillator  , Amount = 1,},
	Item4 = { Type = CMC_Tool_Medical_Bag    , Amount = 1,},
};

local Grenades = {
	CMC_Grenade_Field = 1,
	CMC_Grenade_Frag = 2,
	CMC_Grenade_SensorBall = 1,
};

/* --- Abilities --- */

local Abilities = [CMC_Ability_ImproveMedicalEquipment];

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
