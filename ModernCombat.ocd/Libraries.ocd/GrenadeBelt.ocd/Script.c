/**
	Library for grenade belt.

	@author Marky
*/

/* --- Properties --- */

local cmc_grenade_belt;

/* --- Engine callbacks --- */

func Initialize()
{
	_inherited(...);
	
	cmc_grenade_belt = 
	{
		max_count = 4,
		active_type_index = nil,
		grenades = {},
	};
}

/* --- Interface --- */

/**
	Takes a grenade of a specific type.
	
	@par type This type of grenade will be drawn.
	
	@return bool {@code true} if drawing the grenade was successful.  
 */
public func TakeGrenade(id type)
{
	if (!type)
	{
		PlayerMessage(GetOwner(), "$MsgNoGrenadesInBelt$");
		return false;
	}
	var count = GetGrenadeCount(type);
	if (count > 0)
	{
		var grenade = CreateObject(type, 0, 0, NO_OWNER);
		Collect(grenade);

		if (grenade)
		{
			// Collected it? Reduce the amount
			if (grenade->Contained() == this)
			{
				DoGrenadeCount(type, -1);
				
				// Select the grenade, if possible
				if (this.GetItemPos && this.SetHandItemPos)
				{
					var inventory_pos = this->GetItemPos(grenade);
					this->SetHandItemPos(0, inventory_pos);
				}
				return true;
			}
			else
			{
				grenade->RemoveObject();
			}
		}
	}
	return false;
}


/**
	Tries to stash a grenade into the belt.
 */
public func StashGrenade(object grenade)
{
	AssertNotNil(grenade);
	if (grenade->~IsGrenade())
	{
		if (grenade->IsActive())
		{
			return false;
		}
		else if (DoGrenadeCount(grenade->GetID(), 1) == 1)
		{
			grenade->RemoveObject();
			return true;
		}
		return false;
	}
	else
	{
		FatalError("Trying to stash a non-grenade into the grenade belt");
	}
}


/**
	Changes the amount of grenades in the belt.
	
	@par type Can be a {@code C4V_String} or {@code C4V_Def}.
	@par change The value to change by.
	
	@return int The actual change.
 */
public func DoGrenadeCount(type, int change)
{
	if (GetType(type) == C4V_Def)
	{
		return DoGrenadeCount(Format("%v", type), change);
	}
	else if (GetType(type) == C4V_String)
	{
		var old_count = cmc_grenade_belt.grenades[type];
		var max_count = old_count;
		if (change > 0)
		{
			max_count = cmc_grenade_belt.max_count - GetGrenadeCount();
		}
		var new_count = BoundBy(old_count + change, 0, max_count);

		cmc_grenade_belt.grenades[type] = new_count;
		return new_count - old_count;
	}
	else
	{
		FatalError("Argument must be string or id.");
	}
}


/**
	Gets the amount of stashed grenades.
	
	@par type Get the count for a specific type,
	          or get the total of all grenades
	          if {@code nil} is passed.

	@return int The amount.
 */
public func GetGrenadeCount(id type)
{
	if (type)
	{
		return cmc_grenade_belt.grenades[Format("%v", type)];
	}
	else
	{
		var count = 0;
		for (var saved in GetProperties(cmc_grenade_belt.grenades))
		{
			count += cmc_grenade_belt.grenades[saved];
		}
		return count;
	}
}

/* --- Internals --- */

func GetCurrentGrenadeType()
{
	// No grenades left? Reset active type
	if (cmc_grenade_belt.active_type && GetGrenadeCount(cmc_grenade_belt.active_type) == 0)
	{
		cmc_grenade_belt.active_type = nil;
	}
	// No type set? Switch to type with max count
	if (cmc_grenade_belt.active_type == nil)
	{
		var active_count = 0;
		for (var saved in GetProperties(cmc_grenade_belt.grenades))
		{
			var current_type = GetDefinition(saved);
			var current_count = GetGrenadeCount(current_type);
			if (current_count > active_count)
			{
				cmc_grenade_belt.active_type = current_type;
				active_count = current_count;
			}
		}
	}
	return cmc_grenade_belt.active_type;
}


func SetCurrentGrenadeType(id type)
{
	if (type->~IsGrenade())
	{
		cmc_grenade_belt.active_type = type;
	}
}


/* --- Controls --- */

func ObjectControl(int player, int control, int x, int y, int strength, bool repeat, int status)
{
	if (!this)
		return false;

	// Somewhat redundant, spawn menu blocks the option menu anyway
	if (control == CON_CMC_DrawGrenade && !this->~IsRespawning())
	{
		// Has to be walking, jumping, not scaling
		if (this->~HasActionProcedure(false))
		{
			TakeGrenade(GetCurrentGrenadeType());
		}
		// While not respawning block the option menu always!
		return true;
	}

	return _inherited(player, control, x, y, strength, repeat, status, ...);
}
