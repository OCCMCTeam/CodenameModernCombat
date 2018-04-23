#appendto GUI_Controller_InventoryBar


func AssembleInventoryButton(int max_slots, int slot_number, proplist slot_info)
{
	// Assemble it as usually
	var button = inherited(max_slots, slot_number, slot_info);
	
	// Now modify it
	
	// Do not use graphics for the menu, because we use a background color instead
	button.Symbol = nil;
	
	// Define the background color
	button.BackgroundColor = 
	{
		Std = GUI_CMC_Background_Color_Default,
		Selected = GUI_CMC_Background_Color_Highlight,
		Quick = GUI_CMC_Background_Color_Default,
	};
	
	// And done!
	return button;
}

// Calculates the position of a specific button and returns a proplist.
func CalculateButtonPosition(int slot_number, int max_slots)
{
	return inherited(slot_number, max_slots, ...);
}