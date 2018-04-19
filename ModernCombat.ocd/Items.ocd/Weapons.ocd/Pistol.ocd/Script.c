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
	AddFiremode(FiremodeStandard());
}

func Definition(id def)
{
	def.PictureTransformation = Trans_Mul(Trans_Rotate(-20, 0, 1, 0), Trans_Rotate(-20, 0, 0, 1), Trans_Rotate(5, 1, 0, 0), Trans_Translate(-1800, 0, -3000));
	def.MeshTransformation = Trans_Scale(800);
}

/* --- Display --- */

public func GetCarryMode(object clonk, bool idle)
{
	if (!idle || this.is_in_ironsight)
	{
		return CARRY_Hand;
	}
	else
	{
		return CARRY_Belt;
	}
}
public func GetCarryBone() { return "Grip"; }
public func GetCarryTransform(object clonk, bool idle, bool nohand, bool onback)
{
	if (idle) return;

	var trans_size = Trans_Scale(500);
	if (clonk->~IsWalking() || clonk->~IsJumping())
	{
		return Trans_Mul(trans_size, Trans_Rotate(90, 1), Trans_Rotate(90, 0, 0, 1));
	}
	else
	{
		return Trans_Mul(trans_size, Trans_Rotate(140, 1), Trans_Translate(0, 0, -2500));
	}

}

/* --- Fire modes --- */

func FiremodeStandard()
{
	var mode = new Library_Firearm_Firemode {};

	// Generic info
	mode->SetName("$Bullets$");
	mode->SetMode(WEAPON_FM_Single);

	// Reloading
	mode->SetAmmoID(CMC_Ammo_Bullets);
	mode.ammo_load = 15; //FIXME: This should have a getter, too, most likely in the library (because one of the plugins uses it)
	mode->SetRecoveryDelay(5);
	mode->SetReloadDelay(40);
	mode->SetDamage(11);

	// Projectile
	mode->SetProjectileID(CMC_Projectile_Bullet);
	mode->SetProjectileSpeed(250);
	mode->SetProjectileRange(450);
	mode->SetProjectileDistance(8);
	mode->SetYOffset(-6);

	// Ironsight aiming
	mode->SetIronsightType(WEAPON_FM_IronsightBlend);
	mode->SetIronsightDelay(15);
	mode->SetAimingAnimation("AimArmsGeneric.R");
	mode->SetForwardWalkingSpeed(95);
	mode->SetBackwardWalkingSpeed(65);

	// Effects, CMC custom
	mode->SetFireSound("Weapon::Pistol::Fire", 2);
	return mode;
}

/* --- Effects --- */

func FireSound(object user, proplist firemode)
{
	Sound(firemode->GetCurrentFireSound());
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

	//EffectMuzzleFlash(user, x, y, angle, 20, false, true);
	
	// Casing
	x = +Sin(angle, firemode->GetProjectileDistance() / 2);
	y = -Cos(angle, firemode->GetProjectileDistance() / 2) +  + firemode->GetYOffset();

	CreateCartridgeEffect("Cartridge_Pistol", 2, x, y, user->GetXDir() + Sin(-angle, 5), user->GetYDir() - RandomX(15, 20));
}
