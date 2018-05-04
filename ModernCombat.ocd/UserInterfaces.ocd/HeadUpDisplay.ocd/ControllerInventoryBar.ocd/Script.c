/**
	ControllerInventoryBar

	Displays inventory slots and extra information.

	@authors Zapper, Clonkonaut, Marky
*/


// Proplist for saving the menu layouts, GUI ID and so on.
local gui_cmc_inventory;

/* --- Creation / Destruction --- */

func Construction()
{
	gui_cmc_inventory = {};
	gui_cmc_inventory.Slots = []; // Array with individual inventory slots
	gui_cmc_inventory.Menu = new GUI_Element
	{
		Target = this,
		Player = NO_OWNER, // will be shown once a gui update occurs
		Style = GUI_Multiple | GUI_IgnoreMouse | GUI_NoCrop,
	};
	gui_cmc_inventory.Menu->Open(GetOwner());

	return _inherited(...);
}


func Destruction()
{
	gui_cmc_inventory.Menu->Close();

	_inherited(...);
}

/* --- Callbacks from the HUD adapter --- */

public func OnCrewRecruitment(object clonk, int player)
{
	ScheduleUpdateInventory();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeRecruitment(object clonk, int player)
{
	ScheduleUpdateInventory();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeath(object clonk, int killer)
{
	ScheduleUpdateInventory();

	return _inherited(clonk, killer, ...);
}


public func OnCrewDestruction(object clonk)
{
	ScheduleUpdateInventory();

	return _inherited(clonk, ...);
}


public func OnCrewDisabled(object clonk)
{
	ScheduleUpdateInventory();

	return _inherited(clonk, ...);
}


public func OnCrewEnabled(object clonk)
{
	ScheduleUpdateInventory();

	return _inherited(clonk, ...);
}


public func OnCrewSelection(object clonk, bool unselect)
{
	ScheduleUpdateInventory();

	return _inherited(clonk, unselect, ...);
}


public func OnInventoryChange()
{
	ScheduleUpdateInventory();

	return _inherited(...);
}


public func OnSlotObjectChanged(int slot)
{
	ScheduleUpdateInventory();

	return _inherited(slot, ...);
}


public func OnAmmoChange(object clonk)
{
	ScheduleUpdateInventory();

	return _inherited(clonk);
}


/* --- GUI definition --- */

// Overload this if you want to change the layout
func AssembleInventoryButton(int slot)
{
	var button = new GUI_Element
	{
		ID = slot + 1,
		BackgroundColor =
		{
			Std = GUI_CMC_Background_Color_Default,
			Selected = GUI_CMC_Background_Color_Highlight,
		}
	};
	button->SetWidth(GUI_CMC_Element_Info_Width)
	      ->SetHeight(GUI_CMC_Element_Large_Height);
	
	return button;
}



/* --- Drawing / display --- */

/*
	Callback from the HUD adapter.
	
	Just update the item status, too.
 */
public func ScheduleUpdateInventory()
{
	ScheduleUpdateInventory();
	return _inherited(...);
}


/*
	Schedules an update of the bar for the next frame.
	
	@par bar The name of the bar if you want to update only of the bars. Pass 'nil' to update all bars.
 */
public func ScheduleUpdateInventory()
{
	var timer = GetEffect("ScheduledInventoryUpdateTimer", this) ?? CreateEffect(ScheduledInventoryUpdateTimer, 1, 1);
}


// Update timer
local ScheduledInventoryUpdateTimer = new Effect
{
	Timer = func ()
	{
		Target->UpdateInventory();
		return FX_Execute_Kill;
	},
};


// Update the inventory
func UpdateInventory()
{
	Log("Update inventory");
	var cursor = GetCursor(GetOwner());
	
	if (gui_cmc_inventory.Menu->ShowForCrew(cursor))
	{
		UpdateInventoryButtonAmount(cursor.MaxContentsCount);

		// update inventory-slots
		var selected_item_index = cursor->~GetHandItemPos(0);
		var quick_switch_index = cursor->~GetQuickSwitchSlot();
	
		for (var item_index = 0; item_index < GetLength(gui_cmc_inventory.Slots); ++item_index)
		{
			var slot = gui_cmc_inventory.Slots[item_index];
			var item = cursor->GetItem(item_index);
			
			var tag = nil;
			if (item_index == selected_item_index)
			{
				tag = "Selected";
			}
			
			slot.Symbol = item;
			slot->Show()->Update();
			GuiUpdateTag(tag, gui_cmc_inventory.Menu->GetRootID(), slot.ID);
		}
	}
}



func UpdateInventoryButtonAmount(int max_contents_count)
{
	Log("Update inventory button amount %d", max_contents_count);

	var old_count = GetLength(gui_cmc_inventory.Slots);
	if (old_count != max_contents_count)
	{
		// Need to create more inventory buttons?
		for (var i = old_count; i < max_contents_count; ++i)
		{
			gui_cmc_inventory.Slots[i] = AssembleInventoryButton(i);
			gui_cmc_inventory.Slots[i]->AddTo(gui_cmc_inventory.Menu);
		}
	
		// Need to remove some inventory buttons?
		for (var i = old_count; i > max_contents_count; --i)
		{
			var slot = gui_cmc_inventory.Slots[i - 1];
			GuiClose(gui_cmc_inventory.Menu->GetRootID(), slot.ID, this);
		}
		SetLength(gui_cmc_inventory.Slots, max_contents_count);
		
		// Adjust positions
		var margin = new GUI_Dimension{};
		margin->SetPercent(GUI_CMC_Margin_Element_V);
		
		var margin_small = new GUI_Dimension{};
		margin_small->SetPercent(GUI_CMC_Margin_Element_Small_V);
		
		var position;
		// Depends on include order, unfortunately
		if (this.gui_cmc_item_status)
		{
			position = this.gui_cmc_item_status.Menu->GetTop();
		}
		else if (this.gui_cmc_crew)
		{
			position = this.gui_cmc_crew.Menu->GetTop();
		}
		else
		{
			position = new GUI_Dimension {};
			position->SetPercent(1000);
		}
		position = position->Subtract(margin);
		
		
		for (var i = max_contents_count; i > 0; --i)
		{
			// Align the slot
			var slot = gui_cmc_inventory.Slots[i - 1];
			slot->AlignRight(1000 - GUI_CMC_Margin_Screen_H);
			slot->AlignBottom(position);
			
			// Update top position
			position = slot->GetTop()->Subtract(margin_small);
		}
	}
}
