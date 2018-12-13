/**
	Ammo Bag
*/

#include Library_AmmoManager
#include Library_CMC_Pack
#include Library_ListSelectionMenu

/* --- Engine callbacks --- */

public func Initialize(...)
{
	SetGraphics(nil, DynamiteBox);
	this.PictureTransformation = Trans_Scale(); // Scale the picture of the box mesh, so that it does not appear in the game
	SetGraphics(nil, CMC_Tool_Ammo_Bag, 1, GFXOV_MODE_Picture);
	return _inherited(...);
}

func Hit()
{
	Sound("Items::Tools::AmmoBox::Hit?", {multiple = true});
}

/* --- Display --- */

public func GetCarryMode(object user, bool not_selected)
{
	var can_hold = user->~IsWalking() || user->~IsJumping();
	if (can_hold && !not_selected)
	{
		return CARRY_BothHands;
	}
	else
	{
		return CARRY_Back;
	}
}


public func GetCarryTransform(object user, bool not_selected, bool nohand, bool second_on_back)
{
	if (not_selected)
	{
		if (second_on_back)
		{
			return Trans_Mul(Trans_Translate(-5000, 3000, 0), Trans_Rotate(-45, 0, 1));
		}
		else
		{
			return Trans_Mul(Trans_Translate(0, 3000, 00), Trans_Rotate(-45, 0, 1));
		}
	}
	if (nohand)
	{
		return Trans_Mul(Trans_Translate(0, -3000, -2200), Trans_Rotate(-45, 0, 1));
	}
}


public func GetCarryPhase()
{
	return 450;
}

/* --- Functionality --- */

public func MaxAmmo() { return 200; }

public func GetAmmoRefillInterval() { return 20; }

public func AllowAmmoRefill(object user)
{
	return HasAmmoAbility(user);
}

/* --- User Interface --- */

local active_menu;

func ControlUse(object user, int x, int y)
{
	// User has to be ready to act
	if (user->~HasActionProcedure())
	{
		OpenListSelectionMenu(user);
	}
	return true;
}


func ControlUseItemConfig(object user, int x, int y, int status)
{
	var ready = user->~HasActionProcedure();
	if (status == CONS_Up)
	{
		CloseListSelectionMenu(); // Never close with callback
	}
	else if (ready)
	{
		OpenListSelectionMenu(user);
	}
	return true;
}


func DelayListSelectionMenu(object user, string type)
{
	return CMC_LIST_MENU_DELAY_SHORT;
}


public func OpenListSelectionMenu(object user, string type)
{
	// No ammo and cannot refill? Destroy
	if (RemoveEmptyAmmoBag(user))
	{
		return CloseListSelectionMenu();
	}
	else
	{
		user->SetComDir(COMD_Stop);
		return _inherited(user, type, ...);
	}
}

// Adds the menu entries to the menu 
public func GetListSelectionMenuEntries(object user, string type, proplist main_menu)
{
	main_menu->SetHeaderCaption("$TakeAmmo$");
	main_menu.Settings.ClickAfterHotkey = true;

	// Fill with contents
	var available_types = GetAvailableAmmoTypes();
	if (GetLength(available_types) == 0)
	{
		FatalError("TODO: Add an ingame error message here....");
	}
	else
	{
		var ammo_list = main_menu->GetList();
		var hotkey = 0;

		// Add cancel option as default

		var default_entry = ammo_list->MakeEntryProplist();
		var default_action = "$Cancel$";
		default_entry->SetIcon(Icon_Cancel)
			         ->SetCaption(default_action)
			         ->SetCallbackOnClick(DefineCallback(this.CloseListSelectionMenu))
			         ->SetCallbackOnMouseIn(ammo_list->DefineCallback(ammo_list.SelectEntry, default_action))
			         ->SetScrollHint(true);
		ammo_list->AddEntry(default_action, default_entry);
		this->~SetListSelectionMenuHotkey(default_entry, 9);

		// Add entries for the ammo types
		for (var ammo_type in available_types) 
		{
			// Collect the current info
			var ammo_info = GetCreateAmmoInfo(ammo_type, GetAmmoCount());

			// Text and description
			var text_color = 0xffffffff;
			var description = Format("$UnpackAmmo$", ammo_info.ammo_count, ammo_type->GetName(), CMC_Ammo_Box->GetName());
			var call_on_click = this.CreateAmmoBox;

			// Not available if it would not create any ammo
			if (ammo_info.ammo_count <= 0)
			{
				text_color = GUI_CMC_Text_Color_Inactive;
				description = Format("$PointsRequired$", ammo_info.points_required);
				call_on_click = this.CreateNothing;
			}

			// Not available if you require a special class to create it
			if (ammo_type != CMC_Ammo_Bullets && !HasAmmoAbility(user))
			{
				text_color = GUI_CMC_Text_Color_Inactive;
				description = Format("$AmmoAbilityRequired$", CMC_Ability_ImproveAmmoEquipment->GetName());
				call_on_click = this.CreateNothing;
			}

			var name = Format("<c %x>%s</c>", text_color, ammo_type->GetName());
			var entry = ammo_list->MakeEntryProplist();
			entry->SetIcon(ammo_type)
			     ->SetCaption(name)
			     ->SetCount(ammo_info.ammo_count)
			     ->SetCallbackOnClick(DefineCallback(call_on_click, user, ammo_type))
			     ->SetCallbackOnMouseIn(ammo_list->DefineCallback(ammo_list.SelectEntry, ammo_type))
			     ->SetScrollHint(true);
			ammo_list->AddEntry(ammo_type, entry);
			SetListSelectionMenuHotkey(entry, hotkey++);
		}
	}
}

func CreateNothing()
{
	// Dummy, because the click should not have an effect
}

func CreateAmmoBox(object user, id ammo_type)
{
	// Close the menu first
	CloseListSelectionMenu();

	AssertNotNil(user);
	AssertNotNil(ammo_type);

	// Get info again, in case something has changed in the meantime
	var ammo_info = GetCreateAmmoInfo(ammo_type, GetAmmoCount());

	// Create the box
	var ammo_box = CreateObject(CMC_Ammo_Box, 0, 0, user->GetOwner());
	var ammo = ammo_box->CreateContents(ammo_type);
	ammo->SetStackCount(ammo_info.ammo_count);

	// Remove points
	DoAmmoCount(-ammo_info.points_cost);
	ammo_box->Sound("Items::Tools::AmmoBox::ResupplyOut?", {player = user->GetOwner()});

	// Remove if empty, do this before collecting the ammo box, so that you can unpack from a full inventory
	RemoveEmptyAmmoBag(user);

	// Collect it
	if (user->Collect(ammo_box))
	{
		user->ShiftContents(false, ammo_box->GetID()); // TOOD: would be cool to shift right to that item, no???
	}
	else
	{
	    user->PlayerMessage(user->GetOwner(), "$NoSpace$");
	}
}


// Find all ammo types
func GetAvailableAmmoTypes()
{
	var ammo_types = [];
	var ammo_type;

	// Get all ammo - whether you can unpack this ammo or not is decided later on in the menu.
	for (var i = 0; ammo_type = GetDefinition(i, C4D_StaticBack); ++i)
	{
		if (ammo_type->~IsAmmo())
		{
			PushFront(ammo_types, ammo_type);
		}
	}
	return ammo_types;
}


// Creates a convenient proplist that contains all the necessary info
func GetCreateAmmoInfo(id ammo_type, int available_points)
{
	var max = ammo_type->~MaxStackCount();
	var exchange_rate = ammo_type->~GetSupplyBoxCost() ?? 1;
	var units = BoundBy(available_points / exchange_rate, 0, max);
	return {
		ammo_count = units,
		ammo_max = max,
		points_cost = units * exchange_rate,
		points_required = max * exchange_rate,
	};
}


func HasAmmoAbility(object user)
{
	return user 
	    && user->~GetCrewClass()                                               // User has the function for getting the class
	    && user->GetCrewClass()->HasAbility(CMC_Ability_ImproveAmmoEquipment); // Class has the required ability
}


func RemoveEmptyAmmoBag(object user)
{
	if (GetAmmoCount() == 0 && !AllowAmmoRefill(user))
	{
		RemoveObject();
		user->~UpdateAttach();
		return true;
	}
	return false;
}


/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
