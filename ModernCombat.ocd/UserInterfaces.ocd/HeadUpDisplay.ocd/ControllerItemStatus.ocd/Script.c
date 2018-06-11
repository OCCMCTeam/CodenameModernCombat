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
	gui_cmc_item_status = {};
	gui_cmc_item_status.Menu = AssembleItemStatus();
	gui_cmc_item_status.Menu->Open(GetOwner());

	return _inherited(...);
}


func Destruction()
{
	gui_cmc_item_status.Menu->Close();

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


public func OnCrewRelaunchStart(object clonk)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk, ...);
}


public func OnCrewRelaunchFinish(object clonk)
{
	ScheduleUpdateItemStatus();

	return _inherited(clonk, ...);
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

	var field_large_digit_margin = 160;
	var field_small_digit_margin = 80;
	
	var menu = new GUI_Element
	{
		Target = this,
		Player = NO_OWNER, // will be shown once a gui update occurs
		Style = GUI_Multiple | GUI_NoCrop | GUI_IgnoreMouse,
		Priority = GUI_CMC_Priority_HUD,
		
		BackgroundColor = GUI_CMC_Background_Color_Default,
	};
	
	menu->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Info_Width + GUI_CMC_Element_Icon_Size))
	    ->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Info_Height))
	    ->AlignRight(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_H));
	
	// Depends on include order, unfortunately
	// Also, you have to shift down from the top in this case :/
	if (this.gui_cmc_crew)
	{
		menu->AlignBottom(this.gui_cmc_crew.Menu->GetTop());
	}
	else
	{
		menu->AlignBottom(1000);
	}
	menu->ShiftTop(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_V));

	var field = new GUI_Element {};
	field->SetWidth(separator_button_row_h)
	     ->SetHeight(2 * line_height)
	     ->AddTo(menu);
	     
	// Get button assignments
	
	var button_grenade = GetPlayerControlAssignment(GetOwner(), CON_CMC_DrawGrenade, true, true);
	var button_reload = GetPlayerControlAssignment(GetOwner(), CON_CMC_Reload, true, true);
	
	// Rightmost element: A vertical button row
	var vertical_button_row = new GUI_Element
	{
		button_1 = 
		{
			Top = ToPercentString(0 * line_height + field_small_digit_margin),
			Bottom = ToPercentString(1 * line_height - field_small_digit_margin),
			Symbol = CMC_Icon_Button,
			GraphicsName = button_reload,
		},
		
		button_2 = 
		{
			Top = ToPercentString(1 * line_height + field_small_digit_margin),
			Bottom = ToPercentString(2 * line_height - field_small_digit_margin),
			Symbol = CMC_Icon_Button,
			GraphicsName = button_grenade,
		},
		
		button_3 = 
		{
			Top = ToPercentString(2 * line_height + field_small_digit_margin),
			Bottom = ToPercentString(3 * line_height - field_small_digit_margin),
			Symbol = CMC_Icon_Button,
			GraphicsName = "",
		},
	};
	vertical_button_row->SetLeft(separator_button_row_h)
	                   ->SetRight(1000)
	                   ->AddTo(menu);
	                   
	// Leftmost bottom element: Detail text of one line height
	var object_configuration = new GUI_Element
	{
		Style = GUI_TextHCenter | GUI_TextVCenter,
	};
	object_configuration->SetWidth(separator_button_row_h)
	                    ->SetHeight(line_height)
	                    ->AlignTop(2 * line_height)
	                    ->AddTo(menu);

	// Large digits for object count
	var object_count = new GUI_Counter {};
	object_count->SetMaxDigits(3)
	            ->SetDigitProperties({Symbol = CMC_Icon_Number})
	            ->SetDigitWidth(field_large_digit_width)
	            ->SetTop(field_large_digit_margin)
	            ->SetBottom(1000 - field_large_digit_margin)
	            ->ShowTrailingZeros(true)
	            ->AddTo(field);

	// Separator
	var slash = new GUI_Element { Symbol = CMC_Icon_Number, GraphicsName = "Dash", };
	slash->SetWidth(field_small_digit_width)
	     ->SetTop(field_large_digit_margin)
	     ->SetBottom(500)
	     ->AlignLeft(object_count->GetRight())
	     ->AddTo(field);

	// Small digits for total count
	var total_count = new GUI_Counter{};
	total_count->SetMaxDigits(3)
	           ->SetDigitProperties({Symbol = CMC_Icon_Number})
	           ->SetDigitWidth(field_small_digit_width)
	           ->SetTop(field_large_digit_margin)
	           ->SetBottom(500)
	           ->AlignLeft(slash->GetRight())
	           ->ShowTrailingZeros(true)
	           ->AddTo(field);

	// Small digits for grenade count
	var grenade_count = new GUI_Counter {};
	grenade_count->SetMaxDigits(1)
	             ->SetDigitProperties({Symbol = CMC_Icon_Number})
	             ->SetDigitWidth(field_small_digit_width)
	             ->SetTop(500 + field_large_digit_margin)
	             ->SetBottom(1000)
	             ->AlignLeft(slash->GetRight())
	             ->AddTo(field);

	// Grenade icon
	var grenade_icon = new GUI_Element { Symbol = IronBomb };
	grenade_icon->SetTop(500 + field_large_digit_margin)
	            ->SetBottom(1000)
	            ->AlignLeft(grenade_count->GetRight())
	            ->SetWidth(field_small_digit_width)
	            ->AddTo(field);

	// Add access to the main property
	gui_cmc_item_status.Object_Configuration = object_configuration;
	gui_cmc_item_status.Object_Count = object_count;
	gui_cmc_item_status.Slash = slash;
	gui_cmc_item_status.Total_Count = total_count;
	gui_cmc_item_status.Grenade_Count = grenade_count;
	gui_cmc_item_status.Grenade_Icon = grenade_icon;
	
	return menu;
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
public func GetObjectConfiguration()
{
	return gui_cmc_item_status.Object_Configuration;
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
	
	if (gui_cmc_item_status.Menu->ShowForCrew(cursor, cursor->~IsRespawning()))
	{
		// --- Grenades
		
		var grenade_type = cursor->~GetCurrentGrenadeType();
		if (grenade_type)
		{
			GetGrenadeCount()->Show()->SetValue(cursor->GetGrenadeCount(grenade_type));
			gui_cmc_item_status.Grenade_Icon->Show();
		}
		else
		{
			GetGrenadeCount()->Hide();
			gui_cmc_item_status.Grenade_Icon->Hide();
		}
		
		// --- Item status
		var item = cursor->GetHandItem(0);

		var status;
		// Get status from the object if possible
		if (item)
		{
			status = item->~GetGuiItemStatusProperties(cursor);
		}
		// Fallback to default values
		if (!status)
		{
			status = new GUI_Item_Status_Properties{};
		}
		
		// Show object counts only if there is a value
		var object_count = status->GetObjectCount();
		if (object_count == nil)
		{
			GetObjectCount()->Hide()->SetValue(0);
		}
		else
		{
			GetObjectCount()->Show()->SetValue(object_count);
		}
		
		// Show total count only if there is a value
		var total_count = status->GetTotalCount();
		if (total_count == nil)
		{
			GetTotalCount()->Hide()->SetValue(0);
		}
		else
		{
			GetTotalCount()->Show()->SetValue(total_count);
		}
		
		// Show slash only if there is object count AND total count
		if (object_count == nil || total_count == nil)
		{
			GetSlash()->Hide();
		}
		else
		{
			GetSlash()->Show();
		}
		
		// Text info in the bottom row, item name as a default
		var object_configuration = status->GetObjectConfiguration();
		if (item && object_configuration == nil)
		{
			object_configuration = item->GetName();
		}
		GetObjectConfiguration().Text = object_configuration;
		if (object_configuration)
		{
			GetObjectConfiguration()->Show();
		}
		else
		{
			GetObjectConfiguration()->Hide();
		}

		// --- Actually update everything
		GetObjectCount()->Update();
		GetTotalCount()->Update();
		GetSlash()->Update();
		GetGrenadeCount()->Update();
		GetObjectConfiguration()->Update();
		gui_cmc_item_status.Grenade_Icon->Update();
	}
}

/* --- Misc --- */

static const GUI_Item_Status_Properties = new Global
{
	// Internal properties

	object_configuration = nil, // Description of the object configuration; nil means: Name of the object is displayed
	object_count = nil,         // Amount to be display in object count; nil means: not displayed
	total_count = nil,          // Amount to be display in total count; nil means: not displayed

	// Getters
	
	GetObjectConfiguration = func ()
	{
		return this.object_configuration;
	},

	GetObjectCount = func ()
	{
		return this.object_count;
	},

	GetTotalCount = func ()
	{
		return this.total_count;
	},

	// Setters
	
	SetObjectConfiguration = func (string text)
	{
		this.object_configuration = text;
		return this;
	},

	SetObjectCount = func (int value)
	{
		this.object_count = value;
		return this;
	},

	SetTotalCount = func (int value)
	{
		this.total_count = value;
		return this;
	},
};
