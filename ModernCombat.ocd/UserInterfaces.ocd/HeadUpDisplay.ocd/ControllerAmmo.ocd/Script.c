/**
	Shows the current ammo on the top left corner.

	@author Marky
*/

local gui_cmc_ammo;

/*--- Creation / Destruction ---*/

private func Construction()
{
	gui_cmc_ammo = {};
	gui_cmc_ammo.Menu = AssembleAmmo();
	gui_cmc_ammo.ID = GuiOpen(gui_cmc_ammo.Menu);
	
	CreateAmmoCounters();
	
	return _inherited(...);
}


private func Destruction()
{
	GuiClose(gui_cmc_ammo.ID);
	gui_cmc_ammo.ID = nil;

	_inherited(...);
}

/*--- Callbacks ---*/

public func OnCrewRecruitment(object clonk, int player)
{
	ScheduleUpdateAmmo();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeRecruitment(object clonk, int player)
{
	ScheduleUpdateAmmo();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeath(object clonk, int killer)
{
	ScheduleUpdateAmmo();

	return _inherited(clonk, killer, ...);
}


public func OnCrewDestruction(object clonk)
{
	ScheduleUpdateAmmo();

	return _inherited(clonk, ...);
}


public func OnCrewDisabled(object clonk)
{
	ScheduleUpdateAmmo();

	return _inherited(clonk, ...);
}


public func OnCrewEnabled(object clonk)
{
	ScheduleUpdateAmmo();

	return _inherited(clonk, ...);
}


public func OnCrewSelection(object clonk, bool unselect)
{
	ScheduleUpdateAmmo();

	return _inherited(clonk, unselect, ...);
}


public func OnSlotObjectChanged(int slot)
{
	ScheduleUpdateAmmo();

	return _inherited(slot, ...);
}


public func OnAmmoChange(object clonk)
{
	ScheduleUpdateAmmo();

	return _inherited(clonk);
}

/*--- GUI definition ---*/

private func AssembleAmmo()
{
	var menu = {
		Target = this,
		Player = NO_OWNER, // will be shown once a gui update occurs
		Style = GUI_Multiple | GUI_NoCrop | GUI_IgnoreMouse,
	};
	
	return menu;
}


private func CreateAmmoCounters()
{
	gui_cmc_ammo.counters = [];
	
	var ammo_types = [];
	var ammo_type;

	// Get all ammo
	for (var i = 0; ammo_type = GetDefinition(i, C4D_StaticBack); ++i)
	{
		if (ammo_type->~IsAmmo())
		{
			PushFront(ammo_types, ammo_type);
		}
	}
	
	// Need to remove some buttons?
	for (var type in ammo_types)
	{
		CreateAmmoCounter(GetLength(ammo_types), type);
	}
}


private func CreateAmmoCounter(int max_counters, id ammo_type)
{
	var counter_number = GetLength(gui_cmc_ammo.counters);
	var counter_info =
	{
		ID = counter_number + 1,
		Type = ammo_type,
		Amount = 0,
		Crew = nil,
		Dummy = CreateContents(Dummy),
	};
	PushBack(gui_cmc_ammo.counters, counter_info);

	// Redirect the call back through a dummy, so that we have
	// a) a separate effect for every ammo symbol
	// b) access to this gui in the callback
	counter_info.Dummy.OnAmmoUpdateNotification = this.DelegateUpdateNotification;
	counter_info.Dummy.Delegate = this;
	
	var counter = AssembleAmmoCounter(max_counters, counter_number, counter_info);

	GuiUpdate({_new_icon = counter}, gui_cmc_ammo.ID);
}


private func AssembleAmmoCounter(int max_counters, int counter_number, proplist counter_info)
{
	// The gui already exists, only update it with a new submenu
	var pos = GuiCalculateGridElementPosition(AmmoCounterGridLayout(max_counters), counter_number, 0);

	var counter = 
	{
		Target = this,
		Style = GUI_NoCrop,
		ID = counter_info.ID,
		Style = GUI_NoCrop,
		Count = {
			Target = this,
			Right = ToEmString(20),
			Bottom = ToEmString(12),
			Text = "",
			Style = GUI_TextRight | GUI_TextVCenter,
		},
		Icon = {	
			Symbol = counter_info.Type,
			Style = GUI_NoCrop,
		},
	};
	
	var size = 1000;
	var layout = {
		Prototype = GUI_BoxLayout,
		Align = {X = GUI_AlignCenter, Y = GUI_AlignCenter},
		Width = size, Height = size,
	};

	AddProperties(counter.Icon, GuiCalculateBoxElementPosition(layout));
	
	return AddProperties(counter, pos);
}


private func AmmoCounterGridLayout(int max_counters)
{
	var grid_margin_top = 2 * GUI_Controller_CrewBar_CursorMargin
	                        + GUI_Controller_CrewBar_CursorSize
	                    + 2 * GUI_Controller_CrewBar_BarMargin
	                        + GUI_Controller_CrewBar_BarSize;

	var grid_margin_left = 5;

	var icon_size = 12;
	return
	{
		Grid = {
			Prototype = GUI_BoxLayout,
			Margin = {Left = grid_margin_left, Top = grid_margin_top},
			Align = { X = GUI_AlignLeft,  Y = GUI_AlignTop,},
			Rows = max_counters,
			Columns = 1,
			Dimension = Global.ToEmString,
		},
		Cell = {
			Prototype = GUI_BoxLayout,
			Width = icon_size,
			Height = icon_size,
			Dimension = Global.ToEmString,
		}
	};
}

/*--- Drawing / display ---*/

private func ScheduleUpdateAmmo()
{
	if (!GetEffect("FxUpdateAmmo", this))
		CreateEffect(FxUpdateAmmo, 1, 1);
}


local FxUpdateAmmo = new Effect
{
	Timer = func ()
	{
		Target->UpdateAmmo(this.Type);
		return FX_Execute_Kill;
	},
};


// Update everything
private func UpdateAmmo()
{
	var cursor = GetCursor(GetOwner());
	
	if (GuiShowForCrew(gui_cmc_ammo, GetOwner(), cursor))
	{
		var weapon = cursor->~GetCurrentItem();
		for (var counter_info in gui_cmc_ammo.counters)
		{
			UpdateAmmoCounter(cursor, weapon, counter_info);
		}
	}
}


private func UpdateAmmoCounter(object cursor, object weapon, proplist counter_info)
{
	var amount = cursor->GetAmmo(counter_info.Type);
	
	// Check for symbol update
	
	var ammodiff = 0;
	if (cursor == counter_info.Crew)
	{
		ammodiff = amount - counter_info.Amount;
	}
	counter_info.Amount = amount;
	counter_info.Crew = cursor;

	// Compose the update!
	var update =
	{
		Count = {Text = Format("%d", amount)},
	};

	GuiUpdate(update, gui_cmc_ammo.ID, counter_info.ID, this);
}
