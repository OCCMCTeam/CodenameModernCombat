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
