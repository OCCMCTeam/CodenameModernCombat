/**
	Make clonks use the damage system.
*/

#appendto Clonk

// Add screen fading effect and sound system
func Construction(object creator)
{
	_inherited(creator, ...);
	
	CreateEffect(FxCmcDamageSystem, 1);
}


func CatchBlow(int health_change, object from)
{
	var damage_system = GetEffect(FxCmcDamageSystem.Name, this);
	if (damage_system)
	{
		damage_system->~CatchBlow(health_change, from, ...);
	}
	return _inherited(health_change, from, ...);
}
