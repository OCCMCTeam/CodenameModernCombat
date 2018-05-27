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
	clonk->CreateContents(Pistol);
}