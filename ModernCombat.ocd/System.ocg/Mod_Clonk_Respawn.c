#appendto Clonk

/* --- Respawn stuff --- */

// Find out whether the crew is currently respawning
public func IsRespawning()
{
	return Contained() && Contained()->GetID() == Arena_RelaunchContainer;
}

// Opens the respawn menu
public func OpenRespawnMenu()
{
	if (IsRespawning())
	{
		// Open menu
		var menu = new CMC_GUI_RespawnMenu {};
		menu->Assemble(this)
		    ->Show()
		    ->Open(GetOwner());
		SetMenu(menu->GetRootID());
		
		// Save menu in the container for future reference
		Contained()->SetRespawnMenu(menu);

		// Callback that allows for filling the tabs
		this->~OnOpenRespawnMenu(menu);
	}
}

/* --- Callbacks from respawn system --- */

public func OnOpenRespawnMenu(proplist menu)
{
	_inherited(menu, ...);

	CMC_DeployLocationManager->ScanDeployLocations();

	// Add selection tabs
	for (var location in CMC_DeployLocationManager->GetDeployLocations(GetOwner()))
	{
		menu->GetDeployLocations()->AddTab(location);
	}

	// Select the first location by default
	menu->GetDeployLocations()->SelectTab(nil, 0, true);
}
