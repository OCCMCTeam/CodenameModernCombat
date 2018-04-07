/*
 * Shows a GUI for a crew member, if the crew is enabled.
 * Hides it otherwise
 *
 * @par crew The crew member. The GUI is hidden if this object is nil.
 * @par gui A proplist containing the following properties:
 *           - Menu: a GUI definition (you usually open it with GUIOpen)
 *           - ID: the GUI ID (return value of GUIOpen) 
 */
global func GuiShowForCrew(proplist gui, int player, object crew)
{
	AssertObjectContext();

	if (crew && crew->GetCrewEnabled())
	{
		GuiShowMenu(gui, player);
		return true;
	}
	else
	{
		GuiHideMenu(gui);

		return false;
	}
}


/*
 * Makes a gui visible to a certain player.
 *
 * @par gui A proplist containing the following properties:
 *           - Menu: a GUI definition (you usually open it with GUIOpen)
 *           - ID: the GUI ID (return value of GUIOpen) 
 */
global func GuiShowMenu(proplist gui, int player)
{
	AssertObjectContext();

	player = player ?? GetOwner();
	
	// Make sure inventory is visible
	if (gui.Menu.Player != player)
	{
		gui.Menu.Player = player;
		GuiUpdate(gui.Menu, gui.ID);
	}
}


/*
 * Hides a gui from all players.
 *
 * @par menu A proplist containing the following properties:
 *           - Menu: a GUI definition (you usually open it with GUIOpen)
 *           - ID: the GUI ID (return value of GUIOpen) 
 */
global func GuiHideMenu(proplist gui)
{
	AssertObjectContext();

	// Make sure inventory is invisible
	if (gui.Menu.Player != NO_OWNER)
	{
		gui.Menu.Player = NO_OWNER;
		GuiUpdate(gui.Menu, gui.ID);
	}
}
