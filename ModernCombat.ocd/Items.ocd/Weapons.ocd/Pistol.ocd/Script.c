#include CMC_Firearm_Basic
#include Plugin_Firearm_AmmoChamber
#include Plugin_Firearm_ReloadStates
#include Plugin_Firearm_ReloadStates_Magazine
#include Plugin_Firearm_ReloadStates_Container

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

	StartLoaded();

	DefineWeaponOffset(WEAPON_POS_Magazine, +3, +1);
	DefineWeaponOffset(WEAPON_POS_Chamber, +4, -1);
	DefineWeaponOffset(WEAPON_POS_Muzzle, +8, -1);
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
	else
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
	// Aiming
	->SetIronsightAimingAnimation("AimPistol")
	// Reloading
	->SetAmmoID(CMC_Ammo_Bullets)
	->SetRecoveryDelay(5)
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
	mode->SetName("$Single$")
	->SetProjectileID(CMC_Projectile_Bullet)
	// Reloading
	->SetAmmoAmount(15)
	->SetReloadDelay(40)
	->SetDamage(11)
	// Projectile
	->SetProjectileSpeed(250)
	->SetProjectileRange(450)
	// Spread
	->SetSpread(ProjectileDeviationCmc(20))
	->SetSpreadPerShot(ProjectileDeviationCmc(70))
	->SetSpreadBySelection(ProjectileDeviationCmc(30))
	->SetSpreadLimit(ProjectileDeviationCmc(220));

	// Identification
	mode.IsTracer = false;

	return mode;
}

func FiremodeBullets_TechniqueTracerDart()
{
	var mode = FiremodeBullets();

	// Generic info
	mode->SetName("$TracerDart$")
	->SetProjectileID(CMC_Projectile_TracerDart)
	// Reloading
	->SetAmmoAmount(1)
	->SetReloadDelay(90)
	->SetDamage(0)
	// Projectile
	->SetProjectileSpeed(180)
	->SetProjectileRange(1000)
	// Spread
	->SetSpreadPerShot(ProjectileDeviationCmc(60))
	->SetSpreadBySelection(ProjectileDeviationCmc(10))
	->SetSpreadLimit(ProjectileDeviationCmc(200));

	// Identification
	mode.IsTracer = true;

	return mode;
}

/* --- Reload animations --- */

local ReloadStateMap = 
{
	// Bullets

	/* --- Default sequence --- */
	Magazine_Prepare     = { Delay = 1,  RaiseSpread = true, },
	Magazine_Drop        = { Delay = 10, RaiseSpread = true, StartCall = "PlaySoundEjectMagazine", },
	Magazine_Insert      = { Delay = 20, RaiseSpread = true, EndCall = "PlaySoundInsertMagazine", },
	Magazine_ReadyWeapon = { Delay = 4, },

	/* --- Support adding spare ammo back to the user --- */
	Magazine_StashStart  = { Delay = 10, RaiseSpread = true, StartCall = "PlaySoundEjectMagazine", },
	Magazine_StashFinish = { Delay = 10, RaiseSpread = true, StartCall = "PlaySoundResupplyAmmo", },

	/* --- Support for an extra ammo chamber --- */
	Magazine_LoadAmmoChamber = { Delay = 5, EndCall = "PlaySoundChamberBullet", },

	// Tracer

	/* --- Default sequence --- */
	Container_Prepare     = { Delay = 10, StartCall = "PlaySoundOpenChamber", },
	Container_EjectAmmo   = { Delay = 35, StartCall = "PlaySoundEjectDart", },
	Container_InsertAmmo  = { Delay = 20, StartCall = "PlaySoundInsertDart", },
	Container_Close       = { Delay = 10, StartCall = "PlaySoundCloseChamber", },
	Container_ReadyWeapon = { Delay =  5, },

	/* --- Support adding spare ammo back to the user --- */
	Container_StashStart  = { Delay = 20, },
	Container_StashFinish = { Delay = 20, StartCall = "PlaySoundResupplyAmmo", },
};


func GetReloadStartState(proplist firemode)
{
	if (firemode.IsTracer)
	{
		return GetReloadStartStateContainer(firemode);
	}
	else
	{
		return GetReloadStartStateMagazine(firemode);
	}
}


public func AmmoChamberCapacity(id ammo)
{
	if (GetFiremode().IsTracer) // FIXME: Should have access to the desired firemode via parameter
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/* --- Effects --- */

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	if (!firemode.IsTracer)
	{
		projectile->HitScan();
	}
}

func FireEffect(object user, int angle, proplist firemode)
{
	MuzzleFlash(user, angle, 20);
	if (!firemode.IsTracer)
	{
		EjectCasing(user, angle);
	}
}

func OnStartReload(object user, int x, int y, proplist firemode)
{
	if (firemode.IsTracer)
	{
		var angle = GetAngle(x, y);
		EjectCasing(user, angle, Sin(-angle, 5), -RandomX(15, 20));
	}
}


/* --- Sounds --- */

func PlaySoundEjectMagazine()
{
	Sound("Items::Weapons::Pistol::Reload::EjectMag");
}

func PlaySoundInsertMagazine()
{
	Sound("Items::Weapons::Pistol::Reload::InsertMag");
}

func PlaySoundChamberBullet()
{
	Sound("Items::Weapons::Pistol::Reload::PullSlide"); // Should just be "CloseChamber" actually? Pulling the slide is not really accurate, only on the first reload ever
}

func PlaySoundOpenChamber()
{
	Sound("Items::Weapons::Pistol::Reload::OpenChamber");
}

func PlaySoundCloseChamber()
{
	Sound("Items::Weapons::Pistol::Reload::CloseChamber");
}

func PlaySoundEjectDart()
{
	Sound("Items::Weapons::Pistol::Reload::EjectDart");
}

func PlaySoundInsertDart()
{
	Sound("Items::Weapons::Pistol::Reload::InsertDart");
}


