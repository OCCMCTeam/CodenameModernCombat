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
	AddFiremode(FiremodeBullets_TechniqueTracerDart());
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
	if (IsAiming()) return "pos_hand2";
}

/* --- Fire modes --- */

func FiremodeBullets()
{
	var mode = new Library_Firearm_Firemode {};

	mode->SetCMCDefaults()
	// Generic info
	->SetMode(WEAPON_FM_Single)
	// Reloading
	->SetAmmoID(CMC_Ammo_Bullets)
	->SetRecoveryDelay(5)
	// Projectile
	->SetProjectileDistance(8)
	->SetYOffset(-6)
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::Pistol::Fire?");

	// TODO: Update shooter library and incorporate into above
	mode->SetForwardWalkingSpeed(95);
	mode->SetBackwardWalkingSpeed(65);

	return mode;
}

func FiremodeBullets_TechniqueSingle()
{
	var mode = FiremodeBullets();

	// Generic info
	mode->SetName("$Single$");
	mode->SetProjectileID(CMC_Projectile_Bullet);
	
	// Reloading
	mode.ammo_load = 15; //FIXME: This should have a getter, too, most likely in the library (because one of the plugins uses it)
	mode->SetReloadDelay(40);
	mode->SetDamage(11);
	
	// Projectile
	mode->SetProjectileSpeed(250);
	mode->SetProjectileRange(450);
	
	// Spread
	mode->SetSpread(ProjectileDeviationCmc(20));
	mode->SetSpreadPerShot(ProjectileDeviationCmc(70));
	mode->SetSpreadBySelection(ProjectileDeviationCmc(30));
	mode->SetSpreadLimit(ProjectileDeviationCmc(220));
	
	// Identification
	mode.IsTracer = false;
	return mode;
}


func FiremodeBullets_TechniqueTracerDart()
{
	var mode = FiremodeBullets();

	// Generic info
	mode->SetName("$TracerDart$");
	mode->SetProjectileID(CMC_Projectile_TracerDart);

	// Reloading
	mode.ammo_load = 1; //FIXME: This should have a getter, too, most likely in the library (because one of the plugins uses it)
	mode->SetReloadDelay(90);
	mode->SetDamage(0);
	
	// Projectile
	mode->SetProjectileSpeed(180);
	mode->SetProjectileRange(1000);
	
	// Spread
	mode->SetSpreadPerShot(ProjectileDeviationCmc(60));
	mode->SetSpreadBySelection(ProjectileDeviationCmc(10));
	mode->SetSpreadLimit(ProjectileDeviationCmc(200));
	
	// Identification
	mode.IsTracer = true;
	return mode;
}

/* --- Effects --- */

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	if (!firemode.IsTracer)
	{
		projectile->Trail(2, 150); // FIXME: Is not visible in hitscan :/
		projectile->HitScan();
	}
}

func FireEffect(object user, int angle, proplist firemode)
{
	// Muzzle flash
	var x = +Sin(angle, firemode->GetProjectileDistance());
	var y = -Cos(angle, firemode->GetProjectileDistance()) + firemode->GetYOffset();

	EffectMuzzleFlash(user, x, y, angle, 20, false, true);
	
	if (!firemode.IsTracer)
	{
		Casing(user, angle, firemode);
	}
}

func OnStartReload(object user, int x, int y, proplist firemode)
{
	if (firemode.IsTracer)
	{
		Casing(user, Angle(0, 0, x, y), firemode);
	}
}

func Casing(object user, int angle, proplist firemode)
{
	// Casing
	var x = +Sin(angle, firemode->GetProjectileDistance() / 2);
	var y = -Cos(angle, firemode->GetProjectileDistance() / 2) +  + firemode->GetYOffset();

	CreateCartridgeEffect("Cartridge_Pistol", 2, x, y, user->GetXDir() + Sin(-angle, 5), user->GetYDir() - RandomX(15, 20));
}
