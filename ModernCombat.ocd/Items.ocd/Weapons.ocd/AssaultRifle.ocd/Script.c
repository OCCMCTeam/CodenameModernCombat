#include CMC_Firearm_Basic
#include Plugin_Firearm_ReloadStates // For various animations
#include Plugin_Firearm_AmmoChamber


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
	
	// Reloading, the state saves some custom infos
	this.Reload_MagOut_StashStart = new Reload_MagOut_StashStart{};
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
		return Trans_Mul(this.MeshTransformation, Trans_Rotate(-90, 1, 0, 0), Trans_Rotate(-10, 0, 0, 1));
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
	->SetCooldownDelay(13)
	->SetRecoveryDelay(3)
	->SetReloadDelay(90)
	->SetDamage(14)
	// Projectile
	->SetProjectileID(CMC_Projectile_Bullet)
	->SetProjectileSpeed(270)
	->SetProjectileRange(800)
	->SetProjectileDistance(12)
	->SetYOffset(-6)
	// Spread
	->SetSpreadPerShot(ProjectileDeviationCmc(55))
	->SetSpreadBySelection(ProjectileDeviationCmc(100))
	->SetSpreadLimit(ProjectileDeviationCmc(450))
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::AssaultRifle::Fire?");

	mode.ammo_load = 30;

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
	->SetRecoveryDelay(1)
	->SetCooldownDelay(80)
	->SetReloadDelay(80)
	->SetDamage(20)
	// Projectile
	->SetProjectileSpeed(90)
	->SetProjectileDistance(12)
	->SetYOffset(-6)
	// Spread
	->SetSpreadPerShot(ProjectileDeviationCmc(200))
	->SetSpreadBySelection(ProjectileDeviationCmc(100))
	->SetSpreadLimit(ProjectileDeviationCmc(400))
	// Crosshair, CMC Custom
	->SetAimCursor([CMC_Cursor_Cone])
	// Effects, CMC custom
	->SetFireSound("Items::Weapons::AssaultRifle::FireLauncher?");

	mode.ammo_load = 1;

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

/* --- Effects --- */

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	if (firemode->GetAmmoID() == CMC_Ammo_Bullets)
	{
		projectile->Trail(2, 150); // FIXME: Is not visible in hitscan :/
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
	// Muzzle flash
	var x = +Sin(angle, firemode->GetProjectileDistance());
	var y = -Cos(angle, firemode->GetProjectileDistance()) + firemode->GetYOffset();

	if (firemode->GetAmmoID() == CMC_Ammo_Bullets)
	{
		EffectMuzzleFlash(user, x, y, angle, 20, false, true);
		
		// Casing
		x = +Sin(angle, firemode->GetProjectileDistance() / 3);
		y = -Cos(angle, firemode->GetProjectileDistance() / 3) + firemode->GetYOffset();
		var xdir = user->GetCalcDir() * 14 * RandomX(-2, -1);
		var ydir = RandomX(-11, -13);

		CreateCartridgeEffect("Cartridge_Pistol", 2, x, y, user->GetXDir() + xdir, user->GetYDir() + ydir);
	}
	else
	{
		var user_xdir = user->GetXDir();
		var user_ydir = user->GetYDir();
		
		CreateParticle("Smoke2", PV_Random(x - 5, x + 5), PV_Random(y - 5, y + 5), PV_Random(user_xdir, user_xdir + xdir), PV_Random(user_ydir, user_ydir + ydir), PV_Random(15, 25),
		{
			Prototype = Particles_ThrustColored(200, 200, 200),
			Size = PV_Random(5, 10),
		}, 10);
	
		CreateParticle("Thrust", x, y, user_xdir, user_ydir, PV_Random(20, 30),
		{
			Prototype = Particles_ThrustColored(255, 200, 200),
			Size = 8,
		});
	}
}

/* --- Reload animations --- */

// 	Gets the default reload state that the weapon starts reloading from.
func GetReloadStartState(proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = GetAmmo(ammo_type);
	if (ammo >= firemode.ammo_load && !this->AmmoChamberIsLoaded(ammo_type))
	{
		Log("Reload: Start from manual, because no bullet chambered");
		return Reload_ManualLoad;
	}
	else
	{
		return Reload_GrabMag;
	}
}

// Grab the magazine that is currently in the weapon
local Reload_GrabMag = new Firearm_ReloadState
{
	Delay = 10,

	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Grab mag] - Start");
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Grab mag] - Finish");
		var ammo_type = firemode->GetAmmoID();
		var ammo = firearm->GetAmmo(ammo_type);
		if (ammo > firearm->AmmoChamberCapacity(ammo_type))
		{
			firearm->SetReloadState(firearm.Reload_MagOut_StashStart);
		}
		else
		{
			firearm->SetReloadState(firearm.Reload_MagOut_Drop);
		}
	},

	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		// Repeat the same action
		Log("Reload [Grab mag] - Cancel");
	},
};

// Take out a partially filled magazine and stash it
local Reload_MagOut_StashStart = new Firearm_ReloadState
{
	Delay = 20,

	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, stash it] - Start");
		firearm->Sound("Items::Weapons::AssaultRifle::Reload::EjectMag");
		// Take out ammo now, because the previous version where ammo state is changed only on finish looked strange ingame
		SetTemporaryAmmo(firemode->GetAmmoID(), firearm->ReloadRemoveAmmo(firemode, false));
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, stash it] - Finish");

		// Fill ammo belt of the user
		var ammo_type = firemode->GetAmmoID();
		firearm->GetAmmoReloadContainer()->DoAmmo(ammo_type, GetTemporaryAmmo(ammo_type));
		SetTemporaryAmmo(ammo_type, 0);

		// Finish with stashing sound, symbolizes that ammo is added
		firearm->SetReloadState(firearm.Reload_MagOut_StashFinish);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, stash it] - Cancel");

		// Put a magazine in next
		firearm->SetReloadState(firearm.Reload_MagIn);
	},
	
	SetTemporaryAmmo = func (id ammo_type, int amount)
	{
		if (!this.temp_ammo) this.temp_ammo = {};
		this.temp_ammo[Format("%i", ammo_type)] = amount;
	},
	
	GetTemporaryAmmo = func (id ammo_type)
	{
		return this.temp_ammo[Format("%i", ammo_type)];
	},
};

// Short delay and sound while stashing the magazine, merely cosmetic
local Reload_MagOut_StashFinish = new Firearm_ReloadState
{
	Delay = 20,

	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Stashing] - Start");
		user->Sound("Clonk::Action::Grab?");
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Stashing] - Finish");

		// Put a magazine in next
		firearm->SetReloadState(firearm.Reload_MagIn);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Stashing] - Cancel");

		// Put a magazine in next
		firearm->SetReloadState(firearm.Reload_MagIn);
	},
};

// Drop an empty mag to the ground
local Reload_MagOut_Drop = new Firearm_ReloadState
{
	Delay = 20,
	
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, drop it] - Start");
		firearm->Sound("Items::Weapons::AssaultRifle::Reload::EjectMag");
		
		// Lose current ammo
		firearm->ReloadRemoveAmmo(firemode, false);
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, drop it] - Finish");
		
		// Put a magazine in next
		firearm->SetReloadState(firearm.Reload_MagIn);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, drop it] - Cancel");
		
		// Put a magazine in next
		firearm->SetReloadState(firearm.Reload_MagIn);
	},
};

// Insert a new mag into the weapon
local Reload_MagIn = new Firearm_ReloadState
{
	Delay = 30,
	
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Start");
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Finish");
		firearm->Sound("Items::Weapons::AssaultRifle::Reload::InsertMag");
		firearm->ReloadRefillAmmo(firemode);
		
		// Load a bullet now?
		if (firearm->AmmoChamberIsLoaded(firemode->GetAmmoID()))
		{
			firearm->SetReloadState(firearm.Reload_ReadyWeapon);
		}
		else
		{
			firearm->SetReloadState(firearm.Reload_ManualLoad);
		}
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Cancel");
		
		// Stay in the same state, be fair and keep magazine ;)
	},
};

// Manually load a new bullet to the chamber
local Reload_ManualLoad = new Firearm_ReloadState
{
	Delay = 10,
	
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Start");
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Finish");
		firearm->Sound("Items::Weapons::AssaultRifle::Reload::Bolt");
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
	Delay = 20,
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
	},
};
