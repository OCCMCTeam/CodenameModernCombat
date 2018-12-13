#include CMC_GUI_Controller_AllyInfo
#include Library_HUDController

func Construction()
{
	inherited(...);

	// No need for a menu, we just need to provide the callbacks to player allies
	if (gui_cmc_ally_info.Menu)
	{
		gui_cmc_ally_info.Menu->Close();
	}
}

func UpdateAllyInfo()
{
	// Do nothing
}
