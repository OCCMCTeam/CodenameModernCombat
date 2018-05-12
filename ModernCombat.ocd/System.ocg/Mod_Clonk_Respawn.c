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

		// Callback that allows for filling the tabs
		this->~OnOpenRespawnMenu(menu);
	}
}
