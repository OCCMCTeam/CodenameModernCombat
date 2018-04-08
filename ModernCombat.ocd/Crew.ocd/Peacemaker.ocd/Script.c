/*--- The Peacemaker ---*/

#include Clonk
#include Library_AmmoManager

/*--- Inventory management ---*/

public func GetCurrentItem()
{
	return this->GetHandItem(0);
}

/*--- Ammo management ---*/

public func GetAmmoSource(id ammo)
{
	return AMMO_Source_Local;
}

public func SetAmmo(id ammo, int new_value)
{
	var info = _inherited(ammo, new_value);
	var hud = this->~GetHUDController();
	if (hud)
	{
		hud->~OnAmmoChange(this);
	}
	return info;
}

/*--- No Backpack ---*/

private func AttachBackpack()
{
	// Overridden to do nothing
}


private func RemoveBackpack()
{
	// Overridden to do nothing
}

/*--- Graphics ---*/

private func SetSkin(int new_skin)
{
	// Overridden to do less
	
	// Remember skin:
	// skin 0 => Adventurer
	// skin 1 => Steampunk
	// skin 2 => Alchemist
	// skin 3 => Farmer
	skin = new_skin;
	
	// However, we do not use any of these :(
	skin_name = nil;
	
	SetGraphics(skin_name = nil, Clonk);
	gender = 0;

	// Refreshes animation (otherwise the Clonk will be stuck in the previus animation)
	// Go back to original action afterwards and hope
	// that noone calls SetSkin during more complex activities
	var prev_action = GetAction();
	SetAction("Jump");
	SetAction(prev_action);

	return skin;
}

/*--- Properties ---*/

local Name = "$Name$";
local Description = "$Description$";
local MaxEnergy = 140000;
