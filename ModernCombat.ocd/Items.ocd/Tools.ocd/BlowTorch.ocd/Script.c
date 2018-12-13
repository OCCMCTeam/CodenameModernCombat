/**
	Blow Torch
*/

#include Library_AmmoManager
#include Library_CMC_Pack
#include Library_ListSelectionMenu

/* --- Engine callbacks --- */

public func Initialize(...)
{
	SetGraphics(nil, WindBag);
	this.PictureTransformation = Trans_Scale(); // Scale the picture of the box mesh, so that it does not appear in the game
	SetGraphics(nil, CMC_Tool_BlowTorch, 1, GFXOV_MODE_Picture);
	return _inherited(...);
}

/* --- Display --- */

public func GetCarryMode(object user, bool not_selected)
{
	return CARRY_Blunderbuss;
}


public func GetCarryTransform(object user, bool not_selected, bool nohand, bool second_on_back)
{
	if (not_selected)
	{
		if (!second_on_back)
		{
			return Trans_Mul(Trans_Rotate(180, 1), Trans_Translate(0,-3000));
		}
		else
		{
			return Trans_Mul(Trans_Rotate(180, 1), Trans_Translate(3000,-3000), Trans_Rotate(-30, 0, 1));
		}
	}
	if (nohand)
	{
		return Trans_Mul(Trans_Rotate(180, 1), Trans_Translate(0,3000));
	}

	return Trans_Mul(Trans_Rotate(220, 0, 1, 0), Trans_Rotate(30, 0, 0, 1), Trans_Rotate(-26, 1, 0, 0));

}


public func GetCarryPhase()
{
	return 600;
}

/* --- Functionality --- */

public func MaxAmmo() { return 100; }

public func GetAmmoRefillInterval() { return 35; }

public func AllowAmmoRefill(object user)
{
	return false; // TODO
}

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
