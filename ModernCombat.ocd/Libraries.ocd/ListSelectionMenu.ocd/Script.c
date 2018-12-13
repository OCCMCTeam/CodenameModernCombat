/**
	Provider for a custom list selection menu

	Must implement:

		GetListSelectionMenuEntries(object user, string type, proplist menu)

	where:
		user = the object calling the menu,
		type = a string that serves identification of the menus, if there are several menu types in an object.
		       However, only one object can be open at the same time!
		menu = the menu proplist.

	This function can be used for filling the menu with contents and setting menu appearance.

	@author Marky
*/

/* --- Constants --- */

// All menu types, for reference
static const CMC_LIST_MENU_TYPE_FIREMODE_SELECTION = "FiremodeSelection";
static const CMC_LIST_MENU_TYPE_GRENADE_SELECTION = "GrenadeSelection";

// Delays for opening the menus, in frames
static const CMC_LIST_MENU_DELAY_SHORT = 5;

/* --- Properties --- */

local cmc_list_selection_menu;

/* --- User Interface --- */


func ControlMenu(object user, int control, int x, int y, int strength, bool repeat, int status)
{
	if (status == CONS_Down)
	{
		if (control == CON_GUIClick1 || control == CON_GUIClick2)
		{
			CloseListSelectionMenu(true);
		}
		else if (control == CON_InventoryShiftForward)
		{
			ScrollListSelectionMenu(false);
		}
		else if (control == CON_InventoryShiftBackward)
		{
			ScrollListSelectionMenu(true);
		}
		else if (control == CON_Hotkey1
              || control == CON_Hotkey2
              || control == CON_Hotkey3
              || control == CON_Hotkey4
              || control == CON_Hotkey5
              || control == CON_Hotkey6
              || control == CON_Hotkey7
              || control == CON_Hotkey8
              || control == CON_Hotkey9
              || control == CON_Hotkey0)
        {
        	PressListSelectionMenuHotkey(control);
        }
	}
}

public func GetListSelectionMenu()
{
	if (cmc_list_selection_menu)
	{
		return cmc_list_selection_menu.menu;
	}
}

/** 
	Opens the menu.

	@par user This object uses the menu.
	@par type This is and identifier that is used for handling multiple types of menus in the same object.
	          Only one menu can be active at the same time, and the identifier is not saved in the menu,
	          but the callback {@code GetListSelectionMenuEntries(object user, string type, proplist menu)}
	          uses the identifier.
 */
public func OpenListSelectionMenu(object user, string type)
{
	// Close existing menu, no callback
	CloseListSelectionMenu();

	// If another menu is already open cancel the action.
	if (user->~GetMenu())
	{
		return;
	}

	// Define menu

	var main_menu = new CMC_GUI_SelectionListMenu {};
	main_menu->Assemble()
	         ->AlignCenterH()
	         ->Open(user->GetOwner());

	cmc_list_selection_menu = {};
	cmc_list_selection_menu.user = user;
	cmc_list_selection_menu.menu = main_menu;
	cmc_list_selection_menu.user->~SetMenu(main_menu->GetRootID(), false, this);
	cmc_list_selection_menu.hotkey_controls = GUI_CON_Hotkey_ByIndex();
	cmc_list_selection_menu.hotkey_entries =  [];

	// Add entries and update appearance

	this->GetListSelectionMenuEntries(user, type, main_menu);

	main_menu->AdjustHeightToEntries()
	         ->AlignCenterV()
	         ->ShiftTop(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size)->Scale(5)->Shrink(2)); // Shift upwards by 2.5 items

	var delay = this->~DelayListSelectionMenu(user, type);
	if (delay > 0)
	{
		main_menu->Hide();
		ScheduleCall(this, this.ShowListSelectionMenu, delay);
	}
	main_menu->Update();
	main_menu->GetList()->SelectEntry();
}

public func CloseListSelectionMenu(bool do_callback) // Do callback only if everything is OK. Is more readable and logically when coding conditions for this use case. 
{
	if (cmc_list_selection_menu)
	{
		var selected_entry = cmc_list_selection_menu.menu->GetList()->GetSelectedEntry();
		if (selected_entry && do_callback)
		{
			selected_entry->~OnMenuClosedCall();
		}
		cmc_list_selection_menu.menu->Close();
		if (cmc_list_selection_menu.user) cmc_list_selection_menu.user->MenuClosed();
		ClearScheduleCall(this, this.ShowListSelectionMenu);
	}
	cmc_list_selection_menu = nil;
}

/**
	Shows a hidden menu, if it is open.
 */
public func ShowListSelectionMenu()
{
	if (cmc_list_selection_menu)
	{
		cmc_list_selection_menu.menu->Show()->Update();
	}
}

public func ScrollListSelectionMenu(bool backward)
{
	if (cmc_list_selection_menu)
	{
		cmc_list_selection_menu.menu->GetList()->SelectNextEntry(backward);

		if (cmc_list_selection_menu.menu.Settings.ClickAfterScroll)
		{
			cmc_list_selection_menu.menu->GetList()->GetSelectedEntry()->~OnClickCall();
		}
	}
}

public func PressListSelectionMenuHotkey(int control)
{
	if (cmc_list_selection_menu)
	{
		var index = GetIndexOf(cmc_list_selection_menu.hotkey_controls, control);
		var entry_index = cmc_list_selection_menu.hotkey_entries[index];
		if (index == -1)
		{
			FatalError("This should be impossible!");
		}
		if (nil != entry_index)
		{
			cmc_list_selection_menu.menu->GetList()->SelectEntry(nil, entry_index);

			if (cmc_list_selection_menu.menu.Settings.ClickAfterHotkey)
			{
				cmc_list_selection_menu.menu->GetList()->GetSelectedEntry()->~OnClickCall();
			}
		}
	}
}

public func GetListSelectionMenuHotkey(int index)
{
	if (cmc_list_selection_menu)
	{
		return GetPlayerControlAssignment(cmc_list_selection_menu.user->GetOwner(), cmc_list_selection_menu.hotkey_controls[index], true, true);
	}
	return nil;
}

public func SetListSelectionMenuHotkey(proplist entry, int index)
{
	if (cmc_list_selection_menu)
	{
		AssertNotNil(entry);
		entry->SetButtonHint(GetListSelectionMenuHotkey(index))->Update();
		cmc_list_selection_menu.hotkey_entries[index] = entry->GetIndex();
	}
}
