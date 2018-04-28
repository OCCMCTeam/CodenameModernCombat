/**
	Shows the status of the selected item in the bottom right corner.
	
	Should contain:
	- "object count" for counters, such as ammo in the object
	- "total count" that displays the maximum value of object count
	- "slash", a separator for object and ammo count
	- "grenade count" a counter for grenade items
	- a button bar

	@author Marky
*/

// Proplist for saving the menu layouts, GUI ID and so on.
local gui_cmc_item_status;

static const GUI_CMC_ITEM_STATUS_SUBWINDOW_ID1 = 1;

/* --- Creation / Destruction --- */

func Construction()
{
	// FIXME the properties
	// .Menu and .ID are part of my convention for GuiShowForCrew();
	// a better solution would be cool :)
	gui_cmc_item_status = {};
	gui_cmc_item_status.Menu = AssembleItemStatus();
	gui_cmc_item_status.Object_Count = new GUI_Counter{};
	gui_cmc_item_status.Total_Count = new GUI_Counter{};
	gui_cmc_item_status.Grenade_Count = new GUI_Counter{};
	gui_cmc_item_status.Slash = new GUI_Element_Controller{};
	gui_cmc_item_status.InfoText = new GUI_Element_Controller{};
	
	gui_cmc_item_status.ID = GuiOpen(gui_cmc_item_status.Menu);
	
	GetObjectCount()->AddTo(gui_cmc_item_status.Menu, "object_count", gui_cmc_item_status.ID, GUI_CMC_ITEM_STATUS_SUBWINDOW_ID1, this)
	                ->SetReference(gui_cmc_item_status.Menu.field)
	                ->SetMaxDigits(3)
	                ->ShowTrailingZeros(true)
	                ->SetValue(0);
	GetTotalCount()->AddTo(gui_cmc_item_status.Menu, "total_count", gui_cmc_item_status.ID, GUI_CMC_ITEM_STATUS_SUBWINDOW_ID1, this)
	               ->SetReference(gui_cmc_item_status.Menu.field)
	               ->SetMaxDigits(3)
	               ->ShowTrailingZeros(true)
	               ->SetValue(0);
	GetGrenadeCount()->AddTo(gui_cmc_item_status.Menu, "grenade_count", gui_cmc_item_status.ID, GUI_CMC_ITEM_STATUS_SUBWINDOW_ID1, this)
	                 ->SetReference(gui_cmc_item_status.Menu.field)
	                 ->SetMaxDigits(1)
	                 ->SetValue(0);
	GetSlash()->AddTo(gui_cmc_item_status.Menu.field, "slash", gui_cmc_item_status.ID, GUI_CMC_ITEM_STATUS_SUBWINDOW_ID1, this)
	          ->Hide();
	
	GetInfoText()->AddTo(gui_cmc_item_status.Menu, "info_text", gui_cmc_item_status.ID, nil, this)
	             ->Hide();
	
	return _inherited(...);
}


func Destruction()
{
	GuiClose(gui_cmc_item_status.ID);
	gui_cmc_item_status.ID = nil;

	_inherited(...);
}

/* --- Callbacks from the HUD adapter --- */

public func OnCrewRecruitment(object clonk, int player)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeRecruitment(object clonk, int player)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeath(object clonk, int killer)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk, killer, ...);
}


public func OnCrewDestruction(object clonk)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk, ...);
}


public func OnCrewDisabled(object clonk)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk, ...);
}


public func OnCrewEnabled(object clonk)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk, ...);
}


public func OnCrewSelection(object clonk, bool unselect)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk, unselect, ...);
}


public func OnInventoryChange()
{
	ScheduleUpdateItemStatus();

	return _inherited(...);
}


public func OnSlotObjectChanged(int slot)
{
	ScheduleUpdateItemStatus();

	return _inherited(slot, ...);
}


public func OnAmmoChange(object clonk)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk);
}


/* --- GUI definition --- */

// Overload this if you want to change the layout
// The layout goes like this, with 3 lines:
//
// | Field               |           |
// |_____________________| <- W_B -> |
// |                     |           |
//
// where:
// W_B = width of buttons row = 20% (FIXME: should be 'em's, though, same goes for line height!
// 
// And the top left corner 'Field' is subdivided again, from left to right:
//
// 3 digits with 20% width each;
// 4 digits with 10% width each; the first of these is the slash, the other three for ammo, etc.
func AssembleItemStatus()
{
	var separator_button_row_h = 800;
	var line_height = 333; // There are 3 lines
	
	var field_large_digit_width = 200;
	var field_small_digit_width = 100;

	var menu = {
		Target = this,
		Player = NO_OWNER, // will be shown once a gui update occurs
		Style = GUI_Multiple | GUI_NoCrop | GUI_IgnoreMouse,
		
		BackgroundColor = GUI_CMC_Background_Color_Default,
		
		// Leftmost top element: Field of two lines height
		field = 
		{
			ID = GUI_CMC_ITEM_STATUS_SUBWINDOW_ID1,
			Right = ToPercentString(separator_button_row_h),
			Bottom = ToPercentString(2 * line_height),
			
			// Further elements in the field:
			
			// Large digits for object count
			object_count_digit_100 = 
			{
				Left =  ToPercentString(0 * field_large_digit_width),
				Right = ToPercentString(1 * field_large_digit_width),
				Symbol = Icon_SlimNumber,
			},
			object_count_digit_10 = 
			{
				Left =  ToPercentString(1 * field_large_digit_width),
				Right = ToPercentString(2 * field_large_digit_width),
				Symbol = Icon_SlimNumber,
			},
			object_count_digit_1 = 
			{
				Left =  ToPercentString(2 * field_large_digit_width),
				Right = ToPercentString(3 * field_large_digit_width),
				Symbol = Icon_SlimNumber,
			},

			// Separator
			slash = 
			{
				Left =  ToPercentString(0 * field_small_digit_width + 3 * field_large_digit_width),
				Right = ToPercentString(1 * field_small_digit_width + 3 * field_large_digit_width),
				Bottom = ToPercentString(500),
				Text = "/",
				Style = GUI_TextHCenter | GUI_TextVCenter,
			},
			
			// Small digits for total count
			total_count_digit_100 = 
			{
				Left =  ToPercentString(1 * field_small_digit_width + 3 * field_large_digit_width),
				Right = ToPercentString(2 * field_small_digit_width + 3 * field_large_digit_width),
				Bottom = ToPercentString(500),
				Symbol = Icon_SlimNumber,
			},
			total_count_digit_10 = 
			{
				Left =  ToPercentString(2 * field_small_digit_width + 3 * field_large_digit_width),
				Right = ToPercentString(3 * field_small_digit_width + 3 * field_large_digit_width),
				Bottom = ToPercentString(500),
				Symbol = Icon_SlimNumber,
			},
			total_count_digit_1 = 
			{
				Left =  ToPercentString(3 * field_small_digit_width + 3 * field_large_digit_width),
				Right = ToPercentString(4 * field_small_digit_width + 3 * field_large_digit_width),
				Bottom = ToPercentString(500),
				Symbol = Icon_SlimNumber,
			},
			
			// Small digits for grenade count
			grenade_count_digit_1 = 
			{
				Left =  ToPercentString(1 * field_small_digit_width + 3 * field_large_digit_width),
				Right = ToPercentString(2 * field_small_digit_width + 3 * field_large_digit_width),
				Top = ToPercentString(500),
				Symbol = Icon_SlimNumber,
			},
			grenade_count_icon = 
			{
				Left =  ToPercentString(2 * field_small_digit_width + 3 * field_large_digit_width),
				Right = ToPercentString(3 * field_small_digit_width + 3 * field_large_digit_width),
				Top = ToPercentString(500),
				Symbol = IronBomb
			},
		},
		
		// Leftmost bottom element: Detail text of one line height
		info_text = 
		{
			Right = ToPercentString(separator_button_row_h),
			Top = ToPercentString(2 * line_height),
			Bottom = ToPercentString(3 * line_height),
			Style = GUI_TextHCenter | GUI_TextVCenter,
		},
		
		// Rightmost element: A vertical button row
		vertical_button_row = 
		{
			Left = ToPercentString(separator_button_row_h),
			Right = ToPercentString(1000),
			
			button_1 = 
			{
				Top = ToPercentString(0 * line_height),
				Bottom = ToPercentString(1 * line_height),
				Symbol = Icon_SlimNumber,
				GraphicsName = "10",
			},
			
			button_2 = 
			{
				Top = ToPercentString(1 * line_height),
				Bottom = ToPercentString(2 * line_height),
				Symbol = Icon_SlimNumber,
				GraphicsName = "10",
			},
			
			button_3 = 
			{
				Top = ToPercentString(2 * line_height),
				Bottom = ToPercentString(3 * line_height),
				Symbol = Icon_SlimNumber,
				GraphicsName = "10",
			},
		},
	};
	
	AddProperties(menu, this->GuiItemStatusPositionLayout());
	
	return menu;
}


/*
	Gets the position layout of the item status.
	
	Has the height of 3 "default elements".
	
	@return proplist Position properties for a GUI: Left, Right, Top, Bottom.
	
	                 The values are in percent, relative to the screen size.
*/
public func GuiItemStatusPositionLayout()
{

	var top = 0;
	var bottom = 3 * GUI_CMC_Element_Default_Height;

	var right = 1000;
	var left = right - GUI_CMC_Element_Info_Width;

	var vertical_shift;
	
	// Depends on include order, unfortunately
	// Also, you have to shift down from the top in this case :/
	if (this.gui_cmc_crew)
	{
		var other_top = this.gui_cmc_crew.Menu.Top;
		vertical_shift = Format("%s %s", other_top, ToPercentString(-bottom - GUI_CMC_Margin_Element_Small_V));
	}
	else
	{
		vertical_shift = ToPercentString(1000 - bottom - GUI_CMC_Margin_Screen_V);
	}

	var position =
	{
		Left = ToPercentString(left),
		Right = ToPercentString(right),
		Top = ToPercentString(top),
		Bottom = ToPercentString(bottom),
	};
	return GUI_ShiftPosition(position, ToPercentString(-GUI_CMC_Margin_Screen_H), vertical_shift);
}

/* --- Access to certain layouts --- */

// Gets the counter proplist (is not a layout!)
public func GetGrenadeCount()
{
	return gui_cmc_item_status.Grenade_Count;
}


// Gets the counter proplist (is not a layout!)
public func GetObjectCount()
{
	return gui_cmc_item_status.Object_Count;
}


// Gets the counter proplist (is not a layout!)
public func GetTotalCount()
{
	return gui_cmc_item_status.Total_Count;
}

// Gets the slash layout
public func GetSlash()
{
	return gui_cmc_item_status.Slash;
}

// Gets the info text layout
public func GetInfoText()
{
	return gui_cmc_item_status.InfoText;
}


/* --- Drawing / display --- */

/*
	Callback from the HUD adapter.
	
	Just update the item status, too.
 */
public func ScheduleUpdateInventory()
{
	ScheduleUpdateItemStatus();
	return _inherited(...);
}


/*
	Schedules an update of the bar for the next frame.
	
	@par bar The name of the bar if you want to update only of the bars. Pass 'nil' to update all bars.
 */
public func ScheduleUpdateItemStatus()
{
	var timer = GetEffect("ScheduledItemStatusUpdateTimer", this) ?? CreateEffect(ScheduledItemStatusUpdateTimer, 1, 1);
}


// Update timer
local ScheduledItemStatusUpdateTimer = new Effect
{
	Timer = func ()
	{
		Target->UpdateItemStatus();
		return FX_Execute_Kill;
	},
};


// Update the bars
func UpdateItemStatus()
{
	var cursor = GetCursor(GetOwner());
	
	if (GuiShowForCrew(gui_cmc_item_status, GetOwner(), cursor))
	{
		var item = cursor->GetHandItem(0);
		
		// Object count and max count
		var object_count = 0;
		var total_count = 0;
		var show_counters = false;
		var firemode = nil;
		var ammo_type = nil;
		if (item)
		{
			if (item->~IsAmmoManager() && cursor->~IsAmmoManager())
			{
				var firemode = item->~GetFiremode();
				if (firemode)
				{
					ammo_type = firemode->GetAmmoID();
				}
				if (ammo_type)
				{
			    	object_count = item->GetAmmo(ammo_type);
			    	total_count = cursor->GetAmmo(ammo_type);
					show_counters = true;
				}
			}
		    else if (item && item->~IsStackable())
		    {
		    	object_count = item->GetStackCount();
		    	total_count = item->MaxStackCount();
				show_counters = true;
		    }
		}
		
		GetObjectCount()->SetValue(object_count);
		GetTotalCount()->SetValue(total_count);
		
		if (show_counters)
		{
			GetObjectCount()->Show();
			GetTotalCount()->Show();
			GetSlash()->Show();
		}
		else
		{
			GetObjectCount()->Hide();
			GetTotalCount()->Hide();
			GetSlash()->Hide();
		}
		
		// Grenades
		var grenade_count = 0;
		GetGrenadeCount()->SetValue(grenade_count);
		
		// Text info in the bottom row
		
		var info_text = nil;
		if (firemode && ammo_type)
		{
			// This should display a colored "fire mode" - "fire technique"
			// In the shooter library the CMC fire technique is named firemode
			// whereas the CMC fire mode is actually the ammo type that the weapon uses
			info_text = Format("<c %x>%s</c> - %s", GUI_CMC_Text_Color_Highlight, ammo_type->GetName(), firemode->GetName());
		}
		else if (item)
		{
			info_text = item->GetName();
		}
		
		if (info_text)
		{
			GetInfoText()->Show();
			GetInfoText().Text = info_text;
		}
		else
		{
			GetInfoText()->Hide();
		}

		// Actually update everything
		GetObjectCount()->Update();
		GetTotalCount()->Update();
		GetSlash()->Update();
		GetGrenadeCount()->Update();
		GetInfoText()->Update();
	}
}