/*
	Mod_Testing.c

	Temporary file for testing
 */
 
#appendto Peacemaker

/* When adding to the crew of a player */

func Recruitment(int player)
{
	DoAmmo(CMC_Ammo_Bullets, 100);
	CreateContents(CMC_Weapon_Pistol);
	
	return _inherited(player,...);
}
