#appendto CMC_Goal_HoldTheFlag

local additional_objects = [];

public func DoSetup(int round)
{
	Scenario->AddDeployLocationTeam1();
	Scenario->AddDeployLocationTeam2();

	// Flag post
	var flag = CreateObject(CMC_FlagPost, 3415, 480, NO_OWNER);
    flag->~SetName("$Flag3$");
    SetFlag(flag);

	// Borders
	//CreateObject(BRDR, 2490, 0, -1)->Set(0,1,0,1,1);
	//CreateObject(BRDR, 4340, 0, -1)->Set(1,1,0,1,2);

	// Sentry guns
	/*
	var selfdef = CreateObject(SEGU, 2035, 610, -1);
	selfdef->Set(0,1,1,180,0,2500);
	selfdef->SetTeam(1);
	selfdef = CreateObject(SEGU, 4795, 610, -1);
	selfdef->Set(0,1,1,180,4330);
	selfdef->SetTeam(2);
    */   

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
