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

func SelectionTime() { return 36; }

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeBullets_TechniqueBurst());
	AddFiremode(FiremodeBullets_TechniqueSingle());
	AddFiremode(FiremodeGrenades_Explosive());
	AddFiremode(FiremodeGrenades_Cluster());
	AddFiremode(FiremodeGrenades_Smoke());

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
	->SetAmmoAmount(30)
	->SetCooldownDelay(13)
	->SetRecoveryDelay(3)
	->SetReloadDelay(90)
	->SetDamage(14)
	// Projectile
	->SetProjectileID(CMC_Projectile_Bullet)
	->SetProjectileSpeed(270)
	->SetProjectileRange(800)
	// Spread
	->SetSpreadPerShot(ProjectileDeviationCmc(55))
	->SetSpreadBySelection(ProjectileDeviationCmc(100))
	->SetSpreadLimit(ProjectileDeviationCmc(450))
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::AssaultRifle::Fire?");

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

func FiremodeBullets_TechniqueBurst()
{
	var mode = FiremodeBullets();

	// Generic info
	mode->SetName("$Burst$")
	->SetMode(WEAPON_FM_Burst)
	->SetBurstAmount(3);

	return mode;
}

func FiremodeGrenades()
{
	var mode = new Library_Firearm_Firemode {};

	mode->SetCMCDefaults()
	// Reloading
	->SetAmmoID(CMC_Ammo_Grenades)
	->SetAmmoAmount(1)
	->SetRecoveryDelay(1)
	->SetCooldownDelay(80)
	->SetReloadDelay(80)
	->SetDamage(20)
	// Projectile
	->SetProjectileSpeed(90)
	// Spread
	->SetSpreadPerShot(ProjectileDeviationCmc(200))
	->SetSpreadBySelection(ProjectileDeviationCmc(100))
	->SetSpreadLimit(ProjectileDeviationCmc(400))
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::AssaultRifle::FireLauncher?");

	return mode;
}

func FiremodeGrenades_Explosive()
{
	var mode = FiremodeGrenades();

	// Generic info
	mode->SetName("$Explosive$")
	// Grenade
	->SetProjectileID(CMC_Projectile_ExplosiveShell);

	return mode;
}

func FiremodeGrenades_Cluster()
{
	var mode = FiremodeGrenades();

	// Generic info
	mode->SetName("$Cluster$")
	// Grenade
	->SetProjectileID(CMC_Projectile_FragmentationShell);

	return mode;
}

func FiremodeGrenades_Smoke()
{
	var mode = FiremodeGrenades();

	// Generic info
	mode->SetName("$Smoke$")
	// Grenade
	->SetProjectileID(CMC_Projectile_SmokeShell)
	->SetDamage(mode->GetDamage() + 10);

	return mode;
}

/* --- Reload animations --- */

local ReloadStateMap = 
{
	// Bullets

	/* --- Default sequence --- */
	Magazine_Prepare     = { Delay = 5,  RaiseSpread = true, },
	Magazine_Drop        = { Delay = 20, RaiseSpread = true, StartCall = "PlaySoundEjectMagazine", },
	Magazine_Insert      = { Delay = 30, RaiseSpread = true, StartCall = "PlaySoundInsertMagazine", },
	Magazine_ReadyWeapon = { Delay = 20, },

	/* --- Support adding spare ammo back to the user --- */
	Magazine_StashStart  = { Delay = 20, RaiseSpread = true, StartCall = "PlaySoundEjectMagazine", },
	Magazine_StashFinish = { Delay = 20, RaiseSpread = true, StartCall = "PlaySoundResupplyAmmo", },

	/* --- Support for an extra ammo chamber --- */
	Magazine_LoadAmmoChamber = { Delay = 15, EndCall = "PlaySoundChamberBullet", },

	// Rifle grenades

	/* --- Default sequence --- */
	Container_Prepare     = { Delay = 10, StartCall = "PlaySoundOpenLauncher", },
	Container_EjectAmmo   = { Delay = 35, StartCall = "PlaySoundEjectGrenade", },
	Container_InsertAmmo  = { Delay = 20, StartCall = "PlaySoundInsertGrenade", },
	Container_Close       = { Delay = 10, StartCall = "PlaySoundCloseLauncher", },
	Container_ReadyWeapon = { Delay =  5, },

	/* --- Support adding spare ammo back to the user --- */
	Container_StashStart  = { Delay = 20, },
	Container_StashFinish = { Delay = 20, StartCall = "PlaySoundResupplyAmmo", },
};


func GetReloadStartState(proplist firemode)
{
	if (firemode->GetAmmoID() == CMC_Ammo_Bullets)
	{
		return GetReloadStartStateMagazine(firemode);
	}
	else
	{
		return GetReloadStartStateContainer(firemode);
	}
}


public func AmmoChamberCapacity(id ammo)
{
	if (ammo == CMC_Ammo_Bullets)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/* --- Effects --- */

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	if (firemode->GetAmmoID() == CMC_Ammo_Bullets)
	{
		projectile->HitScan();
	}
	else
	{
		var controller = user->GetController();
		if (GetType(controller) == C4PT_User)
		{
			SetPlrView(controller, projectile);
		}
	}
}

func FireEffect(object user, int angle, proplist firemode)
{
	if (firemode->GetAmmoID() == CMC_Ammo_Bullets)
	{
		MuzzleFlash(user, angle, 20);
		EjectCasing(user, angle)->TypeRifle()->DoSpin();
	}
	else
	{
		var muzzle = GetWeaponPosition(user, WEAPON_POS_Muzzle, angle);
		var user_xdir = user->GetXDir();
		var user_ydir = user->GetYDir();
		var xdir = +Sin(angle, 10);
		var ydir = -Cos(angle, 10);

		CreateParticle("Smoke2", PV_Random(muzzle.X - 5, muzzle.X + 5), PV_Random(muzzle.Y - 5, muzzle.Y + 5), PV_Random(user_xdir, user_xdir + xdir), PV_Random(user_ydir, user_ydir + ydir), PV_Random(15, 25),
		{
			Prototype = Particles_ThrustColored(200, 200, 200),
			Size = PV_Random(5, 10),
		}, 10);

		CreateParticle("Thrust", muzzle.X, muzzle.Y, user_xdir, user_ydir, PV_Random(20, 30),
		{
			Prototype = Particles_ThrustColored(255, 200, 200),
			Size = 8,
		});
	}
}

/* --- Sounds --- */

func PlaySoundEjectMagazine()
{
	Sound("Items::Weapons::AssaultRifle::Reload::EjectMag");
}

func PlaySoundInsertMagazine()
{
	Sound("Items::Weapons::AssaultRifle::Reload::InsertMag");
}

func PlaySoundChamberBullet()
{
	Sound("Items::Weapons::AssaultRifle::Reload::Bolt");
}

func PlaySoundOpenLauncher()
{
	Sound("Items::Weapons::AssaultRifle::Reload::OpenLauncher");
}

func PlaySoundCloseLauncher()
{
	Sound("Items::Weapons::AssaultRifle::Reload::CloseLauncher");
}

func PlaySoundEjectGrenade()
{
	Sound("Items::Weapons::AssaultRifle::Reload::EjectGrenade");
}

func PlaySoundInsertGrenade()
{
	Sound("Items::Weapons::AssaultRifle::Reload::InsertGrenade");
}

