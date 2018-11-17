#appendto CMC_Goal_TeamDeathMatch

public func DoSetup(int round)
{
	if (GetLength(GetActiveTeams()) == 2)
	{
		Scenario->AddDeployLocationTeam1();
		Scenario->AddDeployLocationTeam2();
	}
	else
	{
		Scenario->AddDeployLocationFreeForAll();
	}
	_inherited(round);
}
