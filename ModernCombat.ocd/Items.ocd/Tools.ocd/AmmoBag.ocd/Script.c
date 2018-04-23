/**
	Ammo Bag
*/

#include Library_Stackable
#include Library_CMC_Pack

/* --- Engine callbacks --- */

public func Initialize(...)
{
	SetGraphics(nil, DynamiteBox);
	this.PictureTransformation = Trans_Scale(); // Scale the picture of the box mesh, so that it does not appear in the game
	SetGraphics(nil, CMC_Ammo_Bag, 1, GFXOV_MODE_Picture);
	return _inherited(...);
}

/* --- Display --- */

public func GetCarryMode(object user, bool not_selected)
{
	var can_hold = user->~IsWalking() || user->~IsJumping();
	if (can_hold && !not_selected)
	{
		return CARRY_BothHands;
	}
	else
	{
		return CARRY_Back;
	}
}


public func GetCarryTransform(object user, bool not_selected, bool nohand, bool second_on_back)
{
	if (not_selected)
	{
		if (second_on_back)
		{
			return Trans_Mul(Trans_Translate(-5000, 3000, 0), Trans_Rotate(-45, 0, 1));
		}
		else
		{
			return Trans_Mul(Trans_Translate(0, 3000, 00), Trans_Rotate(-45, 0, 1));
		}
	}
	if (nohand)
	{
		return Trans_Mul(Trans_Translate(0, -3000, -2200), Trans_Rotate(-45, 0, 1));
	}
}


public func GetCarryPhase()
{
	return 450;
}

/* --- Functionality --- */

// At the moment, this is a stack, because it seemed to be more in line 
// with the how things are in the predecessor;
// Can be switched to work like ammo later, if desired
public func MaxStackCount() { return 200; }

public func GetStackRefillInterval() { return 20; }

public func AllowStackRefill(object user)
{
	return true; // TODO: Allowed only for support class
}

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
