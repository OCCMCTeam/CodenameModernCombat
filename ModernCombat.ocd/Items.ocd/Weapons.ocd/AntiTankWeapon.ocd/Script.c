#include CMC_Firearm_Basic

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;

func SelectionTime() { return 55; }

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeMissiles_TechniqueOptical());
	AddFiremode(FiremodeMissiles_TechniqueUnguided());

	StartLoaded();
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
public func GetCarryBone() { return "Grip"; }
public func GetCarryTransform(object clonk, bool not_selected, bool nohand, bool onback)
{
	if (not_selected || !IsUserReadyToUse(clonk)) // On belt?
	{
		return Trans_Mul(Trans_Translate(0, 200), Trans_Rotate(90, 1, 0, 0));
	}
	else
	{
		return Trans_Mul(Trans_Rotate(90, 1, 0, 0), Trans_Rotate(5, 0, 0, 1), Trans_Translate(-2500, 500, 0));
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
	->SetAmmoAmount(1)
	->SetCooldownDelay(200)
	->SetRecoveryDelay(1)
	->SetReloadDelay(150)
	->SetDamage(0)
	// Projectile
	->SetProjectileID(CMC_Projectile_Bullet) // FIXME - obviously
	->SetProjectileSpeed(100)
	->SetProjectileRange(10000)
	->SetProjectileDistance(12)
	->SetYOffset(-6)
	// Spread
	->SetSpreadPerShot(ProjectileDeviationCmc(300))
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::AssaultRifle::Fire?")
	->SetHipShootingDisabled(true);

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

/* --- Effects --- */

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	// Nothing
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
