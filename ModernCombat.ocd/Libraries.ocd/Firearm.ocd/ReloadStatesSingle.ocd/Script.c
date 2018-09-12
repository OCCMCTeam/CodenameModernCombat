
/* --- Reload animations --- */

// 	Gets the default reload state that the weapon starts reloading from.
func GetReloadStartState(proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = this->GetAmmo(ammo_type);
	if (ammo >= firemode.ammo_load)
	{
		if (this->~AmmoChamberCapacity(ammo_type)
		&& !this->~AmmoChamberIsLoaded(ammo_type))
		{
			Log("Reload: Start from manual, because no bullet chambered");
			return Reload_Single_LoadAmmoChamber;
		}
		else
		{
			return nil;
		}
	}
	else
	{
		return Reload_Single_Prepare;
	}
}

// Get ready to reload
local Reload_Single_Prepare = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Prepare] - Start");
		firearm->~PlaySoundOpenAmmoContainer();
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Prepare] - Finish");
		firearm->~Reload_Single_EjectCasings(user, firemode);
		
		Log("Reload [Prepare] - Finish");
		if (firearm->~AmmoChamberCapacity(firemode->GetAmmoID())
		&& !firearm->~AmmoChamberIsLoaded(firemode->GetAmmoID()))
		{
			firearm->SetReloadState(firearm.Reload_Single_OpenAmmoChamber);
		}
		else
		{
			firearm->SetReloadState(firearm.Reload_Single_InsertShell);
		}
	},

	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		// Repeat the same action
		Log("Reload [Prepare] - Cancel");
	},
};

// Insert a single shell into the tube
local Reload_Single_InsertShell = new Firearm_ReloadState
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
			
			source->DoAmmo(info.ammo_type, ammo_spare); // give back the unnecessary ammo
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
		
		firearm.Reload_Single_InsertShell.is_done = is_done;
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Finish");
		// Finish condition?		
		if (firearm.Reload_Single_InsertShell.do_chamber_bullet)
		{
			firearm.Reload_Single_InsertShell.do_chamber_bullet = false;
			firearm->SetReloadState(firearm.Reload_Single_CloseAmmoChamber);
		}
		else if (firearm.Reload_Single_InsertShell.is_done)
		{
			if (firearm->~AmmoChamberCapacity(firemode->GetAmmoID())
			&& !firearm->~AmmoChamberIsLoaded(firemode->GetAmmoID()))
			{
				firearm->SetReloadState(firearm.Reload_Single_LoadAmmoChamber);
			}
			else
			{
				firearm->SetReloadState(firearm.Reload_Single_ReadyWeapon);
			}
		}
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Cancel");
		
		// Stay in the same state, be fair and keep magazine ;)
	},
};

// Bring the weapon to ready stance
local Reload_Single_ReadyWeapon = new Firearm_ReloadState
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

/* --- Support for an extra ammo chamber --- */

// Manually load a new shell to the chamber (open and close in one)
local Reload_Single_LoadAmmoChamber = new Firearm_ReloadState
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
		firearm->~SetReloadState(firearm.Reload_Single_ReadyWeapon);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Cancel");
	},
};


// Open the chamber, for manually inserting a shell 
local Reload_Single_OpenAmmoChamber = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->~PlaySoundOpenAmmoChamber();
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		this->OpenChamber(firearm, user, x, y, firemode);
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		this->OpenChamber(firearm, user, x, y, firemode);
	},

	OpenChamber = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm.Reload_Single_InsertShell.do_chamber_bullet = true;
		firearm->SetReloadState(firearm.Reload_Single_InsertShell);
	},
};

// Close the chamber, after inserting a single shell
local Reload_Single_CloseAmmoChamber = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->~PlaySoundCloseAmmoChamber();
		firearm->~AmmoChamberInsert(firemode->GetAmmoID());
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		this->CloseChamber(firearm, user, x, y, firemode);
	},

	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		this->CloseChamber(firearm, user, x, y, firemode);
	},
	
	CloseChamber = func (object firearm, object user, int x, int y, proplist firemode)
	{
		var source = firearm->GetAmmoReloadContainer();
		var ammo_requested = 0;
		if (source)
		{
			var info = firearm->ReloadGetAmmoInfo(firemode);
			ammo_requested = BoundBy(info.ammo_max + info.ammo_chambered - info.ammo_available, 0, firemode.ammo_usage ?? 1);
		}
		
		if (ammo_requested > 0)
		{
			firearm->SetReloadState(firearm.Reload_Single_InsertShell);
		}
		else
		{
			firearm->SetReloadState(firearm.Reload_Single_ReadyWeapon);
		}
	},
};
