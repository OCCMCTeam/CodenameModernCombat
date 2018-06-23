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
