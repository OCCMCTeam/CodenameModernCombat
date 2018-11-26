#include CMC_Library_GrenadeProjectile

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local Grenade_SecureDistance = 0;
local Grenade_FuseTime = 120;
local Grenade_SmokeColor = CMC_Grenade_Smoke.Grenade_SmokeColor;

/* --- Get functionality from smoke grenade --- */

local smoke_progress = 1;

func Definition(id type)
{
	this.OnDetonation = CMC_Grenade_Smoke.OnDetonation;
	this.ExpelSmoke = CMC_Grenade_Smoke.ExpelSmoke;
	this.Particles_GrenadeSmoke = CMC_Grenade_Smoke.Particles_GrenadeSmoke;
}
