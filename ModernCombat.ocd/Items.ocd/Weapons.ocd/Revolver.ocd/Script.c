#include CMC_Firearm_Basic

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;

func SelectionTime() { return 10; }

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeBullets_TechniqueSingle());
}

func Definition(id def)
{
	def.PictureTransformation = Trans_Mul(Trans_Rotate(-20, 0, 1, 0), Trans_Rotate(-20, 0, 0, 1), Trans_Rotate(5, 1, 0, 0), Trans_Translate(-1800, 0, -3000));
	def.MeshTransformation = Trans_Scale(500);
}

/* --- Display --- */

public func GetCarryMode(object clonk, bool idle) // FIXME - maybe "idle" is a bad description? Currently it means that the item is not the active item that the player is using
{
	if (idle || !IsUserReadyToUse(clonk))
	{
		return CARRY_Belt; // Mesh seems not to be attached to the Clonk in this carry mode
	}
	else
	{
		return CARRY_Hand;
	}
}
public func GetCarryBone() { return "Grip"; }
public func GetCarryTransform(object clonk, bool idle, bool nohand, bool onback)
{
	if (idle || !IsUserReadyToUse(clonk)) // On belt?
	{
		return Trans_Mul(this.MeshTransformation);
	}
	else // FIXME - not necessary at the moment, this is covered by IsUserReadyToUse: if (clonk->~IsWalking() || clonk->~IsJumping())
	{
		return Trans_Mul(this.MeshTransformation, Trans_Rotate(90, 1), Trans_Rotate(90, 0, 0, 1));
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
	->SetCooldownDelay(15)
	->SetRecoveryDelay(1)
	->SetReloadDelay(90)
	->SetDamage(24)
	// Aiming
	->SetIronsightAimingAnimation("AimPistol")
	// Projectile
	->SetProjectileID(CMC_Projectile_Bullet)
	->SetProjectileSpeed(250)
	->SetProjectileRange(400)
	->SetProjectileDistance(8)
	->SetYOffset(-6)
	// Spread
	->SetSpread(ProjectileDeviationCmc(20))
	->SetSpreadPerShot(ProjectileDeviationCmc(110))
	->SetSpreadBySelection(ProjectileDeviationCmc(30))
	->SetSpreadLimit(ProjectileDeviationCmc(220))
	// Crosshair, CMC Custom
	->SetAimCursor(CMC_Cursor_Cone)
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::Pistol::Fire?");

	// FIXME: This should have a getter, too, most likely in the library (because one of the plugins uses it)
	mode.ammo_load = 6;

	return mode;
}

func FiremodeBullets_TechniqueSingle()
{
	var mode = FiremodeBullets();

	// Generic info
	mode->SetName("$Single$")
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

	EffectMuzzleFlash(user, x, y, angle, RandomX(40, 45), false, true);
}
