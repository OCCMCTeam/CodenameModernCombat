/*
	Shooting Range

	Tutorial for weapons!
*/

func InitializePlayer(int plr)
{
	var clonk = GetCrew(plr, 0);
	if (!clonk) // ???
		return;

	clonk->SetPosition(35, 170);
	clonk->CreateContents(CMC_Weapon_Pistol);
	clonk->CreateContents(CMC_Weapon_AssaultRifle);

	/* Debug script for checking frame rate on auto/burst fire
	StartScriptProfiler();
	ScheduleCall(clonk->Contents(), "ControlUseStart", 40, 10, clonk, 100, -10);
	ScheduleCall(nil, Global.StopScriptProfiler, 650);
	*/
}
