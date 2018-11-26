#include CMC_Library_GrenadeProjectile

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local Grenade_SmokeColor = CMC_Grenade_Field.Grenade_SmokeColor;

/* --- Get functionality from field grenade --- */

func Definition(id type)
{
	this.OnDetonation = CMC_Grenade_Field.OnDetonation;
}
