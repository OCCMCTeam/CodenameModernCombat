#include CMC_Library_Grenade

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local ContactIncinerate = 1;
local NoBurnDecay = 1;
local Collectible = true;
/*-- Blendgranate --*/

/* --- Properties --- */

local Grenade_SmokeColor = 0xff00ccff;
local Grenade_ContainedDamage = 20;
local Grenade_Radius = 250;

/* --- Sounds --- */

func PlaySoundActivate()
{
	Sound("Grenade::StunGrenade::Activate");
}

func PlaySoundDetonation()
{
	Sound("Grenade::StunGrenade::Explosion");
}

func PlaySoundHit()
{
	Sound("Grenade::StunGrenade::Hit?", {multiple = true});
}

func PlaySoundFlashbang(object target)
{
	var for_player = target->GetOwner();
	target->Sound("Grenade::StunGrenade::Bang?", { player = for_player});
	target->Sound("Grenade::StunGrenade::EarRinging?", { player = for_player});
}

/* --- Detonation --- */


func OnDetonation()
{
	var full_range = this.Grenade_Radius;
	var short_range = full_range / 5;

	var targets = FindObjects(Find_Distance(full_range), Find_Func(GetFunctionName(CMC_Library_AffectedByStunGrenade.IsAffectedByStunGrenade), this, full_range, short_range));
	for (var target in targets)
	{
		// Targets in helicopters can be blinded (TODO: Replace this by better criteria!)
		/*var container = target->Contained();
		if (container && !container->~IsHelicopter())
		{
			continue;
		}*/

		// Now blind them, determine how much; allies are blinded half as much only
		var distance = ObjectDistance(target);
		var intensity = (400 - distance) * 300 / full_range; // TODO: The numbers behind the calculation were taken from the previous implementation, no idea what they mean
		if (!Hostile(GetController(), target->GetOwner()))
		{
			intensity /= 2;
		}
		if (intensity > 0)
		{
			target->BlindedByStunGrenade(this, intensity);
		};
	}
}
