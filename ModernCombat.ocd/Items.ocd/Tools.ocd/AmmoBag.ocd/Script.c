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
		// No ammo and cannot refill? Destroy
		if (RemoveEmptyAmmoBag(user))
		{
			return true;
		}

		user->SetComDir(COMD_Stop);
		OpenListSelectionMenu(user);
	}
	return true;
}


// Opens the 
public func GetListSelectionMenuEntries(object user, string type, proplist main_menu)
{
	main_menu->SetHeaderCaption("$TakeAmmo$");

	// Fill with contents
	var available_types = GetAvailableAmmoTypes();
	if (GetLength(available_types) == 0)
	{
		FatalError("TODO: Add an ingame error message here....");
	}
	else
	{
		var ammo_list = main_menu->GetList();
		
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
			
			var name = Format("<c %x>%s (%dx)</c>", text_color, ammo_type->GetName(), ammo_info.ammo_count);
			var menu_item = ammo_list->AddItem(ammo_type, name, nil, this, call_on_click, {Target = user, Type = ammo_type});
			ammo_list->AddButtonPrompt(menu_item);
		}
	}
}

func CreateNothing()
{
	// Dummy, because the click should not have an effect
}

func CreateAmmoBox(proplist parameters)
{
	// Close the menu first
	CloseListSelectionMenu();

	AssertNotNil(parameters);
	
	var user = parameters.Target;
	var ammo_type = parameters.Type;
	
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

	// Get all ammo
	// TODO - Only support class should be able to create all ammo types, but for testing purposes this is not implemented yet
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
