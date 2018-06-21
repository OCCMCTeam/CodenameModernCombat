/**
	Refillable use item
	
	When including this, be sure to call the inherited function in:
	- Initialize()
	
	Additionally has to:
	- include the Library_AmmoManager
	- implement the function MaxAmmo()
*/

public func Initialize()
{
	SetAmmoRefillInterval(this->~GetAmmoRefillInterval());
	DoAmmoCount(this->MaxAmmo());
	return _inherited(...);
}


public func SetAmmoRefillInterval(int interval)
{
	interval = interval ?? 35;
	var refill = GetEffect("RefillAmmoEffect", this) ?? CreateEffect(RefillAmmoEffect, 1, interval);
	if (refill)
	{
		refill.Interval = interval;
		return true;
	}
	return false;
}


// Refill!
local RefillAmmoEffect = new Effect
{
	Timer = func ()
	{
		// Already filled?
		if (Target->GetAmmoCount() >= Target->MaxAmmo())
		{
			return FX_OK;
		}
		
		// Do it!
		var user = Target->Contained();
		var can_refill = Target->~AllowAmmoRefill(user);
		if (user && can_refill)
		{
			Target->DoAmmoCount(1);
		}
		return FX_OK;
	},
};


/* --- Internal interface --- */

func GetAmmoCount()
{
	return this->GetAmmo(GetID());
}

func DoAmmoCount(int change)
{
	var difference = this->DoAmmo(GetID(), change);
	if (difference && Contained())
	{
		Contained()->~OnInventoryChange(); // Notify HUD
	}
	return difference;
}

func GetAmmoSource(id ammo)
{
	return AMMO_Source_Local;
}

/* --- HUD information --- */

// Callback from the item status HUD element: What should be displayed?
func GetGuiItemStatusProperties(object user)
{
	var status = new GUI_Item_Status_Properties {};

	// Object count
	status->SetObjectCount(GetAmmoCount());
	
	// Total count
	if (user == Contained())
	{
		status->SetTotalCount(this->MaxAmmo());
	}

	return status;
}
