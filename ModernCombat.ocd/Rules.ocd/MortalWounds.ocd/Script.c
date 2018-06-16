/**
	Prevent instant death.
	
	@author Marky
*/

#include Library_Singleton

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Visibility = VIS_Editor;
local EditorPlacementLimit = 1; // Rules are to be placed only once

/* --- Engine Callbacks --- */

// Message window if rule is selected in the player menu
func Activate(int for_player)
{
	MessageWindow(this.Description, for_player);
	return true;
}

/* --- Functionality --- */

// TODO :D
