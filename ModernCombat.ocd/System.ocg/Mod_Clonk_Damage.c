/**
	Make clonks use the damage system.
*/

#appendto Clonk

func Construction(object creator)
{
	_inherited(creator, ...);
	
	CreateEffect(FxCmcDamageSystem, 1);
}
