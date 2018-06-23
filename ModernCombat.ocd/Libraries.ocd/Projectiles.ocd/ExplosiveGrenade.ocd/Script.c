#include CMC_Library_GrenadeProjectile

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local Grenade_SmokeColor = 0xffdd0000;

func OnDetonation()
{
	Call(CMC_Grenade_Field.OnDetonation);
}
