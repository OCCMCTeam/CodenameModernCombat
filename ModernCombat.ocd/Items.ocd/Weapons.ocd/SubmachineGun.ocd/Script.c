#include CMC_Firearm_Basic

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;

func SelectionTime() { return 30; }

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeBullets_TechniqueAuto());
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
		return CARRY_Blunderbuss;
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
		return Trans_Mul(this.MeshTransformation, Trans_Rotate(-90, 1, 0, 0), Trans_Rotate(-10, 0, 0, 1));
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

	mode->SetCMCDefaults()
	// Reloading
	->SetAmmoID(CMC_Ammo_Bullets)
	->SetCooldownDelay(3)
	->SetRecoveryDelay(3)
	->SetReloadDelay(70)
	->SetDamage(6)
	// Projectile
	->SetProjectileID(CMC_Projectile_Bullet)
	->SetProjectileSpeed(270)
	->SetProjectileRange(550)
	->SetProjectileDistance(12)
	->SetYOffset(-6)
	// Spread
	->SetSpreadPerShot(ProjectileDeviationCmc(20))
	->SetSpreadBySelection(ProjectileDeviationCmc(110))
	->SetSpreadLimit(ProjectileDeviationCmc(410))
	->SetProjectileSpread(ProjectileDeviationCmc(60)) // Min spread
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::SubmachineGun::Fire?");

	mode.ammo_load = 30;

	return mode;
}

func FiremodeBullets_TechniqueAuto()
{
	var mode = FiremodeBullets();

	// Generic info
	mode->SetName("$Auto$")
	->SetMode(WEAPON_FM_Auto);

	return mode;
}

func FiremodeBullets_TechniqueBurst()
{
	var mode = FiremodeBullets();

	// Generic info
	mode->SetName("$Burst$")
	->SetMode(WEAPON_FM_Burst)
	// Modifications
	->SetBurstAmount(3)
	->SetCooldownDelay(mode->GetCooldownDelay() + 3)
	->SetRecoveryDelay(2)
	->SetSpreadPerShot(mode->GetSpreadPerShot() + ProjectileDeviationCmc(5));

	return mode;
}

func FiremodeBullets_TechniqueSingle()
{
	var mode = FiremodeBullets();

	// Generic info
	mode->SetName("$Single$")
	->SetMode(WEAPON_FM_Single)
	// Modifications
	->SetDamage(mode->GetDamage() + 6)
	->SetCooldownDelay(mode->GetCooldownDelay() + 5)
	->SetSpreadPerShot(mode->GetSpreadPerShot() + ProjectileDeviationCmc(30))
	->SetProjectileSpread(ProjectileDeviationCmc(20));

	return mode;
}

/* --- Effects --- */

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
