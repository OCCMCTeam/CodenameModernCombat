#include CMC_Library_GrenadeItem

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
	Sound("Items::Grenades::SmokeGrenade::Activate");
}

func PlaySoundDetonation()
{
	Sound("Items::Grenades::PhosphorGrenade::Explosion");
	inherited();
}

func PlaySoundHit()
{
	Sound("Items::Grenades::SmokeGrenade::Hit?", {multiple = true});
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


// Overloads the explosion effect for a blueish explosion
func ExplosionEffect(int level, int x, int y, int smoothness, bool silent, int damage_level)
{
	var glimmer =
	{
		Prototype = Particles_Glimmer(),
		R = PV_Random(100, 150),
		G = PV_Random(100, 150),
		B = PV_Random(200, 255)
	};
	var fire =
	{
		Prototype = Particles_Fire(),
		R = PV_Random(100, 150),
		G = PV_Random(100, 150),
		B = PV_Random(200, 255),
		Alpha = PV_KeyFrames(0, 0, 128, 500, 128, 1000, 0),
		Size = PV_Random(level * 3, level * 5)
	};
	var smoke =
	{
		Prototype = Particles_SmokeTrail(),
		R = PV_Linear(PV_Random(100, 150), 0),
		G = PV_Linear(PV_Random(100, 150), 0),
		B = PV_Linear(PV_Random(200, 255), 0),
		Alpha = PV_Linear(32, 0),
		Size = PV_Random(level - 5, level + 5)
	};
	
	// Blast particle.
	CreateParticle("Smoke", x, y, PV_Random(-2, 2), PV_Random(-2, 2), PV_Random(20, 40), smoke, 10);
	CreateParticle("MagicFire", x, y, PV_Random(-20, 20), PV_Random(-20, 20), PV_Random(5, 10), fire, 20);
	if (GBackLiquid(x,y))
	{
		CastObjects(Fx_Bubble, level * 4 / 10, level, x, y);
	}
	else
	{
		 CreateParticle("SphereSpark", x, y, PV_Random(-100, 100), PV_Random(-100, 100), PV_Random(5, 36 * 3), glimmer, level);
	}
	return;
}
