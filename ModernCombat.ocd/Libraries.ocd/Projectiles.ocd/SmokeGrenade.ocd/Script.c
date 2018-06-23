#include CMC_Library_GrenadeProjectile

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local Grenade_SecureDistance = 0;
local Grenade_FuseTime = 120;
local Grenade_SmokeColor = CMC_Grenade_Smoke.Grenade_SmokeColor;

func OnDetonation()
{
	Call(CMC_Grenade_Smoke.OnDetonation);
}
