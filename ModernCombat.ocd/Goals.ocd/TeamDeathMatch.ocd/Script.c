#include Arena_Goal_TeamDeathMatch
#include Plugin_Goal_TimeLimit


func OnRoundStart(int round)
{
	inherited(...);
	
	var clock = CreateObject(GUI_Clock);
	SetClock(clock);
}
