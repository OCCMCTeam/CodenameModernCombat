/**
	Screen color

	Colors the screen, should be located behind other HUD elements.

	@author Marky
 */

/* --- Properties --- */

// Proplist for saving the menu layouts, GUI ID and so on.
local gui_cmc_color_overlay;


/* --- Creation / Destruction --- */

func Construction()
{
	gui_cmc_color_overlay = gui_cmc_color_overlay ?? {};
	gui_cmc_color_overlay.Menu = new GUI_Element
	{
		Style = GUI_Multiple | GUI_NoCrop | GUI_IgnoreMouse,
		Priority = GUI_CMC_Priority_ScreenColor,
	};
	gui_cmc_color_overlay.Menu->SetWidth(1000)
	                          ->SetHeight(1000)
	                          ->Open(GetOwner());

	return _inherited(...);
}


func Destruction()
{
	gui_cmc_color_overlay.Menu->Close();

	_inherited(...);
}


/* --- Callbacks from the HUD adapter --- */

public func OnCrewRecruitment(object clonk, int player)
{
	ScheduleUpdateColorOverlay();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeRecruitment(object clonk, int player)
{
	ScheduleUpdateColorOverlay();

	return _inherited(clonk, player, ...);
}


public func OnCrewDeath(object clonk, int killer)
{
	ScheduleUpdateColorOverlay();

	return _inherited(clonk, killer, ...);
}


public func OnCrewDestruction(object clonk)
{
	ScheduleUpdateColorOverlay();

	return _inherited(clonk, ...);
}


public func OnCrewDisabled(object clonk)
{
	ScheduleUpdateColorOverlay();

	return _inherited(clonk, ...);
}


public func OnCrewEnabled(object clonk)
{
	ScheduleUpdateColorOverlay();

	return _inherited(clonk, ...);
}


public func OnCrewSelection(object clonk, bool unselect)
{
	ScheduleUpdateColorOverlay();

	return _inherited(clonk, unselect, ...);
}


public func OnCrewRelaunchStart(object clonk)
{
	ScheduleUpdateColorOverlay();

	return _inherited(clonk, ...);
}


public func OnCrewRelaunchFinish(object clonk)
{
	ScheduleUpdateColorOverlay();

	return _inherited(clonk, ...);
}


/* --- Drawing / display --- */


/*
	Schedules an update of the bar for the next frame.
	
	@par bar The name of the bar if you want to update only of the bars. Pass 'nil' to update all bars.
 */
public func ScheduleUpdateColorOverlay()
{
	GetEffect("ScheduledColorOverlayUpdateTimer", this) ?? CreateEffect(ScheduledColorOverlayUpdateTimer, 1, 1);
}


// Update timer
local ScheduledColorOverlayUpdateTimer = new Effect
{
	Timer = func ()
	{
		Target->UpdateColorOverlay();
		return FX_Execute_Kill;
	},
};


func UpdateColorOverlay()
{
	var cursor = GetCursor(GetOwner());
	
	if (gui_cmc_color_overlay.Menu->ShowForCrew(cursor, cursor->~IsRespawning()))
	{
		for (var identifier in GetProperties(gui_cmc_color_overlay))
		{
			if ("Menu" == identifier)
			{
				continue;
			}
			
			for (var for_object in GetProperties(gui_cmc_color_overlay[identifier]))
			{
				var overlay = gui_cmc_color_overlay[identifier][for_object];
				var target = overlay.Target;
				if (target == cursor)
				{
					overlay->Show();
				}
				else
				{
					overlay->Hide();
				}

				overlay->Update();
			}
		}
	}
}


/* --- Functionality --- */


// Adds the layer if it is not there
func GetColorLayer(object target, string identifier)
{
	if (!gui_cmc_color_overlay[identifier])
	{
		gui_cmc_color_overlay[identifier] = {};
	}
	var for_object = Format("object%d", target->ObjectNumber());
	if (!gui_cmc_color_overlay[identifier][for_object])
	{
		gui_cmc_color_overlay[identifier][for_object] = new GUI_Element
		{ 
			Target = target,
			Style = GUI_Multiple | GUI_NoCrop | GUI_IgnoreMouse,
		};
		gui_cmc_color_overlay[identifier][for_object]->SetWidth(1000)->SetHeight(1000);
		gui_cmc_color_overlay[identifier][for_object]->AddTo(gui_cmc_color_overlay.Menu)->Show();
	}
	
	return gui_cmc_color_overlay[identifier][for_object];
}
