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

/* --- Properties --- */

local cmc_list_selection_menu;

/* --- User Interface --- */


func ControlMenu(object user, int control, int x, int y, int strength, bool repeat, int status)
{
	if (status == CONS_Down)
	{
		if (control == CON_GUIClick1 || control == CON_GUIClick2)
		{
			CloseListSelectionMenu();
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


// Opens the 
public func OpenListSelectionMenu(object user, string type)
{
	// Close existing menu
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
	cmc_list_selection_menu.hotkey_controls = [CON_Hotkey1, CON_Hotkey2, CON_Hotkey3, CON_Hotkey4, CON_Hotkey5, CON_Hotkey6, CON_Hotkey7, CON_Hotkey8, CON_Hotkey9, CON_Hotkey0];
	cmc_list_selection_menu.hotkey_entries =  [];
	
	// Add entries and update appearance
	
	this->GetListSelectionMenuEntries(user, type, main_menu);
	
	main_menu->AdjustHeightToEntries()
	         ->AlignCenterV()
	         ->ShiftTop(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size)->Scale(5)->Shrink(2)) // Shift upwards by 2.5 items
		     ->Update();
	main_menu->GetList()->SelectEntry();
}

public func CloseListSelectionMenu()
{
	if (cmc_list_selection_menu)
	{
		cmc_list_selection_menu.menu->Close();
		if (cmc_list_selection_menu.user) cmc_list_selection_menu.user->MenuClosed();
	}
	cmc_list_selection_menu = nil;
}

public func ScrollListSelectionMenu(bool backward)
{
	if (cmc_list_selection_menu)
	{
		cmc_list_selection_menu.menu->GetList()->SelectNextEntry(backward);
	}
}

public func PressListSelectionMenuHotkey(int control)
{
	if (cmc_list_selection_menu)
	{
		var index = GetIndexOf(cmc_list_selection_menu.hotkey_controls, control);
		if (index == -1)
		{
			FatalError("This should be impossible!");
		}
		if (nil != cmc_list_selection_menu.hotkey_entries[index])
		{
			cmc_list_selection_menu.menu->GetList()->SelectEntry(nil, index);
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
