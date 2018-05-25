#include CMC_Library_Grenade

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local ContactIncinerate = 1;
local NoBurnDecay = 1;
local Collectible = true;

local Grenade_SmokeColor = 0xffebc736; // TODO: Should be 235, 199, 54
local Grenade_ContainedDamage = 100;

public func BlastRadius()	{return 10;}			//Explosionsradius

/* --- Sounds --- */

func PlaySoundActivate()
{
	Sound("Grenade::SmokeGrenade::Activate");
}

func PlaySoundDetonation()
{
	Sound("Grenade::PhosphorGrenade::Explosion");
	inherited();
}

func PlaySoundHit()
{
	Sound("Grenade::SmokeGrenade::Hit?", {multiple = true});
}

/* --- Detonation --- */

func OnDetonation()
{
	for (var i = 0; i < 8; ++i)
	{
		var phosphor = CreateObject(CMC_Grenade_PhosphorHelper, 0, 0, GetController());
		phosphor->SetSpeed(RandomX(-50, +50), RandomX(-50, +50));
	}
	Explosion([7, 15], [27 , 20]);
}
