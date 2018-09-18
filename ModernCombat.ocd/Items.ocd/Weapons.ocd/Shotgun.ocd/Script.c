#include CMC_Firearm_Basic
#include Plugin_Firearm_AmmoChamber
#include Plugin_Firearm_ReloadStates
#include CMC_Firearm_ReloadStates_Single

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
	
	// Reloading times
	this.Reload_Single_Prepare          = new Reload_Single_Prepare          { Delay = 10, UserAnimation = {Name = "MusketLoadArms", Begin =    0, End = 1000, }, };
	this.Reload_Single_OpenAmmoChamber  = new Reload_Single_OpenAmmoChamber  { Delay = 20, UserAnimation = {Name = "MusketLoadArms", Begin = 1000, End = 1001, }, };
	this.Reload_Single_CloseAmmoChamber = new Reload_Single_CloseAmmoChamber { Delay = 20, UserAnimation = {Name = "MusketLoadArms", Begin = 3500, End = 3600, }, };
	this.Reload_Single_InsertShell      = new Reload_Single_InsertShell      { Delay = 15, UserAnimation = {Name = "MusketLoadArms", Begin =  500, End = 4000, }, };
	this.Reload_Single_LoadAmmoChamber  = new Reload_Single_LoadAmmoChamber  { Delay =  5, UserAnimation = {Name = "MusketLoadArms", Begin = 3500, End = 3501, }, };
	this.Reload_Single_ReadyWeapon      = new Reload_Single_ReadyWeapon      { Delay = 25, UserAnimation = {Name = "MusketLoadArms", Begin = 3501, End = 4500, }, };
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
	ScheduleCall(this, this.PlaySoundLoadAmmoChamber, firemode->GetRecoveryDelay(), 1);
	ScheduleCall(this, this.EjectCasing, firemode->GetRecoveryDelay() + 5, 1, user, angle, firemode);
}

func EjectCasing(object user, int angle, proplist firemode)
{
	var x = +Sin(angle, firemode->GetProjectileDistance() / 3);
	var y = -Cos(angle, firemode->GetProjectileDistance() / 3) + firemode->GetYOffset();
	var xdir = user->GetCalcDir() * 8 * RandomX(-2, -1);
	var ydir = RandomX(-11, -13);

	CreateCartridgeEffect("Cartridge_Pistol", 4, x, y, user->GetXDir() + xdir, user->GetYDir() + ydir);
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
