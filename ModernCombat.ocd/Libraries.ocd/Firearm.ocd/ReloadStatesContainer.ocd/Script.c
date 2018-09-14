
/* --- Reload animations --- */

// 	Gets the default reload state that the weapon starts reloading from.
func GetReloadStartState(proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = this->GetAmmo(ammo_type);
	if (ammo >= firemode->GetAmmoAmount())
	{
		if (this->~AmmoChamberCapacity(ammo_type)
		&& !this->~AmmoChamberIsLoaded(ammo_type))
		{
			Log("Reload: Start from manual, because no bullet chambered");
			return Reload_Container_LoadAmmoChamber;
		}
		else
		{
			Log("Reload: Do nothing, because ammo amount %d > %d", ammo, firemode->GetAmmoAmount());
			return nil;
		}
	}
	else
	{
		return Reload_Container_Prepare;
	}
}

// Get ready to reload
local Reload_Container_Prepare = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Prepare] - Start");
		firearm->~PlaySoundOpenAmmoContainer();
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Prepare] - Finish");
		firearm->SetReloadState(firearm.Reload_Container_EjectAmmo);
	},

	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		// Repeat the same action
		Log("Reload [Prepare] - Cancel");
	},
};

// Remove an ammo container from the weapon
local Reload_Container_EjectAmmo = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Ammo eject] - Start");
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Ammo eject] - Finish");
		var ammo_type = firemode->GetAmmoID();
		firearm->~Reload_Container_EjectCasings(user, firemode);
		firearm->~PlaySoundEjectAmmo();
		
		if (firearm->GetAmmo(ammo_type) > firearm->~AmmoChamberCapacity(ammo_type))
		{
			firearm->SetReloadState(firearm.Reload_Container_StashStart);
		}
		else
		{
			firearm->SetReloadState(firearm.Reload_Container_InsertAmmo);
		}
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Ammo eject] - Cancel");
		
		// Stay in the same state, be fair and keep magazine ;)
	},
};


// Insert an ammo container into the weapon
local Reload_Container_InsertAmmo = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Ammo insert] - Start");
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->~PlaySoundInsertAmmo();
		firearm->ReloadRefillAmmo(firemode);
		firearm->SetReloadState(firearm.Reload_Container_Close);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Ammo insert] - Cancel");
		
		// Stay in the same state, be fair and keep magazine ;)
	},
};

local Reload_Container_Close = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Close] - Start");
		firearm->~PlaySoundCloseAmmoContainer();
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Close] - Finish");
		var ammo_type = firemode->GetAmmoID();
		if (this->~AmmoChamberCapacity(ammo_type)
		&& !this->~AmmoChamberIsLoaded(ammo_type))
		{
			firearm->SetReloadState(firearm.Reload_Container_LoadAmmoChamber);
		}
		else
		{
			firearm->SetReloadState(firearm.Reload_Container_ReadyWeapon);
		}
	},

	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		// Repeat the same action
		Log("Reload [Close] - Cancel");
	},
};


// Bring the weapon to ready stance
local Reload_Container_ReadyWeapon = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->~PlaySoundCloseAmmoContainer();
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
	},
};

/* --- Support adding spare ammo back to the user --- */



// Take out a partially filled magazine and stash it
local Reload_Container_StashStart = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, stash it] - Start");
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
		firearm->SetReloadState(firearm.Reload_Container_StashFinish);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, stash it] - Cancel");

		// Put a magazine in next
		firearm->SetReloadState(firearm.Reload_Container_InsertAmmo);
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
local Reload_Container_StashFinish = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Stashing] - Start");
		firearm->~PlaySoundResupplyAmmo();
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Stashing] - Finish");

		// Put a magazine in next
		firearm->SetReloadState(firearm.Reload_Container_InsertAmmo);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Stashing] - Cancel");

		// Put a magazine in next
		firearm->SetReloadState(firearm.Reload_MagIn);
	},
};

/* --- Support for an extra ammo chamber --- */

// Manually load a new shell to the chamber (open and close in one)
local Reload_Container_LoadAmmoChamber = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Start");
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Finish");
		firearm->~PlaySoundLoadAmmoChamber();
		firearm->~AmmoChamberInsert(firemode->GetAmmoID());
		firearm->~SetReloadState(firearm.Reload_Container_ReadyWeapon);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Cancel");
	},
};
