#include CMC_Library_GrenadeItem

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local ContactIncinerate = 1;
local NoBurnDecay = 1;
local Collectible = true;

local Grenade_SmokeColor = 0xffccff00;
local Grenade_FuseTime = 70;
local Grenade_ContainedDamage = 20;

local smoke_progress = 1;


/* --- Sounds --- */


func PlaySoundActivate()
{
	Sound("Items::Grenades::SmokeGrenade::Activate");
}


func PlaySoundDetonation()
{
	Sound("Items::Grenades::SmokeGrenade::Fused");
}


func PlaySoundHit()
{
	Sound("Items::Grenades::SmokeGrenade::Hit?", {multiple = true});
}

/* --- Overloads --- */


func OnDetonation()
{
	ScheduleCall(this, this.ExpelSmoke, 3, 10);

	CastSmokeParticles("Smoke", 10, 50, 0, 0, PV_Random(65, 85), Particles_GrenadeSmoke(60, 70));

	FadeOut(35, true);
}


/* --- Smoke effect --- */

func ExpelSmoke()
{
	CastSmokeParticles("Smoke", 2, 10, 0, 0, 85, Particles_GrenadeSmoke(40, 60));

	var smoke = CreateObjectAbove(CMC_Grenade_SmokeHelper, 0, 0, GetController());
	// Original code, did not yield as much effect as I liked
	//var prec = 100;
	//smoke->SetXDir((2 * GetXDir(prec) / 3) + (smoke_progress * RandomX(-30, +30)), prec);
	//smoke->SetYDir((2 * GetYDir(prec) / 3) - (smoke_progress * RandomX(+10, +20)), prec);
	smoke->SetXDir((2 * GetXDir() / 3) + (smoke_progress * RandomX(-6, +6)));
	smoke->SetYDir((2 * GetYDir() / 3) - (smoke_progress * RandomX(+1, +3)));

	++smoke_progress;
}

func Particles_GrenadeSmoke(int size_min, int size_max)
{
	var angle_start = RandomX(-180, 180);
	var angle_finish = angle_start + RandomX(-180, 180);
	var heavy = !Random(4);
	return 
	{
		Prototype = Particles_Smoke(heavy),
		Size = PV_Linear(size_min, size_max),
		Rotation = PV_Linear(angle_start, angle_finish),
	};
}

