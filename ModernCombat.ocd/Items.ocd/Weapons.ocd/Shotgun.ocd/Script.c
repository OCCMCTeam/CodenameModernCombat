#include CMC_Firearm_Basic

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;

func SelectionTime() { return 20; }

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeBullets_TechniqueSpreadshot());
}

func Definition(id weapon)
{
	weapon.PictureTransformation = Trans_Mul(Trans_Rotate(230, 0, 1, 0), Trans_Rotate(-15, 0, 0, 1), Trans_Rotate(10, 1, 0, 0), Trans_Translate(-2500, 1000, -1000));
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
		return Trans_Mul(Trans_Rotate(90, 1, 0, 0), Trans_Translate(-3000, 1500, 0));
	}
}
public func GetCarrySpecial(clonk)
{
	if(IsAiming()) return "pos_hand2";
}

/* --- Fire modes --- */

func FiremodeBullets()
{
	var mode = new Library_Firearm_Firemode {};

	// Reloading
	mode->SetAmmoID(CMC_Ammo_Bullets);
	mode.ammo_load = 20;
	mode->SetCooldownDelay(30);
	mode->SetRecoveryDelay(1);
	mode->SetReloadDelay(75);
	mode->SetDamage(10);

	// Projectile
	mode->SetProjectileAmount(5); // 5 per shot
	mode->SetProjectileID(CMC_Projectile_Bullet);
	mode->SetProjectileSpeed([250, 270]);
	mode->SetProjectileRange(300);
	mode->SetProjectileDistance(12);
	mode->SetYOffset(-6);

	// Ironsight aiming
	mode->SetIronsightType(WEAPON_FM_IronsightBlend);
	mode->SetIronsightDelay(15);
	mode->SetIronsightAimingAnimation("MusketAimArms");
	mode->SetForwardWalkingSpeed(95);
	mode->SetBackwardWalkingSpeed(65);
	
	// Spread
	mode->SetSpreadPerShot(ProjectileDeviationCmc(150));
	mode->SetSpreadBySelection(ProjectileDeviationCmc(320));
	mode->SetSpreadLimit(ProjectileDeviationCmc(570));
	mode->SetProjectileSpread(ProjectileDeviationCmc(120));

	// Crosshair, CMC Custom
	mode->SetAimCursor([CMC_Cursor_Cone, CMC_Cursor_Default]);

	// Effects, CMC custom
	mode->SetFireSound("Items::Weapons::Shotgun::Fire", 2);

	// Ammo name override, CMC custom
	mode->SetAmmoName("$Pellets$");
	return mode;
}

func FiremodeBullets_TechniqueSpreadshot()
{
	var mode = FiremodeBullets();
	
	// Generic info
	mode->SetName("$SpreadShot$");
	mode->SetMode(WEAPON_FM_Single);
	return mode;
}


/* --- Effects --- */

func FireSound(object user, proplist firemode)
{
	Sound(firemode->GetCurrentFireSound(), {multiple = true});
}

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	projectile->Trail(2, 150); // FIXME: Is not visible in hitscan :/
	projectile->HitScan();
}

func FireEffect(object user, int angle, proplist firemode)
{
	// Muzzle flash
	var x = +Sin(angle, firemode->GetProjectileDistance());
	var y = -Cos(angle, firemode->GetProjectileDistance()) + firemode->GetYOffset();

	EffectMuzzleFlash(user, x, y, angle, RandomX(40, 55), false, true);
	
	// Casing
	ScheduleCall(this, this.EjectCasing, 5, 1, user, angle, firemode);
}

func EjectCasing(object user, int angle, proplist firemode)
{
	var x = +Sin(angle, firemode->GetProjectileDistance() / 3);
	var y = -Cos(angle, firemode->GetProjectileDistance() / 3) + firemode->GetYOffset();
	var xdir = user->GetCalcDir() * 8 * RandomX(-2, -1);
	var ydir = RandomX(-11, -13);

	CreateCartridgeEffect("Cartridge_Pistol", 4, x, y, user->GetXDir() + xdir, user->GetYDir() + ydir);
}
