/* --- The Peacemaker --- */

#include Clonk
#include Library_AmmoManager
#include CMC_Library_HasClass
#include CMC_Library_GrenadeBelt
#include CMC_Library_AffectedBySmokeGrenade
#include CMC_Library_AffectedByStunGrenade
#include Plugin_Firearm_DynamicSpread

/* --- Engine callbacks --- */

func Recruitment(int player)
{
	if (GetPlayerType(player) == C4PT_User)
	{
		CMC_Virtual_Cursor->AddTo(this)->SetCursorType(CMC_Cursor_Default);
	}
	return _inherited(player, ...);
}

/* --- Inventory management --- */

public func GetCurrentItem()
{
	return this->GetHandItem(0);
}

/* --- Ammo management --- */

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

/* --- No Backpack --- */

private func AttachBackpack()
{
	// Overridden to do nothing
}


private func RemoveBackpack()
{
	// Overridden to do nothing
}

/* --- Graphics --- */

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

/* --- Sounds --- */

func PlaySoundMovementStepHard()
{
	Sound("Clonk::Movement::StepHard?", {multiple = true, volume = 25});
}

func PlaySoundMovementStepSoft()
{
	Sound("Clonk::Movement::StepSoft?", {multiple = true, volume = 25});
}

func PlaySoundMovementStepWater()
{
	Sound("Clonk::Movement::StepWater?", {multiple = true, volume = 25});
}

func PlaySoundMovementRustle()
{
	Sound("Clonk::Movement::Rustle?", {multiple = true});
}

func PlaySoundMovementRustleLand()
{
	Sound("Clonk::Movement::Land?", {multiple = true, volume = 25});
}

func Footstep()
{
    if(GBackLiquid(0, 6))
    {
    	PlaySoundMovementStepWater();
    }
    else
    {
    	inherited(...);
    }
}

func DoRoll(bool is_falling)
{
	// Additional sound from falling
	if (is_falling)
	{
		PlaySoundMovementRustleLand();
	}
	// Everything as usual
	inherited(is_falling, ...);
}

/* --- Firearm spread management --- */

// Opening angle is half the cone here
static const CLONK_SPREAD_Max = 25000;
static const CLONK_SPREAD_Jumping_Min = 10000;
static const CLONK_SPREAD_Running_Min =  5000;
static const CLONK_SPREAD_Walking_Min =  2500;
static const CLONK_SPREAD_Standing_Min =  1000;

static const CLONK_SPREAD_Reduction_Base = -150;
static const CLONK_SPREAD_Reduction_Aiming = -200;
static const CLONK_SPREAD_Reduction_Crawling = -200;

func UpdateFirearmSpread()
{
	// Reduce constantly on its own
	var spread_reduction = 0;
	if (this->IsAiming() && this->GetAimType() == WEAPON_AIM_TYPE_IRONSIGHT)
	{
		spread_reduction = CLONK_SPREAD_Reduction_Aiming;
	}
	else if (IsWalking())
	{
		spread_reduction = CLONK_SPREAD_Reduction_Base;
	}
	
	// Certain actions set a minimum spread
	var action_spread_min = 0;
	if (IsJumping() || GetEffect("Rolling", this))
	{
		action_spread_min = CLONK_SPREAD_Jumping_Min;
	}
	else if (IsWalking())
	{
		var animation = GetCurrentWalkAnimation();
		if (animation == Clonk_WalkRun)
		{
			action_spread_min = CLONK_SPREAD_Running_Min;
		}
		else if (animation == Clonk_WalkWalk)
		{
			action_spread_min = CLONK_SPREAD_Walking_Min;
		}
		else if (animation == Clonk_WalkStand)
		{
			action_spread_min = CLONK_SPREAD_Standing_Min;
		}
	}
	
	// Get weapon data
	var firemode, cursor_type, spread_limit;
	var weapon = this->GetHandItem(0);
	if (weapon)
	{
		firemode = weapon->~GetFiremode();
	}
	if (firemode)
	{
		cursor_type = firemode->~GetAimCursor();
		spread_limit = firemode->~GetSpreadLimit();
	}
	
	// Apply the values
	SetFirearmSpreadLimit(spread_limit ?? CLONK_SPREAD_Max);
	DoFirearmSpread(spread_reduction);
	RaiseFirearmSpread(action_spread_min);
	
	// Crosshair
	var cursor = CMC_Virtual_Cursor->Get(this);
	if (cursor)
	{
		if (this->IsAiming() && cursor_type && GetCursor(GetOwner()) == this)
		{
			cursor->SetCursorType(cursor_type);
			cursor->Show();
			cursor->UpdateAimSpread(weapon->ComposeSpread(this, firemode));
		}
		else
		{
			cursor->Hide();
		}
	}
}


/* --- Better death animation --- */

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

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local MaxEnergy = 140000;
