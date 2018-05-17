/**
	The inventory should issue a callback if the item number is pressed, even in the same slot.

	@author Marky
 */

#appendto Library_HUDAdapter

// Callback from Mod_Inventory.c
func OnSlotUnchanged(int slot)
{
	if (HUDcontroller)
		HUDcontroller->~OnSlotObjectUnchanged(slot);
	return _inherited(slot, ...);
}

// Callback from class system
func OnSetCrewClass(id class)
{
	if (HUDcontroller)
		HUDcontroller->~OnSetCrewClass(this, class);
	return _inherited(class, ...);
}

// Callback from relaunch container
func OnCrewRelaunchStart()
{
	if (HUDcontroller)
		HUDcontroller->~OnCrewRelaunchStart(this);
	return _inherited(...);
}

// Callback from relaunch container
func OnCrewRelaunchFinish()
{
	if (HUDcontroller)
		HUDcontroller->~OnCrewRelaunchFinish(this);
	return _inherited(...);
}

/* --- Overloads --- */

// Either returns the current HUD controller or creates one.
// But only if owner is a human otherwise returns nil.
func GetHUDController()
{
	// Get the existing HUD, if possible
	var player = GetOwner();
	var controller = inherited(...);
	if (controller)
	{
		return controller;
	}
	// Bots do not usually get a HUD, but here we want a simple one that
	// can notify the other HUDs of health changes
	else if (GetPlayerType(player) == C4PT_Script)
	{
		var controllerDef = CMC_GUI_ControllerBot;
		HUDcontroller = FindObject(Find_ID(controllerDef), Find_Owner(player));
		if (!HUDcontroller)
			HUDcontroller = CreateObject(controllerDef, AbsX(0), AbsY(0), player);
		return HUDcontroller;
	}
	return nil;
}
