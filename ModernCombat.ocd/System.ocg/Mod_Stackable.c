/**
	Stackable objects should be displayed with object count and total count in the HUD.
 */

#appendto Library_Stackable

// Callback from the item status HUD element: What should be displayed?
public func GetGuiItemStatusProperties(object user)
{
	var status = new GUI_Item_Status_Properties {};
	
	return status->SetObjectCount(GetStackCount())
		         ->SetTotalCount(MaxStackCount());
}
