/**
	Ally Info

	Displays a bar for each (allied) player, containing:
	- Player name
	- Player rank
	- Information about the selected crew: Health, class, status icons

	@authors Marky
*/


// Proplist for saving the menu layouts, GUI ID and so on.
local gui_cmc_ally_info;

/* --- Creation / Destruction --- */

func Construction()
{
	gui_cmc_ally_info = {};
	gui_cmc_ally_info.Allies = []; // Array with ally info
	gui_cmc_ally_info.Info = []; // Array with individual slots
	gui_cmc_ally_info.Menu = new GUI_Element
	{
		Target = this,
		Player = NO_OWNER, // will be shown once a gui update occurs
		Style = GUI_Multiple | GUI_IgnoreMouse | GUI_NoCrop,
	};
	gui_cmc_ally_info.Menu->Open(GetOwner())->Show()->Update();

	return _inherited(...);
}


func Destruction()
{
	gui_cmc_ally_info.Menu->Close();

	_inherited(...);
}

/* --- Callbacks from the HUD adapter --- */

public func OnCrewRecruitment(object clonk, int player)
{
	ScheduleUpdateAllyInfo();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeRecruitment(object clonk, int player)
{
	ScheduleUpdateAllyInfo();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeath(object clonk, int killer)
{
	ScheduleUpdateAllyInfo();

	return _inherited(clonk, killer, ...);
}


public func OnCrewDestruction(object clonk)
{
	ScheduleUpdateAllyInfo();

	return _inherited(clonk, ...);
}


public func OnCrewDisabled(object clonk)
{
	ScheduleUpdateAllyInfo();

	return _inherited(clonk, ...);
}


public func OnCrewEnabled(object clonk)
{
	ScheduleUpdateAllyInfo();

	return _inherited(clonk, ...);
}


public func OnCrewSelection(object clonk, bool unselect)
{
	ScheduleUpdateAllyInfo();

	return _inherited(clonk, unselect, ...);
}


/* --- GUI definition --- */

// Overload this if you want to change the layout
func AssembleAllyInfo(int slot)
{
	var button = new CMC_GUI_Ally_Info { ID = slot + 1};
	button->Assemble();
	
	return button;
}



/* --- Drawing / display --- */

/*
	Schedules an update of the element for the next frame.
 */
public func ScheduleUpdateAllyInfo()
{
	var timer = GetEffect("ScheduledAllyInfoUpdateTimer", this) ?? CreateEffect(ScheduledAllyInfoUpdateTimer, 1, 1);
}


// Update timer
local ScheduledAllyInfoUpdateTimer = new Effect
{
	Timer = func ()
	{
		Target->UpdateAllyInfo();
		return FX_Execute_Kill;
	},
};


// Update the ally info
func UpdateAllyInfo()
{
	UpdateAllyAmount();
	
	for (var i = 0; i < GetLength(gui_cmc_ally_info.Allies); ++i)
	{
		var ally = gui_cmc_ally_info.Allies[i];
		var info = gui_cmc_ally_info.Info[i];

		var color = nil;
		if (ally == GetOwner())
		{
			color = GUI_CMC_Text_Color_Highlight;
		}
		
		// Player info

		info->SetNameLabel(GetPlayerName(ally), color);
		info->SetRankSymbol(Rule_TeamAccount);		
		info->Update();		

		// Selected clonk info
		var cursor = GetCursor(ally);
		
		if (info->GetHealthBar()->ShowForCrew(cursor))
		{
			info->GetHealthBar()->SetHealth(cursor);
		}
	}
}


func UpdateAllyAmount()
{
	var old_count = GetLength(gui_cmc_ally_info.Allies);
	gui_cmc_ally_info.Allies = GetAlliedPlayers(GetOwner());
	var new_count = GetLength(gui_cmc_ally_info.Allies);

	if (old_count != new_count)
	{
		// Need to create more ally info buttons?
		for (var i = old_count; i < new_count; ++i)
		{
			gui_cmc_ally_info.Info[i] = AssembleAllyInfo(i);
			gui_cmc_ally_info.Info[i]->AddTo(gui_cmc_ally_info.Menu)->Show();
		}
	
		// Need to remove some ally info buttons?
		for (var i = old_count; i > new_count; --i)
		{
			var slot = gui_cmc_ally_info.Info[i - 1];
			GuiClose(gui_cmc_ally_info.Menu->GetRootID(), slot.ID, this);
		}
		SetLength(gui_cmc_ally_info.Info, new_count);
		
		// Adjust positions
		var margin_small = GuiDimensionCmc(nil, GUI_CMC_Margin_Element_Small_V);
		
		var position = GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_V);
		
		for (var i = new_count; i > 0; --i)
		{
			// Align the slot
			var slot = gui_cmc_ally_info.Info[i - 1];
			slot->AlignLeft(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_H));
			slot->AlignTop(position);
			
			// Update top position
			position = slot->GetBottom()->Add(margin_small);
		}
	}
}
