/**
	Add some deployment locations for testing.
 */

#appendto CMC_Game_Session_Configurator

public func Initialize()
{
	inherited(...);
	
	var base_team1 = CreateObject(CMC_DeployLocation);
	var base_team2 = CreateObject(CMC_DeployLocation);
	var base_htf_ffa = CreateObject(CMC_DeployLocation);
	var deployment = CreateObject(CMC_DeployLocation);

	base_team1->SetName("Base 1");
	base_team2->SetName("Base 2");
	base_htf_ffa->SetName("Base FFA");
	deployment->SetName("Deployment");
	
	// Contains all locations for now
	
	base_team1->SetTeam(1)
	          ->AddRelaunchLocation(420, 400)
	          ->AddRelaunchLocation(420, 630)
	          ->AddRelaunchLocation(450, 340);

	base_team2->SetTeam(2)
	          ->AddRelaunchLocation(1465, 285)
	          ->AddRelaunchLocation(1495, 320)
	          ->AddRelaunchLocation(1530, 550);

	base_htf_ffa->AddRelaunchLocation(540, 420)
	            ->AddRelaunchLocation(540, 540)
	            ->AddRelaunchLocation(540, 630)
	            ->AddRelaunchLocation(1410, 340)
	            ->AddRelaunchLocation(1410, 460)
	            ->AddRelaunchLocation(1410, 550);

	deployment->AddRelaunchLocation(540, 540)
	          ->AddRelaunchLocation(550, 630)
	          ->AddRelaunchLocation(730, 610)
	          ->AddRelaunchLocation(1220, 530)
	          ->AddRelaunchLocation(1420, 460)
	          ->AddRelaunchLocation(1400, 550);
}
