#include CMC_Library_GrenadeItem

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
	Sound("Items::Grenades::StunGrenade::Activate");
}

func PlaySoundDetonation()
{
	Sound("Items::Grenades::StunGrenade::Explosion");
}

func PlaySoundHit()
{
	Sound("Items::Grenades::StunGrenade::Hit?", {multiple = true});
}

func PlaySoundFlashbang(object target)
{
	var for_player = target->GetOwner();
	target->Sound("Items::Grenades::StunGrenade::Bang?", { player = for_player});
	target->Sound("Items::Grenades::StunGrenade::EarRinging?", { player = for_player});
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
	
	Flashbang();
	FadeOut(35, true);
}

func Flashbang()
{
	var flash =
	{
		Prototype = Particles_Flash(),
		Size = PV_KeyFrames(0, 0, 0, 100, 4 * this.Grenade_Radius / 5, 1000, 0),
	};
	
	var angles = GetPathFreeAngles(10, 5);
	
	for (var i = 0; i < 100; ++i)
	{
		var radius = RandomX(this.Grenade_Radius / 20, this.Grenade_Radius / 5);
		var angle = angles[Random(GetLength(angles))];

		var x = +Sin(angle, radius);
		var y = -Cos(angle, radius);
		var xdir = x / 2;
		var ydir = y / 2;
		
		CreateParticle("MagicFire", x, y, PV_Random(xdir - 10, xdir + 10), PV_Random(ydir - 10, ydir + 10), PV_Random(10, 40), Particles_Glimmer(), 1);
	}
	
	CreateParticle("Flash", 0, 0, 0, 0, 20, flash);
}
