#appendto CMC_Goal_TeamDeathMatch

public func DoSetup(int round)
{
	Scenario->AddDeployLocationTeam1();
	Scenario->AddDeployLocationTeam2();
	_inherited(round);
}
