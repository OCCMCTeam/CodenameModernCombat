#appendto Library_CMC_Scenario


public func AddDeployLocationTeam1()
{
	var base_team1 = CreateObject(CMC_DeployLocation);
	
	base_team1->SetName("$Team1$");
	base_team1->SetTeam(1)
	          ->AddRelaunchLocation(1440, 450)
	          ->AddRelaunchLocation(1440, 540)
	          ->AddRelaunchLocation(1560, 570);
}

public func AddDeployLocationTeam2()
{
	var base_team2 = CreateObject(CMC_DeployLocation);
	
	base_team2->SetName("$Team2$");
	base_team2->SetTeam(2)
	          ->AddRelaunchLocation(5390, 450)
	          ->AddRelaunchLocation(5390, 540)
	          ->AddRelaunchLocation(5270, 570);
}
