#include CMC_Firearm_Basic

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeBullets_TechniqueBurst());
	AddFiremode(FiremodeBullets_TechniqueSingle());
}

func Definition(id weapon)
{
	weapon.PictureTransformation = Trans_Mul(Trans_Rotate(-20, 0, 1, 0), Trans_Rotate(-20, 0, 0, 1), Trans_Rotate(5, 1, 0, 0), Trans_Translate(-1800, 0, -3000));
	weapon.MeshTransformation = Trans_Mul(Trans_Scale(2200), Trans_Rotate(180, 0, 1, 0), Trans_Translate(1100));
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
		return CARRY_Hand;
	}
}
public func GetCarryBone() { return "grip"; }
public func GetCarryTransform(object clonk, bool not_selected, bool nohand, bool onback)
{
	if (not_selected || !IsUserReadyToUse(clonk)) // On belt?
	{
		return Trans_Mul(this.MeshTransformation, Trans_Translate(0, 200), Trans_Rotate(90, 1, 0, 0));
	}
	else
	{
		return Trans_Mul(this.MeshTransformation, Trans_Rotate(-90, 1, 0, 0), Trans_Rotate(-90, 0, 0, 1), Trans_Translate(0, -1500, 0));
	}
}

/* --- Fire modes --- */

func FiremodeBullets()
{
	var mode = new Library_Firearm_Firemode {};

	// Reloading
	mode->SetAmmoID(CMC_Ammo_Bullets);
	mode.ammo_load = 30;
	mode->SetCooldownDelay(13);
	mode->SetRecoveryDelay(3);
	mode->SetReloadDelay(90);
	mode->SetDamage(14);

	// Projectile
	mode->SetProjectileID(CMC_Projectile_Bullet);
	mode->SetProjectileSpeed(270);
	mode->SetProjectileRange(800);
	mode->SetProjectileDistance(12);
	mode->SetYOffset(-6);

	// Ironsight aiming
	mode->SetIronsightType(WEAPON_FM_IronsightBlend);
	mode->SetIronsightDelay(15);
	mode->SetIronsightAimingAnimation("AimArmsGeneric.R");
	mode->SetForwardWalkingSpeed(95);
	mode->SetBackwardWalkingSpeed(65);
	
	// Spread
	mode->SetSpreadPerShot(ProjectileDeviationCmc(55));
	mode->SetSpreadBySelection(ProjectileDeviationCmc(100));
	mode->SetSpreadLimit(ProjectileDeviationCmc(450));

	// Crosshair, CMC Custom
	mode->SetAimCursor(CMC_Cursor_Default);

	// Effects, CMC custom
	mode->SetFireSound("Weapon::AssaultRifle::Fire", 3);
	return mode;
}

func FiremodeBullets_TechniqueSingle()
{
	var mode = FiremodeBullets();
	
	// Generic info
	mode->SetName("$Single$");
	mode->SetMode(WEAPON_FM_Single);
	return mode;
}


func FiremodeBullets_TechniqueBurst()
{
	var mode = FiremodeBullets();
	
	// Generic info
	mode->SetName("$Burst$");
	mode->SetMode(WEAPON_FM_Burst);
	mode->SetBurstAmount(3);
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

	EffectMuzzleFlash(user, x, y, angle, 20, false, true);
	
	// Casing
	x = +Sin(angle, firemode->GetProjectileDistance() / 3);
	y = -Cos(angle, firemode->GetProjectileDistance() / 3) + firemode->GetYOffset();
	var xdir = user->GetCalcDir() * 14 * RandomX(-2, -1);
	var ydir = RandomX(-11, -13);

	CreateCartridgeEffect("Cartridge_Pistol", 2, x, y, user->GetXDir() + xdir, user->GetYDir() + ydir);
}