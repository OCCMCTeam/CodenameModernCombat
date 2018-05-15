
#appendto Arena_RelaunchContainer

static const CMC_RELAUNCH_TIME = 15;

/* --- Properties --- */

local respawn_menu;

/* --- Engine callbacks --- */

func Initialize()
{
	inherited(...);

	// Relaunch time is 15 seconds
	SetRelaunchTime(CMC_RELAUNCH_TIME, true, 6);
}

/* --- Functions --- */

public func GetRespawnMenu()
{
	return respawn_menu;
}

public func SetRespawnMenu(proplist menu)
{
	if (respawn_menu)
	{
		FatalError("Already has a respawn menu!");
	}
	respawn_menu = menu;
}

/* --- Callbacks --- */

/**
	Callback when the crew is contained.
	This happens after the crew has entered the container.
	
	@par crew This object is being initialized.
 */
public func OnInitializeCrew(object crew)
{
	// Remove previous contents
	for (var contents = crew->Contents(); contents != nil; contents = crew->Contents())
	{
		contents->RemoveObject();
	}
	
	// Open the respawn menu
	crew->~OpenRespawnMenu();

	// Notify the HUD
	crew->~OnCrewRelaunchStart();
	
	// Remove light
	crew->SetLightRange(0);
}


/**
	Callback when the contained crew is relaunched.
	This happens after the crew has left the container and
	before the container is removed. 

	@par crew This object is being relaunched.
 */
public func OnRelaunchCrew(object crew)
{
	var location = nil;
	var player = crew->GetOwner();

	// Reset zoom to defaults
	SetPlayerZoomDefault(player);
	
	
	// Close the menu
	if (GetRespawnMenu())
	{
		var deploy = GetRespawnMenu()->SelectedDeployLocation();
		if (deploy)
		{
			location = deploy->RecommendRelaunchLocation(player);
		}
		
		GetRespawnMenu()->Close();
	}
	
	// Update location
	if (location)
	{
		crew->SetPosition(location->GetX(), location->GetY());
		SetPlrView(player, crew);
	}
	
	// Notify the HUD
	crew->~OnCrewRelaunchFinish();
	
	// Restore light
	crew->SetLightRange(300);
}

/* --- Overloaded functions --- */

/**
	Find out whether a relaunch is currently blocked.
	
	@return bool Returns {@code true} if the relaunch is blocked.
	             By default, this is if the crew has a menu,
	             or if a callback "RejectRelaunch" in the crew
	             returns {@code true}. 
 */
public func IsRelaunchBlocked()
{
	if (GetRespawnMenu())
	{
		return GetRespawnMenu()->IsRespawnBlocked();
	}
	else
	{
		return inherited(...);
	}
}


/**
	Callback from the relaunch timer.
	
	By default this displays the remaining time as a message above the container.

	@par frames This many frames are remaining.
 */
public func OnTimeRemaining(int frames)
{
	if (GetRespawnMenu())
	{
		GetRespawnMenu()->GetRespawnButton()->OnTimeRemaining(frames);
	}
}

