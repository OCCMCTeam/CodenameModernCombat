
/* --- Reload animations --- */

// 	Gets the default reload state that the weapon starts reloading from.
func GetReloadStartState(proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = this->GetAmmo(ammo_type);
	if (ammo >= firemode.ammo_load)
	{
		Log("Reload: Start from manual, because no bullet chambered");
		return nil;
	}
	else
	{
		return Reload_Revolver_Prepare;
	}
}

// Get ready to reload
local Reload_Revolver_Prepare = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Prepare] - Start");
		firearm->~PlaySoundDrumOpen();
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Prepare] - Finish");
		firearm->SetReloadState(firearm.Reload_Revolver_InsertShell);
		firearm->~EjectCasings(user, firemode);
	},

	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		// Repeat the same action
		Log("Reload [Prepare] - Cancel");
	},
};

// Insert a single shell into the tube
local Reload_Revolver_InsertShell = new Firearm_ReloadState
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
			firearm->SetReloadState(firearm.Reload_Revolver_ReadyWeapon);
		}
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Finish");
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Cancel");
		
		// Stay in the same state, be fair and keep magazine ;)
	},
};


// Bring the weapon to ready stance
local Reload_Revolver_ReadyWeapon = new Firearm_ReloadState
{
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
		firearm->~PlaySoundDrumClose();
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
	},
};
