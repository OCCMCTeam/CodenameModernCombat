#include CMC_Firearm_Basic
#include Plugin_Firearm_ReloadStates
#include Plugin_Firearm_ReloadStates_Container

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;

func SelectionTime() { return 36; }

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeBullets_TechniqueAuto());
	AddFiremode(FiremodeBullets_TechniqueBurst());

	StartLoaded();

	DefineWeaponOffset(WEAPON_POS_Magazine, +2, 2);
	DefineWeaponOffset(WEAPON_POS_Chamber,  +2, -1);
	DefineWeaponOffset(WEAPON_POS_Muzzle,  +12, -1);
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
		return Trans_Mul(this.MeshTransformation, Trans_Rotate(-90, 1, 0, 0), Trans_Rotate(-6, 0, 0, 1));
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
	// Reloading
	->SetAmmoID(CMC_Ammo_Bullets)
	->SetAmmoAmount(60)
	->SetCooldownDelay(4)
	->SetRecoveryDelay(4)
	->SetReloadDelay(220)
	->SetDamage(8)
	// Projectile
	->SetProjectileID(CMC_Projectile_Bullet)
	->SetProjectileSpeed(270)
	->SetProjectileRange(750)
	// Spread
	->SetSpread(ProjectileDeviationCmc(20))
	->SetSpreadPerShot(ProjectileDeviationCmc(30))
	->SetSpreadBySelection(ProjectileDeviationCmc(220))
	->SetSpreadLimit(ProjectileDeviationCmc(420))
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::MachineGun::Fire");

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
	// Burst
	->SetDamage(mode->GetDamage() + 1)
	->SetBurstAmount(4)
	->SetCooldownDelay(mode->GetCooldownDelay() + 20)
	->SetSpreadPerShot(mode->GetSpreadPerShot() + ProjectileDeviationCmc(10));

	return mode;
}

/* --- Reload animations --- */

local ReloadStateMap = 
{
	/* --- Default sequence --- */
	Container_Prepare     = { Delay = 20, StartCall  = "PlaySoundOpenAmmoContainer", },
	Container_EjectAmmo   = { Delay = 50, EndCall    = "PlaySoundEjectAmmo", },
	Container_InsertAmmo  = { Delay = 90, EndCall    = "PlaySoundInsertAmmo", },
	Container_Close       = { Delay = 30, StartCall  = "PlaySoundCloseAmmoContainer", },
	Container_ReadyWeapon = { Delay = 30, StartCall  = "PlaySoundCloseAmmoContainer", },

	/* --- Support adding spare ammo back to the user --- */
	Container_StashStart  = { Delay = 20, },
	Container_StashFinish = { Delay = 20, StartCall = "PlaySoundResupplyAmmo", },
};

/* --- Effects --- */

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	projectile->Trail(2, 150); // FIXME: Is not visible in hitscan :/
	projectile->HitScan();
}

func FireEffect(object user, int angle, proplist firemode)
{
	var muzzle = GetWeaponPosition(user, WEAPON_POS_Muzzle, angle);
	EffectMuzzleFlash(user, muzzle.X, muzzle.Y, angle, RandomX(35, 50), false, true);
	EjectCasing(user, angle, "Cartridge_Pistol", 2);
}

/* --- Sounds --- */

func PlaySoundOpenAmmoContainer()
{
	Sound("Items::Weapons::MachineGun::Reload::Bolt");
}

func PlaySoundCloseAmmoContainer()
{
	Sound("Items::Weapons::MachineGun::Reload::CloseLid");
}

func PlaySoundEjectAmmo()
{
	Sound("Items::Weapons::MachineGun::Reload::EjectMag");
}

func PlaySoundInsertAmmo()
{
	Sound("Items::Weapons::MachineGun::Reload::InsertMag");
}

