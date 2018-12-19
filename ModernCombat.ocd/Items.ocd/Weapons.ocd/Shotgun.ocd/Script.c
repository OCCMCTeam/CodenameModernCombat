#include CMC_Firearm_Basic
#include Plugin_Firearm_AmmoChamber
#include Plugin_Firearm_ReloadStates
#include Plugin_Firearm_ReloadStates_Single

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

	StartLoaded();

	DefineWeaponOffset(WEAPON_POS_Magazine, +2, 2);
	DefineWeaponOffset(WEAPON_POS_Chamber,  +2, -1);
	DefineWeaponOffset(WEAPON_POS_Muzzle,  +12, -1);
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
	->SetAmmoAmount(20)
	->SetRecoveryDelay(5)
	->SetCooldownDelay(25)
	->SetReloadDelay(75)
	->SetAmmoUsage(4)
	->SetDamage(10)
	// Projectile
	->SetProjectileAmount(5) // 5 per shot
	->SetProjectileID(CMC_Projectile_Bullet)
	->SetProjectileSpeed([250, 270])
	->SetProjectileRange(300)
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


/* --- Reload animations --- */

local ReloadStateMap = 
{
	/* --- Default sequence --- */
	Single_Prepare     = { Delay = 10, UserAnimation = {Name = "MusketLoadArms", Begin =    0, End = 1000, }, RaiseSpread = true, },
	Single_InsertAmmo  = { Delay = 15, UserAnimation = {Name = "MusketLoadArms", Begin =  500, End = 4000, }, RaiseSpread = true, },
	Single_ReadyWeapon = { Delay = 25, UserAnimation = {Name = "MusketLoadArms", Begin = 3501, End = 4500, }, },
	/* --- Support for an extra ammo chamber --- */
	Single_LoadAmmoChamber  = { Delay =  5, UserAnimation = {Name = "MusketLoadArms", Begin = 3500, End = 3501, }, EndCall = "PlaySoundLoadAmmoChamber", RaiseSpread = true, },
	Single_OpenAmmoChamber  = { Delay = 20, UserAnimation = {Name = "MusketLoadArms", Begin = 1000, End = 1001, }, StartCall = "PlaySoundOpenAmmoChamber", RaiseSpread = true, },
	Single_CloseAmmoChamber = { Delay = 20, UserAnimation = {Name = "MusketLoadArms", Begin = 3500, End = 3600, }, StartCall = "PlaySoundCloseAmmoChamber", },
};


public func AmmoChamberCapacity(id ammo)
{
	if (ammo == CMC_Ammo_Bullets)
	{
		return GetFiremode()->GetAmmoUsage();
	}
	else
	{
		return 0;
	}
}

/* --- Effects --- */

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	projectile->HitScan();
}

func FireEffect(object user, int angle, proplist firemode)
{
	MuzzleFlash(user, angle, RandomX(40, 55));

	// Casing
	ScheduleCall(this, this.PlaySoundLoadAmmoChamber, firemode->GetRecoveryDelay(), 1);
	ScheduleCall(this, this.EjectCasing2, firemode->GetRecoveryDelay() + 5, 1, user, angle);
}

func EjectCasing2(object user, int angle)
{
	var color = user->GetColor();
	EjectCasing(user, angle, RandomX(-8, -4))->TypeShotgun()->SetColor(color);
}


/* --- Sounds --- */

func PlaySoundInsertShell()
{
	Sound("Items::Weapons::Shotgun::Reload::InsertShell?", {multiple = true});
}


func PlaySoundLoadAmmoChamber()
{
	Sound("Items::Weapons::Shotgun::Reload::BoltAction", {multiple = true});
}


func PlaySoundOpenAmmoChamber()
{
	Sound("Items::Weapons::Shotgun::Reload::BoltOpen", {multiple = true});
}


func PlaySoundCloseAmmoChamber()
{
	Sound("Items::Weapons::Shotgun::Reload::BoltClose", {multiple = true});
}
