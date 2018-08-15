#include CMC_Firearm_Basic

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;

local RocketLauncher_Laser = nil; // Laser aim object
local RocketLauncher_Missile = nil; // The guided missile

func SelectionTime() { return 45; }

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeMissiles_TechniqueOptical());
	AddFiremode(FiremodeMissiles_TechniqueUnguided());
}

func Definition(id weapon)
{
	weapon.PictureTransformation = Trans_Mul(Trans_Rotate(235, 0, 1, 0), Trans_Rotate(30, 0, 0, 1), Trans_Translate(-2000, 0, -1000));
}

/* --- Display --- */

public func GetCarryMode(object clonk, bool not_selected)
{
	if (not_selected || !IsUserReadyToUse(clonk))
	{
		return CARRY_Back;
	}
	else
	{
		return CARRY_Blunderbuss;
	}
}
public func GetCarryBone() { return "grip"; }
public func GetCarryTransform(object clonk, bool not_selected, bool nohand, bool onback)
{
	if (not_selected || !IsUserReadyToUse(clonk)) // On belt?
	{
		return Trans_Mul(Trans_Translate(3000, 500, 3000), Trans_Rotate(90, 1, 0, 0));
	}
	else
	{
		return Trans_Rotate(90, 1, 0, 0);
	}
}
public func GetCarrySpecial(clonk)
{
	if(IsAiming()) return "pos_hand2";
}

/* --- Fire modes --- */

func FiremodeMissiles()
{
	var mode = new Library_Firearm_Firemode {};

	mode->SetCMCDefaults()
	// Generic info
	->SetMode(WEAPON_FM_Single)
	// Reloading
	->SetAmmoID(CMC_Ammo_Missiles)
	->SetCooldownDelay(200)
	->SetRecoveryDelay(1)
	->SetReloadDelay(150)
	->SetDamage(0)
	// Projectile
	->SetProjectileID(CMC_Projectile_Missile)
	->SetProjectileSpeed(100)
	->SetProjectileRange(10000)
	->SetProjectileDistance(12)
	->SetYOffset(-6)
	// Spread
	->SetSpreadPerShot(ProjectileDeviationCmc(300))
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::RocketLauncher::Fire?")
	->SetHipShootingDisabled(true);

	mode.ammo_load = 1;

	return mode;
}

func FiremodeMissiles_TechniqueOptical()
{
	var mode = FiremodeMissiles();

	// Generic info
	mode->SetName("$Optical$");

	return mode;
}

func FiremodeMissiles_TechniqueUnguided()
{
	var mode = FiremodeMissiles();

	// Generic info
	mode->SetName("$Unguided$");

	return mode;
}

/* --- Missile control --- */

// Called by the shooter library in ControlUseStart
func OnPressUse(object user, int x, int y)
{
	// Does nothing while missile is there
	if (this.RocketLauncher_Missile)
	{
		if (this.RocketLauncher_Missile->PossibleTracerLink())
		{
			this.RocketLauncher_Missile->ConfirmTracerLink();
		}
		return true;
	}
	
	return inherited(user, x, y, ...);
}

/* --- Optical aiming --- */

// Will happen always at the moment, will be restricted to optical aiming firemode
func AimOptical(object user, int x, int y)
{
	// No optical aiming while linked to a tracer
	if (this.RocketLauncher_Missile && this.RocketLauncher_Missile->HasTracerLink())
	{
		return AimOpticalReset();
	}

	var precision = 1000;

	// Start in global coordinates	
	var x_start = user->GetX();
	var y_start = user->GetY() - 5;
	
	var aim_angle = Angle(x_start, y_start, user->GetX() + x, user->GetY() + y, precision);
	
	var max_distance = Distance(0, 0, LandscapeWidth(), LandscapeHeight());
	
	var x_target = x_start + Sin(aim_angle, max_distance, precision);
	var y_target = y_start - Cos(aim_angle, max_distance, precision);
	
	// If path is free the target is OK - the distance was chosen in such a way that it always hits the border of a landscape
	var end = PathFree2(x_start, y_start, x_target, y_target) ?? [x_target, y_target];
	
	var x_end = end[0];
	var y_end = end[1];
		
	if (!this.RocketLauncher_Laser)
	{
		this.RocketLauncher_Laser = CreateObject(LaserEffect, 0, 0, user->GetController());
		this.RocketLauncher_Laser.Visibility = VIS_Owner;
		this.RocketLauncher_Laser
			 ->SetWidth(2)
			 ->Color(RGB(200, 0, 0))
			 ->Activate();
	}
	
	if (this.RocketLauncher_Laser)
	{
		this.RocketLauncher_Laser->SetPosition(x_start, y_start);
		this.RocketLauncher_Laser->Line(x_start, y_start, x_end, y_end)->Update();
	}
	
	if (this.RocketLauncher_Missile)
	{
		this.RocketLauncher_Missile->GuideTo(x_end, y_end);
	}
}


func AimOpticalReset()
{
	if (this.RocketLauncher_Laser)
	{
		this.RocketLauncher_Laser->RemoveObject();
	}
}


// Called by the CMC modified clonk, see ModernCombat.ocd\System.ocg\Mod_Clonk.c
public func ControlUseAiming(object user, int x, int y)
{
	if (IsAiming() && CanFiremodeGuide())
	{
		AimOptical(user, x, y);
	}
	else
	{
		AimOpticalReset();
	}
	return inherited(user, x, y);
}

public func StopAiming(object user)
{
	AimOpticalReset();
	return inherited(user, ...);
}

public func Reset(object user)
{
	AimOpticalReset();
	return inherited(user, ...);
}

func CanFiremodeGuide(proplist firemode)
{
	firemode = firemode ?? GetFiremode();
	return firemode && firemode->GetIndex() == 0;
}

/* --- Effects --- */

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	if (CanFiremodeGuide(firemode))
	{
		RocketLauncher_Missile = projectile;
		RocketLauncher_Missile->SetGuided(true);
	}
}

func FireEffect(object user, int angle, proplist firemode)
{
	var x = +Sin(angle, firemode->GetProjectileDistance());
	var y = -Cos(angle, firemode->GetProjectileDistance()) + firemode->GetYOffset();

	// Muzzle flash, the coordinates are inverted here, because the effect should be at the rear of the bazooka
	var xdir = -x;
	var ydir = -y;

	CreateParticle("Thrust", x, y, xdir/4, ydir/4, 16,
	{
		Prototype = Particles_Thrust(),
		R = PV_KeyFrames(0, 0, 255, 500, 100, 1000, 50),
		G = PV_KeyFrames(0, 0, 200, 500,  40, 1000, 20),
		B = PV_KeyFrames(0, 0, 200, 500,  40, 1000, 20),
	});

	for (var i = 0; i < 20; ++i)
	{
		var radius = RandomX(-10, 10);
		CreateParticle("Smoke2", x + Sin(angle, radius), y - Cos(angle, radius), PV_Random(0, xdir), PV_Random(0, ydir), PV_Random(30, 40),
		{
			Prototype = Particles_Thrust(),
			Size = PV_Random(16, 28),
			R = PV_KeyFrames(0, 0, 220, 500,  90, 1000, 45),
			G = PV_KeyFrames(0, 0, 200, 500,  80, 1000, 40),
			B = PV_KeyFrames(0, 0, 180, 500,  70, 1000, 35),
		}, 1);
	}
}
