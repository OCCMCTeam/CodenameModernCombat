#include CMC_Library_GrenadeItem
#include Library_ObjectLimit


/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local ContactIncinerate = 1;
local NoBurnDecay = 1;
local Collectible = true;

local Grenade_SmokeColor = nil; // No smoke
local Grenade_FuseTime = 60;
local Grenade_ThrowSpeed = 22;
local Grenade_MaxDamage = 20;
local Grenade_ContainedDamage = 10;
local Sensor_Distance = 190;

local sensor_active = false;
local sensor_defuse_progress = 0;

local ObjectLimitPlayer = 2;

public func IsDetectable()	{return false;} // TODO: Cannot be found by other sensors

public func IsDefusable()	{return IsActive();} // TODO: No idea who can defuse something at the moment

/* --- Sounds --- */

func PlaySoundActivate()
{
	Sound("Items::Grenades::SensorBall::Activate");
}

func PlaySoundBeep()
{
	Sound("Items::Grenades::SensorBall::Beep");
}

func PlaySoundDetonation()
{
	Sound("Items::Grenades::SensorBall::Fused");
	Sound("Items::Grenades::SensorBall::Scan", {volume = 50, loop_count = +1});
}

func PlaySoundHit()
{
	Sound("Items::Tools::BoobyTrap::Hit?", {multiple = true});
}

/* --- Engine callback --- */


// TODO: Collection should not be possible after launching it?
func Entrance(object into)
{
	SetController(into->GetController());
}

/* --- Activation --- */

func Fuse()
{
	if (!IsActive())
	{
		Flash(this.Grenade_FuseTime);
	}
	return inherited();
}

public func HandleTrail()
{
	// Nothing, we do not want smoke here
}


func OnDetonation()
{
	sensor_active = true;

	Flash();

	// Permanent light
	SetLightColor(GetPlayerColor(GetController()));
	SetLightRange(this.Sensor_Distance, 40);
	
	// Light flash
	var flash_colors = SplitRGBaValue(GetPlayerColor(GetController()));
	var flash =
	{
		Prototype = Particles_Flash(),
		Size = PV_KeyFrames(0, 0, 0, 100, this.Sensor_Distance, 1000, 0),
		R = flash_colors.R,
		G = flash_colors.G,
		B = flash_colors.B,
		Attach = ATTACH_Front | ATTACH_MoveRelative,
	};
	CreateParticle("Flash", 0, 0, 0, 0, 15, flash);

	ScheduleCall(this, this.DestroySensor, 40 * 38); // 1200 + 320 = 1520; 

	AddTimer(this.Sense, 5);
}


func OnMaxDamage()
{
	DestroySensor();
}

/* --- Detect hostile movement --- */

func Sense ()
{
	if (GetController() == NO_OWNER)
	{
		return RemoveObject();
	}

	var menaces = FindObjects(Find_Distance(Sensor_Distance),
                                  Find_Exclude(this),
                                  Find_NoContainer(),
                                  Find_Or(Find_OCF(OCF_Alive), Find_Func("IsDetectable")));

	for (var menace in menaces)
	{
		if (Hostile(GetController(), menace->GetController()))
		{
			Beep();
			var tag = CMC_Icon_SensorBall_Tag->Get(menace, GetController(), CMC_Grenade_SensorBall);
			if (tag)
			{
				tag->~RefreshRemoveTimer(this);
			}
			else
			{
				CMC_Icon_SensorBall_Tag->AddTo(menace, GetController(), CMC_Grenade_SensorBall);

				//TODO: Achievement-Fortschritt (Intelligence)
				//DoAchievementProgress(1, AC21, GetOwner());
			}
		}
	}
}



func Beep()
{
	if (GetEffect("IntNoBeep", this))
	{
		return;
	}
	var beep_interval = 50;
	AddEffect("IntNoBeep", this, 1, beep_interval, this);

	Flash(beep_interval / 2);
	PlaySoundBeep();

	CMC_Icon_SensorBall_Circle->AddTo(this);
}

/* Entfernung */

func DestroySensor()
{
	//Effekte
/*	Sparks(2,RGB(250,100));
	Sparks(2,RGB(0,200));
	if(GetEffectData(EFSM_ExplosionEffects) > 0) CastSmoke("Smoke3",4, 10, 0, 0, 120, 140, RGBa(255,255,255,100), RGBa(255,255,255,130));
	  Sound("Limitation.ogg");*/
	// TODO: Should not be necessary upon removal Sound("SNSR_Scan.ogg", false, this, 50, 0, -1);

	ExplosionEffect(5, 0, 0, 0, true);
	RemoveObject();
}


/* --- Defuse --- */

func RTDefuse() // TODO: No idea who calls this; Update: Blow torch :)
{
	++sensor_defuse_progress;
	if (sensor_defuse_progress > 8)
	{
		// Sichtobjekte dekorativ entfernen
		/*
		var pObj;
		while(pObj = Contents(0,this()))
		{
		Exit(pObj);
		Schedule("SetPlrViewRange(--Local(0)*10,this(),1)",1,19,pObj);
		Schedule("RemoveObject(this())",20,1,pObj);
		}*/

		// This is the same as OnMaxDamage destruction.
		// The normal destruction works a little different, with the limitation sound and no deco explosion
/*
		Sound("MISL_ShotDown.ogg");
		Sound("SNSR_Scan.ogg", false, this, 50, 0, -1);
		DecoExplode(10);
*/
		return true;
	}

	return false;
}


func Flash(int lifetime)
{
	CreateLEDEffect(GetPlayerColor(GetController()), 0, 0, 8, lifetime, true);
}
