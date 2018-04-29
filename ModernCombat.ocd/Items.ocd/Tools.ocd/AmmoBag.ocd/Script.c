/**
	Ammo Bag
*/

#include Library_Stackable
#include Library_CMC_Pack

/* --- Engine callbacks --- */

public func Initialize(...)
{
	SetGraphics(nil, DynamiteBox);
	this.PictureTransformation = Trans_Scale(); // Scale the picture of the box mesh, so that it does not appear in the game
	SetGraphics(nil, CMC_Ammo_Bag, 1, GFXOV_MODE_Picture);
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

// At the moment, this is a stack, because it seemed to be more in line 
// with the how things are in the predecessor;
// Can be switched to work like ammo later, if desired
//
// Having ammo instead of stack count might be better actually:
// Stack always gets removed if the thing is empty, ammo does not
public func MaxStackCount() { return 200; }

public func GetStackRefillInterval() { return 20; }

public func AllowStackRefill(object user)
{
	return true; // TODO: Allowed only for support class
}

/* --- User Interface --- */

local active_menu;

func ControlUse(object user, int x, int y)
{
	// User has to be ready to act
	if (user->~HasActionProcedure())
	{
		user->SetComDir(COMD_Stop);

		OpenMenuCreateAmmoBox(user);
	}
	return true;
}

// Opens the 
public func OpenMenuCreateAmmoBox(object user)
{
	// Close existing menu
	CloseMenuCreateAmmoBox();
	
	// If another menu is already open cancel the action.
	if (user->~GetMenu())
	{
		return;
	}
	
	var description_top = "70%";
	var header_size = "+2em";
	
	// Create a menu
	var main_menu = 
	{
		Left = "30%", Right = "70%", Top = "20%", Bottom = "80%",
		header =
		{
			Bottom = header_size,
			Text = "$TakeAmmo$",
			Style = GUI_TextHCenter | GUI_TextVCenter,
			BackgroundColor = GUI_CMC_Background_Color_Default,
			icon =
			{
				Left = "0.1em",
				Right = Format("0.1em %s", header_size),
				actual_icon =
				{
					Symbol = this,
					Left = "20%",
					Right = "80%",
					Top = "20%",
					Bottom = "80%"
				}
			}
		},
		body =
		{
			Top = header_size, 
		    list_display = 
		    {
		    	Left = "0.2em",
		    	Bottom = description_top,
		    	BackgroundColor = GUI_CMC_Background_Color_Default, 
	    	},
		    description_display =
		    {
		    	Top = description_top, 
		    	BackgroundColor = GUI_CMC_Background_Color_Default, 
		    	text_field =
		    	{
		    		ID = 9999, 
		    		Left = "0.6em", 
		    		Right = "100%-0.5em"
	    		}
    		}
		},
	};
	GuiAddCloseButton(main_menu, this, GetFunctionName(this.CloseMenuCreateAmmoBox));
	active_menu = {};
	
	// Fill with contents
	var available_types = GetAvailableAmmoTypes();
	if (GetLength(available_types) == 0)
	{
		main_menu.body.list_display.Style = GUI_TextHCenter | GUI_TextVCenter;
		main_menu.body.list_display.Text = "$NoAmmoAvailable$";
	}
	else
	{
		var ammo_list = CreateObject(MenuStyle_List);
		main_menu.body.list_display.list = ammo_list;
		
		ammo_list->SetMouseOverCallback(this, this.OnHoverCreateAmmoBox);
		

		for (var ammo_type in available_types) 
		{
			// Collect the current info
			var ammo_info = GetCreateAmmoInfo(ammo_type, GetStackCount());

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
			
			var name = Format("<c %x>%d %s</c>", text_color, ammo_info.ammo_count, ammo_type->GetName());
			ammo_list->AddItem(ammo_type, name, nil, this, call_on_click, {Target = user, Type = ammo_type});
		}
	}
	
	active_menu.user = user;
	active_menu.ID = GuiOpen(main_menu);
	active_menu.user->~SetMenu(active_menu.ID);
}

public func CloseMenuCreateAmmoBox()
{
	if (active_menu)
	{
		GuiClose(active_menu.ID);
		if (active_menu.upgrade_list) active_menu.upgrade_list->RemoveObject();
		if (active_menu.user) active_menu.user->MenuClosed();
	}
	active_menu = nil;
}

func OnHoverCreateAmmoBox(proplist upgrade, int ID)
{
	GuiUpdateText(upgrade.Info, active_menu.ID, 9999);
}

func CreateNothing()
{
	// Dummy, because the click should not have an effect
}

func CreateAmmoBox(proplist parameters)
{
	// Close the menu first
	CloseMenuCreateAmmoBox();

	AssertNotNil(parameters);
	
	var user = parameters.Target;
	var ammo_type = parameters.Type;
	
	AssertNotNil(user);
	AssertNotNil(ammo_type);

	// Get info again, in case something has changed in the meantime
	var ammo_info = GetCreateAmmoInfo(ammo_type, GetStackCount());
	
	// Create the box
	var ammo_box = CreateObject(CMC_Ammo_Box, 0, 0, user->GetOwner());
	var ammo = ammo_box->CreateContents(ammo_type);
	ammo->SetStackCount(ammo_info.ammo_count);
	
	// Remove points
	DoStackCount(-ammo_info.points_cost);
	ammo_box->Sound("Tool::AmmoBox::ResupplyOut?", {player = user->GetOwner()});

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


/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
