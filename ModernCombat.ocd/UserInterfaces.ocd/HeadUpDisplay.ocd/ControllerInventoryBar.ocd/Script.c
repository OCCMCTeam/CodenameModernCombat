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
	gui_cmc_inventory.Collapse = []; // Array with individual collapse effects
	gui_cmc_inventory.Menu = new GUI_Element
	{
		Target = this,
		Player = NO_OWNER, // will be shown once a gui update occurs
		Style = GUI_Multiple | GUI_IgnoreMouse | GUI_NoCrop,
		Priority = GUI_CMC_Priority_HUD,
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
	ScheduleUpdateInventory(true);

	return _inherited(slot, ...);
}

public func OnSlotObjectUnchanged(int slot)
{
	ScheduleUpdateInventory(true);

	return _inherited(slot, ...);
}

public func OnAmmoChange(object clonk)
{
	ScheduleUpdateInventory();

	return _inherited(clonk);
}


public func OnCrewRelaunchStart(object clonk)
{
	ScheduleUpdateInventory();

	return _inherited(clonk, ...);
}


public func OnCrewRelaunchFinish(object clonk)
{
	ScheduleUpdateInventory();

	return _inherited(clonk, ...);
}


/* --- GUI definition --- */

// Overload this if you want to change the layout
func AssembleInventoryButton(int slot)
{
	var button = new GUI_CMC_InventoryButton
	{
		ID = slot + 1,
	};
	button->Assemble(slot + 1);
	return button;
}



/* --- Drawing / display --- */

/*
	Schedules an update of the bar for the next frame.
	
	@par bar The name of the bar if you want to update only of the bars. Pass 'nil' to update all bars.
 */
public func ScheduleUpdateInventory(bool selection_changed)
{
	var timer = GetEffect("ScheduledInventoryUpdateTimer", this) ?? CreateEffect(ScheduledInventoryUpdateTimer, 1, 1);
	timer.selection_changed = selection_changed;
}


// Update timer
local ScheduledInventoryUpdateTimer = new Effect
{
	Timer = func ()
	{
		Target->UpdateInventory(this.selection_changed);
		return FX_Execute_Kill;
	},
};


// Update the inventory
func UpdateInventory(bool selection_changed)
{
	var cursor = GetCursor(GetOwner());
	
	if (gui_cmc_inventory.Menu->ShowForCrew(cursor, cursor->~IsRespawning()))
	{
		UpdateInventoryButtonAmount(cursor.MaxContentsCount);

		// update inventory-slots
		var selected_item_index = cursor->~GetHandItemPos(0);
		var quick_switch_index = cursor->~GetQuickSwitchSlot();

		for (var item_index = 0; item_index < GetLength(gui_cmc_inventory.Slots); ++item_index)
		{
			var slot = gui_cmc_inventory.Slots[item_index];
			var item = cursor->GetItem(item_index);
			
			if (selection_changed)
			{
				SetInventoryButtonCompact(item_index, false, 5);
			}
			
			slot->SetInfo(item, item_index == selected_item_index);
		}
	}
}


func UpdateInventoryButtonAmount(int max_contents_count)
{
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
		var margin = GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V);
		var margin_small = GuiDimensionCmc(nil, GUI_CMC_Margin_Element_Small_V);
		
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
			slot->AlignRight(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_H));
			slot->AlignBottom(position);
			slot->Update(slot->ComposeLayout());
			
			// Update top position
			position = slot->GetTop()->Subtract(margin_small);
		}
	}
}


func SetInventoryButtonCompact(int index, bool compact, int blend_time)
{
	var change = BoundBy(100 / (blend_time ?? 25), 1, 100);
	if (compact)
	{
		change *= -1;
	}
	
	var fx = this.gui_cmc_inventory.Collapse[index] ?? CreateEffect(this.CollapseButtonEffect, 1, 1);
	fx.index = index;
	fx.collapse_timeout = 35;
	fx.change = change;
	this.gui_cmc_inventory.Collapse[index] = fx;
}


local CollapseButtonEffect = new Effect
{
	Timer = func ()
	{
		var button = this.Target.gui_cmc_inventory.Slots[this.index];
		var max = 100;
		if (this.change)
		{
			button.GUI_Compactness = BoundBy(button.GUI_Compactness + this.change, 0, max);

			var width = button.GUI_Width_Compact * (max - button.GUI_Compactness)
			          + button.GUI_Width_Expanded * button.GUI_Compactness;

			width /= max;
			
			var text = button.GUI_Item_Name;
			if (text)
			{
				if (button.GUI_Compactness <= 0)
				{
					text = nil;
				}
				else
				{
					text = Format("<c %x>%s</c>", RGBa(255, 255, 255, BoundBy(button.GUI_Compactness * 255 / 100, 0, 255)), text);
				}
			}
			button.item_name.Text = text;
			button.item_name->Update({Text = text});
			button.bar->SetWidth(GuiDimensionCmc(nil, width))->AlignRight()->Update();

			if (button.GUI_Compactness >= max)
			{
				this.change = 0;
			}
			else if (button.GUI_Compactness <= 0)
			{
				return FX_Execute_Kill;
			}
		}
		else
		{
			this.collapse_timeout -= 1;
			
			if (this.collapse_timeout <= 0)
			{
				this.Target->SetInventoryButtonCompact(this.index, true);
			}
		}
		return FX_OK;
	}
};
