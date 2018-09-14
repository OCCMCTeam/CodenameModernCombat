#include CMC_Firearm_Basic
#include Plugin_Firearm_ReloadStates
#include CMC_Firearm_ReloadStates_Single

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;

local casing_count = 0;

func SelectionTime() { return 10; }

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeBullets_TechniqueSingle());
	
	// Reloading times
	this.Reload_Single_Prepare     = new Reload_Single_Prepare     { Delay = 10, };
	this.Reload_Single_InsertShell = new Reload_Single_InsertShell { Delay = 15, };
	this.Reload_Single_ReadyWeapon = new Reload_Single_ReadyWeapon { Delay = 20, };
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
	->SetAmmoAmount(6)
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
	++casing_count;
}

func FireEffect(object user, int angle, proplist firemode)
{
	// Muzzle flash
	var x = +Sin(angle, firemode->GetProjectileDistance());
	var y = -Cos(angle, firemode->GetProjectileDistance()) + firemode->GetYOffset();

	EffectMuzzleFlash(user, x, y, angle, RandomX(40, 45), false, true);
}

func Casing(object user, proplist firemode)
{
	var angle = user->GetCalcDir() * 90;
	
	// Casing
	var x = +Sin(angle, firemode->GetProjectileDistance() / 2);
	var y = -Cos(angle, firemode->GetProjectileDistance() / 2) +  + firemode->GetYOffset();

	CreateCartridgeEffect("Cartridge_Pistol", 2, x, y, user->GetXDir() + RandomX(-2, 2), user->GetYDir() - Random(2));
}

func Reload_Single_EjectCasings(object user, proplist firemode)
{
	for (; casing_count > 0; --casing_count)
	{
		Casing(user, firemode);
	}
}

/* --- Sounds --- */

func PlaySoundInsertShell()
{
	Sound("Items::Weapons::Pistol::Reload::InsertDart", {multiple = true});
}

func PlaySoundOpenAmmoContainer()
{
	Sound("Items::Weapons::Pistol::Reload::OpenChamber", {multiple = true});
}

func PlaySoundCloseAmmoContainer()
{
	Sound("Items::Weapons::Pistol::Reload::CloseChamber", {multiple = true});
}
