/**
	The inventory should issue a callback if the item number is pressed, even in the same slot.

	@author Marky
 */

#appendto Library_Inventory

public func SetHandItemPos(int hand, int inv)
{
	var return_value = inherited(hand, inv, ...);

	if (inv == GetHandItemPos(hand))
	{
		this->~OnSlotUnchanged();
	}
	
	return return_value;
}
