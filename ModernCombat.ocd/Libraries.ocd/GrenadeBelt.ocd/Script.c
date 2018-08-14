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
		if (GetCurrentGrenadeType() == nil)
		{
			SetCurrentGrenadeType(type);
		}
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
		return cmc_grenade_belt.grenades[Format("%v", type)] ?? 0;
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
		var ready = this->~HasActionProcedure(false);

		// Close or open menu
		if (status == CONS_Up)
		{
			this->~CloseListSelectionMenu(ready);
		}
		else if (ready)
		{
			this->~OpenListSelectionMenu(this, CMC_LIST_MENU_TYPE_GRENADE_SELECTION);
		}
		
		// While not respawning block the option menu always!
		return true;
	}
	return _inherited(player, control, x, y, strength, repeat, status, ...);
}


// Opens the menu
public func GetListSelectionMenuEntries(object user, string type, proplist main_menu)
{
	if (type == CMC_LIST_MENU_TYPE_GRENADE_SELECTION)
	{
		main_menu->SetHeaderCaption("$ConfigureGrenades$");
		
		// Always show grenades in the same order, so that the grenade selection is independent of classes
		// - that is: I want the player to be able to pick the same grenade type with a certain hotkey,
		//            no matter what class is selected
		
		// Fill with contents
		var available_types = GetAvailableGrenadeTypes();
		if (GetLength(available_types) == 0)
		{
			FatalError("No grenades available...");
		}
		else
		{
			var list = main_menu->GetList();
			var hotkey = 0;
			var current_type = GetCurrentGrenadeType();
			
			// Add entry for drawing the current grenade type
			var entry = list->MakeEntryProplist();
			var default_action = "$DrawGrenade$";
			if (current_type)
			{
				default_action = Format("%s (<c %x>%s</c>)", default_action, GUI_CMC_Text_Color_Highlight, current_type->GetName());
			}
			entry->SetCaption(default_action)
			     ->SetCallbackOnMouseIn(list->DefineCallback(list.SelectEntry, default_action))           // Select the entry by hovering; the other possibilities are scrolling and hotkey
			     ->SetCallbackOnClick(this->DefineCallback(Library_ListSelectionMenu.CloseListSelectionMenu, true))       // Clicking the entry closes the menu; It is automatically selected, because you hover the entry to click it; 'false' means that the selection is not cancelled
			     ->SetCallbackOnMenuClosed(this->DefineCallback(this.TakeGrenade, current_type)) // Closing the menu selects the entry
			     ->SetScrollHint(true);
			list->AddEntry(default_action, entry);
			this->~SetListSelectionMenuHotkey(entry, hotkey++);


			// Add entries for the grenade types
			for (var grenade_type in available_types) 
			{
				var amount = GetGrenadeCount(grenade_type);
				var disabled = (amount == 0);
			
				// Text and description
				var text_color = nil;
				
				if (current_type == grenade_type)
				{
					text_color = GUI_CMC_Text_Color_Highlight;
				}
				else if (disabled)
				{
					text_color = GUI_CMC_Text_Color_Inactive;
				}
				var name = Format("%s", grenade_type->GetName(), amount);
				if (text_color)
				{
					name = Format("<c %x>%s</c>", text_color, name);
				}

				var entry = list->MakeEntryProplist();
				entry->SetIcon(grenade_type)
				     ->SetCaption(name)
				     ->SetCount(amount)
				     ->SetScrollHint(true);

				if (disabled)
				{
					 entry.SetSelected = CMC_GUI_SelectionListSeparator.SetSelected; // Overwrite selection thing, so that the entry cannot be selected
				}
				else
				{
				     entry->SetCallbackOnMouseIn(list->DefineCallback(list.SelectEntry, name))           // Select the entry by hovering; the other possibilities are scrolling and hotkey
				          ->SetCallbackOnClick(this->DefineCallback(Library_ListSelectionMenu.CloseListSelectionMenu, true))       // Clicking the entry closes the menu; It is automatically selected, because you hover the entry to click it; 'false' means that the selection is not cancelled
				          ->SetCallbackOnMenuClosed(this->DefineCallback(this.SetCurrentGrenadeType, grenade_type)); // Closing the menu selects the entry
				}
				list->AddEntry(name, entry);
				this->~SetListSelectionMenuHotkey(entry, hotkey++);
			}
		}
	}
	else
	{
		_inherited(user, type, main_menu, ...);
	}
}

private func DelayListSelectionMenu(object user, string type)
{
	if (type == CMC_LIST_MENU_TYPE_GRENADE_SELECTION)
	{
		return CMC_LIST_MENU_DELAY_SHORT;
	}
	return _inherited(user, type);
}

// Find all grenade types
func GetAvailableGrenadeTypes()
{
	var grenade_types = [];
	var grenade_type;

	for (var i = 0; grenade_type = GetDefinition(i, C4D_Object); ++i)
	{
		if (grenade_type->~IsGrenade() && grenade_type.Collectible)
		{
			PushFront(grenade_types, grenade_type);
		}
	}
	return grenade_types;
}
