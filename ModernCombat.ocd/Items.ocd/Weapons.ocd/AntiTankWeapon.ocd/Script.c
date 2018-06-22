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
	
	// Generic info
	mode->SetMode(WEAPON_FM_Single);

	// Reloading
	mode->SetAmmoID(CMC_Ammo_Missiles);
	mode.ammo_load = 1;
	mode->SetCooldownDelay(200);
	mode->SetRecoveryDelay(1);
	mode->SetReloadDelay(150);
	mode->SetDamage(0);

	// Projectile
	mode->SetProjectileID(CMC_Projectile_Bullet); // FIXME - obviously
	mode->SetProjectileSpeed(100);
	mode->SetProjectileRange(10000);
	mode->SetProjectileDistance(12);
	mode->SetYOffset(-6);

	// Ironsight aiming
	mode->SetIronsightType(WEAPON_FM_IronsightBlend);
	mode->SetIronsightDelay(15);
	mode->SetIronsightAimingAnimation("MusketAimArms");
	mode->SetForwardWalkingSpeed(95);
	mode->SetBackwardWalkingSpeed(65);
	
	// Spread
	mode->SetSpreadPerShot(ProjectileDeviationCmc(300));

	// Crosshair, CMC Custom
	mode->SetAimCursor([CMC_Cursor_Cone]);

	// Effects, CMC custom
	mode->SetFireSound("Items::Weapons::AssaultRifle::Fire", 3);
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

func FireSound(object user, proplist firemode)
{
	Sound(firemode->GetCurrentFireSound(), {multiple = true});
}

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
