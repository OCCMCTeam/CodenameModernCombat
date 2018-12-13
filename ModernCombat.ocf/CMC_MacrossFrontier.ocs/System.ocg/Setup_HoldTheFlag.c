#appendto CMC_Goal_HoldTheFlag

local additional_objects = [];

public func DoSetup(int round)
{
	if (GetLength(GetActiveTeams()) == 2)
	{
		Scenario->AddDeployLocationTeam1();
		Scenario->AddDeployLocationTeam2();
	}
	else
	{
		Scenario->AddDeployLocationFreeForAllHTF();
	}


    // Flag Post
    var flag = CreateObject(CMC_FlagPost, 995, 560, NO_OWNER);
    flag->~SetName("$DeployFlag$");
    SetFlag(flag);

    // TODO: Remove objects
    // RemoveAll(JPTP);
    // RemoveAll(JMPD);

    // Additional ammo
    // TODO: Comply with "no ammo" rule 
    // if(!FindObject(NOAM))
    // {
		// Create template spawn points with default configuration
		var config = Scenario->GetSpawnPointTemplates();

		// Place spawn points
		var ammo_crate = CopySpawnPoint(config.ammo_bullets, 960, 635);
		ammo_crate->OnRoundStart();
		PushBack(additional_objects, ammo_crate);

		// Remove template spawn points
		Scenario->RemoveSpawnPointTemplates(config);
    //}


	_inherited(round);
}

public func DoCleanup(int round)
{
	for (var additional in additional_objects)
	{
		if (additional)
		{
			additional->RemoveObject();
		}
	}
	inherited(round, ...);
}
