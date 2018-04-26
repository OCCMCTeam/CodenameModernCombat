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

/* --- Creation / Destruction --- */

func Construction()
{
	// FIXME the properties
	// .Menu and .ID are part of my convention for GuiShowForCrew();
	// a better solution would be cool :)
	gui_cmc_item_status = {};
	gui_cmc_item_status.Menu = AssembleItemStatus();
	gui_cmc_item_status.ID = GuiOpen(gui_cmc_item_status.Menu);
	
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
func AssembleItemStatus()
{
	var menu = {
		Target = this,
		Player = NO_OWNER, // will be shown once a gui update occurs
		Style = GUI_Multiple | GUI_NoCrop | GUI_IgnoreMouse,
		
		BackgroundColor = GUI_CMC_Background_Color_Default,
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

/* --- Drawing / display --- */

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
	}
}
