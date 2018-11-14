/**
	Flag
 */

/* --- Functions --- */

func Initialize()
{
	SetOwner(NO_OWNER);
	SetAction("Fly");
	AddTimer(this.Wind, 10);
}


func Wind()
{
	if (Random(6))
	{
		// TODO
		//Sound("FlagWave?.ogg", 0, 0, RandomX(50,75));
	}

	// Change with the wind
	if (GetAction() == "Fly")
	{
		SetDir(BoundBy(GetWind() / 15 + 3, 0, 6));
	}
}

/* --- Properties ---*/

local Name = "$Name$";
local Description = "$Description$";

local ActMap = {
Fly = {
	Prototype = Action,
	Name = "Fly",
	Procedure = DFA_FLOAT,
	Length = 26,
	Delay = 3,
	X = 18, Y = 70,
	Wdt = 55, Hgt = 31,
	OffX = -27,
	FacetBase = 1,
	NextAction = "Fly",
},
};
