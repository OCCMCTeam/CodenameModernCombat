/**
	Add some deployment locations for testing.
 */

#appendto Library_CMC_Scenario


public func AddDeployLocationTeam1()
{
	var base_team1 = CreateObject(CMC_DeployLocation);
	
	base_team1->SetName("$DeployBaseTeam1$");
	base_team1->SetTeam(1)
	          ->AddRelaunchLocation(420, 400)
	          ->AddRelaunchLocation(420, 630)
	          ->AddRelaunchLocation(450, 340);
}

public func AddDeployLocationTeam2()
{
	var base_team2 = CreateObject(CMC_DeployLocation);
	
	base_team2->SetName("$DeployBaseTeam2$");
	base_team2->SetTeam(2)
	          ->AddRelaunchLocation(1465, 285)
	          ->AddRelaunchLocation(1495, 320)
	          ->AddRelaunchLocation(1530, 550);
}

public func AddDeployLocationFreeForAllHTF()
{
	var base_htf_ffa = CreateObject(CMC_DeployLocation);
	
	base_htf_ffa->SetName("$DeployFreeForAll$");
	base_htf_ffa->AddRelaunchLocation(540, 420)
	            ->AddRelaunchLocation(540, 540)
	            ->AddRelaunchLocation(540, 630)
	            ->AddRelaunchLocation(1410, 340)
	            ->AddRelaunchLocation(1410, 460)
	            ->AddRelaunchLocation(1410, 550);

	var location = CMC_Game_Session_Configurator->ContainCrewAt();
	base_htf_ffa->SetPosition(location->GetX(), location->GetY());
}

public func AddDeployLocationFreeForAll()
{
	var deployment = CreateObject(CMC_DeployLocation);

	deployment->SetName("$DeployFreeForAll$");
	deployment->AddRelaunchLocation(540, 540)
	          ->AddRelaunchLocation(550, 630)
	          ->AddRelaunchLocation(730, 610)
	          ->AddRelaunchLocation(1220, 530)
	          ->AddRelaunchLocation(1420, 460)
	          ->AddRelaunchLocation(1400, 550);
	          
	var location = CMC_Game_Session_Configurator->ContainCrewAt();
	deployment->SetPosition(location->GetX(), location->GetY());
}
