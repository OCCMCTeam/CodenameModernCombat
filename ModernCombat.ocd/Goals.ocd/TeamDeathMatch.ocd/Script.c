#include Arena_Goal_TeamDeathMatch
#include Plugin_Goal_TimeLimit
#include Plugin_Goal_EliminateLosingPlayers


func OnRoundStart(int round)
{
	SetTimeLimit(0, 30);
	inherited(...);
	
	
	var clock = CreateObject(GUI_Clock);
	SetClock(clock);
}
