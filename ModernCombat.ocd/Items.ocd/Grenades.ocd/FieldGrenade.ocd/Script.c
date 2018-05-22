#include CMC_Library_Grenade

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local ContactIncinerate = 1;
local NoBurnDecay = 1;
local Collectible = true;

local Grenade_ContainedDamage = 120;

/* --- Callbacks --- */

// What happens when the grenade explodes
public func OnDetonation()
{
	BlastObjects(GetX(), GetY(), 60, Contained(), GetController(), 80, GetObjectLayer());
	Explode(30, true);
}
