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
	AddFiremode(FiremodeStandard());
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

func FiremodeStandard()
{
	var mode = new Library_Firearm_Firemode {};

	// Generic info
	mode->SetName("$Single$");
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

	// Hip shooting
	mode->SetHipFireAimingAnimation("AimArmsGeneric.R");

	// Ironsight aiming
	mode->SetIronsightType(WEAPON_FM_IronsightBlend);
	mode->SetIronsightDelay(15);
	mode->SetIronsightAimingAnimation("AimPistol");
	mode->SetForwardWalkingSpeed(95);
	mode->SetBackwardWalkingSpeed(65);
	
	// Spread
	mode->SetSpread(ProjectileDeviationCmc(20));
	mode->SetSpreadPerShot(ProjectileDeviationCmc(70));
	mode->SetSpreadBySelection(ProjectileDeviationCmc(30));
	mode->SetSpreadLimit(ProjectileDeviationCmc(220));
	
	// Crosshair, CMC Custom
	mode->SetAimCursor(CMC_Cursor_Cone);

	// Effects, CMC custom
	mode->SetFireSound("Items::Weapons::Pistol::Fire", 2);
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
	x = +Sin(angle, firemode->GetProjectileDistance() / 2);
	y = -Cos(angle, firemode->GetProjectileDistance() / 2) +  + firemode->GetYOffset();

	CreateCartridgeEffect("Cartridge_Pistol", 2, x, y, user->GetXDir() + Sin(-angle, 5), user->GetYDir() - RandomX(15, 20));
}
