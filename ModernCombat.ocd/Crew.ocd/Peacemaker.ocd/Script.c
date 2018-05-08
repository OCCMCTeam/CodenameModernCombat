/*--- The Peacemaker ---*/

#include Clonk
#include Library_AmmoManager
#include CMC_Library_HasClass

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
	
	//SetGraphics(skin_name = nil, Clonk);
	gender = 0;

	// Refreshes animation (otherwise the Clonk will be stuck in the previus animation)
	// Go back to original action afterwards and hope
	// that noone calls SetSkin during more complex activities
	var prev_action = GetAction();
	SetAction("Jump");
	SetAction(prev_action);

	return skin;
}

/*--- Better death animation ---*/

func StartDead()
{
	// Blend death animation with other animations, except for the death slot
	var merged_animations = false;	
	for (var slot = 0; slot < CLONK_ANIM_SLOT_Death; ++slot)
	{
		if (GetRootAnimation(slot) == nil) continue;
		OverlayDeathAnimation(slot);
		merged_animations = true;
	}

	// Force the death animation if there were no other animations active
	if (!merged_animations)
	{
		OverlayDeathAnimation(CLONK_ANIM_SLOT_Death);
	}

	// Update carried items
	UpdateAttach();
	// Set proper turn type
	SetTurnType(1);
}

// Merges the animation in an animation slot with the death animation
// More variation is possible if we considered adding a random value for the length,
// or if we set the parameters according to current speed, etc.
func OverlayDeathAnimation(int slot)
{
	var animation = "Dead";
	PlayAnimation(animation, slot, Anim_Linear(0, 0, GetAnimationLength(animation), 20, ANIM_Hold), Anim_Linear(0, 0, 1000, 10, ANIM_Remove));
}

/*--- Properties ---*/

local Name = "$Name$";
local Description = "$Description$";
local MaxEnergy = 140000;
