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

	mode->SetCMCDefaults()
	// Reloading
	->SetAmmoID(CMC_Ammo_Bullets)
	->SetCooldownDelay(30)
	->SetRecoveryDelay(1)
	->SetReloadDelay(75)
	->SetDamage(10)
	// Projectile
	->SetProjectileAmount(5) // 5 per shot
	->SetProjectileID(CMC_Projectile_Bullet)
	->SetProjectileSpeed([250, 270])
	->SetProjectileRange(300)
	->SetProjectileDistance(12)
	->SetYOffset(-6)
	// Spread
	->SetSpreadPerShot(ProjectileDeviationCmc(150))
	->SetSpreadBySelection(ProjectileDeviationCmc(320))
	->SetSpreadLimit(ProjectileDeviationCmc(570))
	->SetProjectileSpread(ProjectileDeviationCmc(120))
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::Shotgun::Fire?")
	// Ammo name override, CMC custom
	->SetAmmoName("$Pellets$");

	mode.ammo_load = 20;

	return mode;
}

func FiremodeBullets_TechniqueSpreadshot()
{
	var mode = FiremodeBullets();

	// Generic info
	mode->SetName("$SpreadShot$")
	->SetMode(WEAPON_FM_Single);

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
