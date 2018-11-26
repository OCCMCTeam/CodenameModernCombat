#include CMC_Library_GrenadeProjectile

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local Grenade_FuseTime = 80;
local Grenade_SmokeColor = CMC_Grenade_Frag.Grenade_SmokeColor;

func OnDetonation()
{
	Call(CMC_Grenade_Frag.OnDetonation);
}

/* --- Get functionality from frag grenade --- */

func Definition(id type)
{
	this.OnDetonation = CMC_Grenade_Frag.OnDetonation;
	this.LaunchShrapnel = CMC_Grenade_Frag.LaunchShrapnel;
	this.ShrapnelDamage = CMC_Grenade_Frag.ShrapnelDamage;
}
