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
	if (status == CONS_Down && (control == CON_GUIClick1 || control == CON_GUIClick2))
	{
		CloseListSelectionMenu();
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

	var main_menu = new CMC_GUI_SelectionListMenu {};
	main_menu->Assemble()
	         ->AlignCenterH()
	         ->Open(user->GetOwner());
	
	this->GetListSelectionMenuEntries(user, type, main_menu);
	
	main_menu->AdjustHeightToEntries()
	         ->AlignCenterV()
	         ->ShiftTop(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size)->Scale(5)->Shrink(2)) // Shift upwards by 2.5 items
		     ->Update();

		     
	cmc_list_selection_menu = {};
	cmc_list_selection_menu.user = user;
	cmc_list_selection_menu.menu = main_menu;
	cmc_list_selection_menu.user->~SetMenu(main_menu->GetRootID(), false, this);
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
