#include CMC_Firearm_Basic
#include Plugin_Firearm_AmmoChamber
#include Plugin_Firearm_ReloadStates

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
	this.Reload_Prepare          = new Reload_Prepare          { Delay = 10, };
	this.Reload_InsertShellLong1 = new Reload_InsertShellLong1 { Delay = 20, };
	this.Reload_InsertShellLong2 = new Reload_InsertShellLong2 { Delay = 20, };
	this.Reload_InsertShell      = new Reload_InsertShell      { Delay = 15, };
	this.Reload_ManualLoad       = new Reload_ManualLoad       { Delay = 5, };
	this.Reload_ReadyWeapon      = new Reload_ReadyWeapon      { Delay = 25, };
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

	mode.ammo_load = 20;

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
	ScheduleCall(this, this.PlaySoundChamberBullet, firemode->GetRecoveryDelay(), 1);
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


func PlaySoundChamberBullet()
{
	Sound("Items::Weapons::Shotgun::Reload::BoltAction", {multiple = true});
}


func PlaySoundChamberOpen()
{
	Sound("Items::Weapons::Shotgun::Reload::BoltOpen", {multiple = true});
}


func PlaySoundChamberClose()
{
	Sound("Items::Weapons::Shotgun::Reload::BoltClose", {multiple = true});
}


/* --- Reload animations --- */

// 	Gets the default reload state that the weapon starts reloading from.
func GetReloadStartState(proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = this->GetAmmo(ammo_type);
	if (ammo >= firemode.ammo_load && !this->AmmoChamberIsLoaded(ammo_type))
	{
		Log("Reload: Start from manual, because no bullet chambered");
		return Reload_ManualLoad;
	}
	else
	{
		return Reload_Prepare;
	}
}

// Get ready to reload
local Reload_Prepare = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Prepare] - Start");
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Prepare] - Finish");
		if (firearm->AmmoChamberIsLoaded(firemode->GetAmmoID()))
		{
			firearm->SetReloadState(firearm.Reload_InsertShell);
		}
		else
		{
			firearm->SetReloadState(firearm.Reload_InsertShellLong1);
		}
	},

	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		// Repeat the same action
		Log("Reload [Prepare] - Cancel");
	},
};

// Insert a single shell into the chamber
local Reload_InsertShellLong1 = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->PlaySoundChamberOpen();
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm.Reload_InsertShell.do_chamber_bullet = true;
		firearm->SetReloadState(firearm.Reload_InsertShell);
	},
};

// Close chamber, after inserting a single shell
local Reload_InsertShellLong2 = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->PlaySoundChamberClose();
		firearm->AmmoChamberInsert(firemode->GetAmmoID());
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(firearm.Reload_InsertShell);
	},
};

// Insert a single shell into the tube
local Reload_InsertShell = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		// Do everything at the beginning here and count the next rest of the process
		// as a delay for getting the next shell ready - if that fails, start from
		// the beginning
		Log("Reload [Mag insert] - Start");
		
		var is_done = false;
		var source = firearm->GetAmmoReloadContainer();
		if (source)
		{
			var info = firearm->ReloadGetAmmoInfo(firemode);
			var ammo_requested = BoundBy(info.ammo_max + info.ammo_chambered - info.ammo_available, 0, firemode.ammo_usage ?? 1);
			var ammo_received = Abs(source->DoAmmo(firemode->GetAmmoID(), -ammo_requested)); // see how much you can get
			var ammo_spare = (info.ammo_available + ammo_received) % (firemode.ammo_usage ?? 1); // get ammo only in increments of ammo_usage
			
			source->DoAmmo(info.ammo_type, ammo_spare); // give back the unecessary ammo
			if (ammo_received > 0)
			{
				firearm->PlaySoundInsertShell();
				firearm->DoAmmo(info.ammo_type, ammo_received);
			}
			else
			{
				is_done = true;
			}
			
			// Finish condition?
			var is_full = firearm->GetAmmo(info.ammo_type) >= (info.ammo_max + info.ammo_chambered);
			var no_ammo = source->GetAmmo(info.ammo_type) == 0;
			if (is_full || no_ammo)
			{
				is_done = true;
			}			
		}
		else
		{
			is_done = true;
		}

		// Finish condition?
		if (is_done)
		{
			if (firearm->~AmmoChamberIsLoaded(info.ammo_type))
			{
				firearm->SetReloadState(firearm.Reload_ReadyWeapon);
			}
			else
			{
				firearm->SetReloadState(firearm.Reload_ManualLoad);
			}
		}
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Finish");
		if (firearm.Reload_InsertShell.do_chamber_bullet)
		{
			firearm.Reload_InsertShell.do_chamber_bullet = false;
			firearm->SetReloadState(firearm.Reload_InsertShellLong2);
		}
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Cancel");
		
		// Stay in the same state, be fair and keep magazine ;)
	},
};

// Manually load a new shell to the chamber
local Reload_ManualLoad = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Start");
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Finish");
		firearm->PlaySoundChamberBullet();
		firearm->AmmoChamberInsert(firemode->GetAmmoID());
		firearm->SetReloadState(firearm.Reload_ReadyWeapon);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Cancel");
	},
};

// Bring the weapon to ready stance
local Reload_ReadyWeapon = new Firearm_ReloadState
{
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
	},
};
