/**
	Medical Bag
*/

#include Library_AmmoManager
#include Library_CMC_Pack
#include Library_ListSelectionMenu

/* --- Engine callbacks --- */

public func Initialize(...)
{
	SetGraphics(nil, DynamiteBox);
	this.PictureTransformation = Trans_Scale(); // Scale the picture of the box mesh, so that it does not appear in the game
	SetGraphics(nil, CMC_Tool_Medical_Bag, 1, GFXOV_MODE_Picture);
	return _inherited(...);
}

func Hit()
{
	Sound("Items::Tools::MedicalBag::Hit?", {multiple = true});
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

public func MaxAmmo() { return 150; }

public func GetAmmoRefillInterval() { return 30; }

public func AllowAmmoRefill(object user)
{
	return HasMedicalAbility(user);
}

/* --- User Interface --- */

func ControlUse(object user, int x, int y)
{
	// User has to be ready to act
	if (user->~HasActionProcedure())
	{
		return UnpackSyringe(user);
	}
	return true;
}


func ControlUseItemConfig(object user, int x, int y, int status)
{
	if (status == CONS_Down)
	{
		return ControlUse(user, x, y);
	}
	return true;
}

public func UnpackSyringe(object user)
{
	AssertNotNil(user);

	// Has to be a medic
	if (!HasMedicalAbility(user))
	{
	    user->PlayerMessage(user->GetOwner(), "$AbilityRequired$", CMC_Ability_ImproveMedicalEquipment);
	    return false;
	}
	
	// Not enough points?
	var points_required = 40;
	if (GetAmmoCount() < points_required)
	{
	    user->PlayerMessage(user->GetOwner(), "$PointsRequired$", points_required);
		return;
	}
	
	// Create the syringe
	var syringe = CreateObject(CMC_Tool_Syringe, 0, 0, NO_OWNER);
	
	// Collect it
	if (user->Collect(syringe))
	{
		DoAmmoCount(-points_required);
		
		user->ShiftContents(false, syringe->GetID()); // TOOD: would be cool to shift right to that item, no???
		user->SetComDir(COMD_Stop);
		
		syringe->SetOwner(user->GetOwner());
		syringe->EvaluateObjectLimit(user->GetOwner());
	}
	else
	{
	    user->PlayerMessage(user->GetOwner(), "$NoSpace$");
		syringe->RemoveObject();
	}
}


func HasMedicalAbility(object user)
{
	return user 
	    && user->~GetCrewClass()                                                  // User has the function for getting the class
	    && user->GetCrewClass()->HasAbility(CMC_Ability_ImproveMedicalEquipment); // Class has the required ability
}


/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
