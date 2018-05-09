
#appendto Arena_RelaunchContainer

/* --- Engine callbacks --- */


func Initialize()
{
	inherited(...);

	// Relaunch time is 15 seconds
	SetRelaunchTime(15);
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

	crew->~StartClassSelection();
}


/**
	Callback when the contained crew is relaunched.
	This happens after the crew has left the container and
	before the container is removed. 

	@par crew This object is being relaunched.
 */
public func OnRelaunchCrew(object crew)
{
	// Nothing at the moment
}
