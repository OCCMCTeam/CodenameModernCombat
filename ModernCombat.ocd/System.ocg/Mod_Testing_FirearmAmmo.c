/*
	Temporary file for testing
 */
 
#appendto CMC_Firearm_Basic

// As long as there is no proper HUD just give me a message!
public func OnAmmoChange(id ammo_type)
{
	_inherited(ammo_type);
	if (Contained() && this->~GetAmmoSource(ammo_type) != AMMO_Source_Infinite)
	{
		Contained()->PlayerMessage(Contained()->GetOwner(), "{{%i}}: %d", ammo_type, GetAmmo(ammo_type));
	}
}

// Update me
public func OnFinishReload(object user, int x, int y, proplist firemode)
{
	_inherited(user, x, y, firemode, ...);
	OnAmmoChange(firemode->GetAmmoID());
}
