/**
	Shows health and breath bar in the bottom left corner

	@author Marky
*/

// Proplist for saving the menu layouts, GUI ID and so on.
local gui_cmc_crew;

/* --- Creation / Destruction --- */

func Construction()
{
	gui_cmc_crew = {};
	gui_cmc_crew.Menu = AssembleCrewBarsPosition();
	gui_cmc_crew.Health_Bar = AssembleHealthBar();
	gui_cmc_crew.Breath_Bar = AssembleBreathBar();
	
	// Open the menu (is actually just the position for the bars) with the two bars added to it
	GetHealthBar()->AddTo(gui_cmc_crew.Menu);
	GetBreathBar()->AddTo(gui_cmc_crew.Menu);
	gui_cmc_crew.Menu->Open(GetOwner());
	
	return _inherited(...);
}


func Destruction()
{
	gui_cmc_crew.Menu->Close();

	_inherited(...);
}

/* --- Callbacks from the HUD adapter --- */

public func OnCrewRecruitment(object clonk, int player)
{
	ScheduleUpdateCrewBars();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeRecruitment(object clonk, int player)
{
	ScheduleUpdateCrewBars();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeath(object clonk, int killer)
{
	ScheduleUpdateCrewBars();

	return _inherited(clonk, killer, ...);
}


public func OnCrewDestruction(object clonk)
{
	ScheduleUpdateCrewBars();

	return _inherited(clonk, ...);
}


public func OnCrewDisabled(object clonk)
{
	ScheduleUpdateCrewBars();

	return _inherited(clonk, ...);
}


public func OnCrewEnabled(object clonk)
{
	ScheduleUpdateCrewBars();

	return _inherited(clonk, ...);
}


public func OnCrewSelection(object clonk, bool unselect)
{
	ScheduleUpdateCrewBars();

	return _inherited(clonk, unselect, ...);
}


public func OnCrewHealthChange(object clonk, int change, int cause, int caused_by)
{
	if (GetCursor(GetOwner()) == clonk)
	{
		ScheduleUpdateCrewBars(GetHealthBar()->GetName());
	}
	return _inherited(clonk, change, cause, caused_by, ...);
}


public func OnCrewBreathChange(object clonk, int change)
{
	if (GetCursor(GetOwner()) == clonk)
	{
		ScheduleUpdateCrewBars(GetBreathBar()->GetName());
	}
	return _inherited(clonk, change, ...);
}


/* --- GUI definition --- */

// Overload this if you want to change the layout
func AssembleCrewBarsPosition()
{
	var menu = new GUI_Element
	{
		Target = this,
		Player = NO_OWNER, // will be shown once a gui update occurs
		Style = GUI_Multiple | GUI_NoCrop | GUI_IgnoreMouse,
	};

	menu->SetWidth(GUI_CMC_Element_Info_Width)
	    ->SetHeight(GUI_CMC_Element_Default_Height)
	    ->AlignRight(1000 - GUI_CMC_Margin_Screen_H)
	    ->AlignBottom(1000 - GUI_CMC_Margin_Screen_V);

	return menu;
}


// Overload this if you want to change the layout
func AssembleHealthBar()
{
	// Health bar takes up the top 1/3rd of the position
	var health_bar = new CMC_GUI_ProgressBar {};
	health_bar->SetBottom(335)
	          ->SetBackgroundColor(GUI_CMC_Color_HealthBar_Transparent)
	          ->SetBarColor({
	          	Health_Full = GUI_CMC_Color_HealthBar_White,
	          	Health_Warn = GUI_CMC_Color_HealthBar_Opaque,
	          })
	          ->SetValue(1000); // Full in the beginning
	return health_bar;
}


// Overload this if you want to change the layout
private func AssembleBreathBar()
{
	// Health bar takes up the bottom 1/3rd of the position
	var breath_bar = new CMC_GUI_ProgressBar {};
	breath_bar->SetTop(665)
	          ->SetBackgroundColor(GUI_CMC_Color_BreathBar_Transparent)
	          ->SetBarColor(GUI_CMC_Color_BreathBar_Opaque)
	          ->SetValue(1000); // Full in the beginning
	return breath_bar;
}

/* --- Access to certain layouts --- */

// Gets the health bar layout, for editing it
public func GetHealthBar()
{
	return gui_cmc_crew.Health_Bar;
}


// Gets the health bar layout, for editing it
public func GetBreathBar()
{
	return gui_cmc_crew.Breath_Bar;
}


/* --- Drawing / display / update --- */

/*
	Schedules an update of the bar for the next frame.
	
	@par bar The name of the bar if you want to update only of the bars. Pass 'nil' to update all bars.
 */
public func ScheduleUpdateCrewBars(string bar)
{

	var timer = GetEffect("ScheduledCrewBarsUpdateTimer", this) ?? CreateEffect(ScheduledCrewBarsUpdateTimer, 1, 1);
	if (timer)
	{
		timer.update = timer.update ?? {};
		if (bar)
		{
			timer.update[bar] = true;
		}
		else
		{
			timer.update[GetHealthBar()->GetName()] = true;
			timer.update[GetBreathBar()->GetName()] = true;
		}
	}
}


// Update timer
local ScheduledCrewBarsUpdateTimer = new Effect
{
	Timer = func ()
	{
		var update_health = this.update[Target->GetHealthBar()->GetName()];
		var update_breath = this.update[Target->GetBreathBar()->GetName()];
		Target->UpdateCrewBars(update_health, update_breath);
		return FX_Execute_Kill;
	},
};


// Update the bars
func UpdateCrewBars(bool update_health, bool update_breath)
{
	var cursor = GetCursor(GetOwner());
	
	if (gui_cmc_crew.Menu->ShowForCrew(cursor))
	{
		if (update_health)
		{
			// Default tag for health bar
			var tag = "Health_Full";
			
			// Update the values
			var health_max = cursor->~GetMaxEnergy();
			if (health_max)
			{
				var health = cursor->GetEnergy();
				
				// Set values for the update, this does not yet apply the changes to the menu
				GetHealthBar()->Show();
				GetHealthBar()->SetValue(1000 * health / health_max);
				
				// Change tag for incomplete health bar
				if (health < health_max) tag = "Health_Warn";
			}
			else
			{
				GetHealthBar()->SetValue(1000);
				GetHealthBar()->Hide();
			}
			// Apply changes
			GetHealthBar()->Update();
			
			// Change tag for health bar; has to be updated after changing the progress values to take proper effect
			GuiUpdateTag(tag, GetHealthBar()->GetRootID());
		}
		
		if (update_breath)
		{
			var breath_max = cursor->~GetMaxBreath();
			var breath = cursor->GetBreath();
			
			// In case of breath_max == 0 this will also hide the bar :)
			if (breath < breath_max)
			{
				// Set values for the update, this does not yet apply the changes to the menu
				GetBreathBar()->Show();
				GetBreathBar()->SetValue(1000 * breath / breath_max);
			}
			else
			{
				GetBreathBar()->SetValue(1000);
				GetBreathBar()->Hide();
			}
			// Apply changes
			GetBreathBar()->Update();
		}
	}
}
