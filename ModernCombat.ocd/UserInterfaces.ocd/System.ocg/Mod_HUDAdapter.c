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