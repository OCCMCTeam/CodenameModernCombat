/**
	Firearms have infinite ammo in the test scenario
 */
 
#appendto CMC_Firearm_Basic

public func GetAmmoSource(ammo)
{
	return AMMO_Source_Infinite;
}
