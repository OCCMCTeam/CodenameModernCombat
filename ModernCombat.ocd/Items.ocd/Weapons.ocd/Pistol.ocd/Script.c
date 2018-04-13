#include CMC_Firearm_Basic

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);
	this.MeshTransformation = Trans_Scale(800, 800, 800);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeStandard());
}

func Definition(id def)
{
	def.PictureTransformation = Trans_Mul(Trans_Rotate(-20, 0, 1, 0), Trans_Rotate(-20, 0, 0, 1), Trans_Rotate(5, 1, 0, 0), Trans_Translate(-1800, 0, -3000));
}

/* --- Display --- */

local carry_mode = CARRY_Hand;

public func GetCarryMode(object clonk, bool idle, bool nohand)
{
	if (idle || nohand)
	{
		return CARRY_None;
	}
	return carry_mode;
}
public func GetCarrySpecial(object user) { return "pos_hand2"; }
public func GetCarryBone() { return "Grip"; }
public func GetCarryTransform()
{
	return Trans_Mul(Trans_Rotate(90, 1, 0, 0), Trans_Translate(-2500, 800, 0), Trans_Scale(800, 800, 800));
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
	projectile->Trail(2, 150);
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
