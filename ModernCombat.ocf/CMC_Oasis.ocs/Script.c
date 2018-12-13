#include Library_CMC_Scenario


func InitializeAmbience()
{
	// TODO: SetGamma(RGB(7, 6, 0), RGB(152, 147, 128), RGB(255, 254, 236));
	_inherited(...);
}

func CreateInterior()
{
	_inherited(...);

	// Ramps
	DrawMaterialQuad("Earth-earth_root", 2360, 678, 2400, 678, 2360, 680, 2400, 680, DMQ_Bridge);
	DrawMaterialQuad("Earth-earth_root", 2490, 678, 2510, 678, 2490, 680, 2510, 680, DMQ_Bridge);
	DrawMaterialQuad("Granite-granite",  2680, 678, 2750, 678, 2680, 680, 2750, 680, DMQ_Bridge);
	DrawMaterialQuad("Rock-rock",        2890, 678, 3010, 678, 2890, 680, 3010, 680, DMQ_Bridge);
	DrawMaterialQuad("Rock-rock",        3820, 678, 3940, 678, 3820, 680, 3940, 680, DMQ_Bridge);
	DrawMaterialQuad("Granite-granite",  4080, 678, 4150, 678, 4080, 680, 4150, 680, DMQ_Bridge);
	DrawMaterialQuad("Earth-earth_root", 4320, 678, 4340, 678, 4320, 680, 4340, 680, DMQ_Bridge);
	DrawMaterialQuad("Earth-earth_root", 4430, 678, 4470, 678, 4430, 680, 4470, 680, DMQ_Bridge);
}

