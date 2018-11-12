/**
	Booby Trap Bag
*/

#include Library_AmmoManager
#include Library_CMC_Pack
#include Library_ListSelectionMenu
#include Library_ObjectLimit

/* --- Engine callbacks --- */

public func Initialize(...)
{
	SetGraphics(nil, DynamiteBox);
	this.PictureTransformation = Trans_Scale(); // Scale the picture of the box mesh, so that it does not appear in the game
	SetGraphics(nil, CMC_Tool_BoobyTrap_Bag, 1, GFXOV_MODE_Picture);
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

public func MaxAmmo() { return 3; }
public func InitialAmmo() { return 1; }

/* --- User Interface --- */

public func HoldingEnabled() { return true; }

func ControlUseStart(object user, int x, int y)
{
	return RedirectToBoobyTrap("ControlUseStart", user, x, y, true);
}

func RejectUse(object user)
{
	return RedirectToBoobyTrap("RejectUse", user);
}


func ControlUseHolding(object user, int x, int y)
{
	return RedirectToBoobyTrap("ControlUseHolding", user, x, y);
}

func ControlUseStop(object user, int x, int y)
{
	return RedirectToBoobyTrap("ControlUseStop", user, x, y);
}

func ControlUseCancel(object user, int x, int y)
{
	return RedirectToBoobyTrap("ControlUseCancel", user, x, y);
}

func RedirectToBoobyTrap(string call_name, object user, int x, int y, bool create)
{
	var trap = GetBoobyTrap(create);
	if (trap)
	{
		return trap->Call(call_name, user, x, y);
	}
	return false;
}


func GetBoobyTrap(bool create)
{
	// Get existing one?
	var trap = FindContents(CMC_Tool_BoobyTrap);
	if (trap)
	{
		return trap;
	}
	else if (!create)
	{
		return false;
	}
	
	// Create one?
	if (GetAmmoCount() > 0)
	{
		trap = CreateObject(CMC_Tool_BoobyTrap, 0, 0, NO_OWNER); // Initialize without owner to prevent limitation count
		trap->Enter(this);
		return trap;
	}
	else
	{
		RemoveObject();
		return false;
	}
}


func PlacedBoobyTrap(object booby_trap)
{
	// Deduct ammo
	DoAmmoCount(-1);
	
	// Remove if used up
	if (GetAmmoCount() <= 0)
	{
		RemoveObject();
	}
}

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ObjectLimitPlayer = 1;
