/*
	Shooter Library Interface for CMC

	Basic functionality for all CMC firearms
*/

#include Library_Firearm // Basic functionality
//#include Library_AmmoManager // Provide functions for managing ammo
//#include Plugin_Firearm_AmmoLogic // For having an ammo logic in general
//#include Plugin_Firearm_ReloadFromAmmoSource // For ammo transfer between weapon and Clonk
//#include Plugin_Firearm_ReloadProgressBar // Temporary include, because the weapon needs to reload and there is no proper HUD yet

/* --- Constants --- */

// Transition from carrying to ironsight
static const WEAPON_FM_IronsightAnim =  0; // With an animtion
static const WEAPON_FM_IronsightBlend = 1; // Animation blending
static const WEAPON_FM_IronsightInst  = 2; // Instant ironsight

/* --- Variables --- */

// Right mouse buttons is pressed down
local ironsight = false;
// Transition from carrying to ironsight is done, ready to fire
local is_in_ironsight = false;

/* --- Right click controls --- */

// Called by the shooter library in ControlUseAltStart
func OnPressUseAlt(object clonk, int x, int y)
{
	// Check if the clonk can currently perform an action with hands
	if (!clonk->HasHandAction(true, nil, true))
		return false;

	// Great. Go into ironsight aiming
	StartIronSight(clonk, x, y);

	return true;
}

// Called by the shooter library in ControlUseAltHolding
func OnHoldingUseAlt(object clonk, int x, int y)
{
	// Shouldn't happen
	if (!ironsight)
		return false;

	// Still transitioning into ironsight: wait
	if (!is_in_ironsight)
		return true;

	// Aim!
	var angle = Angle(0, 0, x, y + GetFiremode()->GetYOffset());
	angle = Normalize(angle, -180);
	clonk->SetAimPosition(angle);
}

// Called by the shooter library in ControlUseAltStop
func OnUseAltStop(object clonk, int x, int y)
{
	if (!ironsight)
		return false;

	if (is_in_ironsight)
		clonk->StopAim();
	else
	{
		var effect = GetEffect("IronsightHelper", this);
		if (effect)
		{
			effect->Cancel();
			RemoveEffect(nil, nil, effect, true);
		}
	}

	ironsight = false;
	is_in_ironsight = false;
	return true;
}

/* --- Ironsight aiming --- */

public func StartIronSight(object clonk, int x, int y)
{
	if (ironsight)
		return;

	var trans_type = GetFiremode()->GetIronsightType();

	ironsight = true;
	is_in_ironsight = false;

	// Instant transition into ironsight
	if (trans_type == WEAPON_FM_IronsightInst)
	{
		FinishIronsight(clonk, x, y);
	} else if (trans_type == WEAPON_FM_IronsightAnim)
	{
		var anim = GetFiremode()->GetIronsightAnimation();
		var delay = GetFiremode()->GetIronsightDelay();
		// Play animation for set amount of time
		var number = clonk->PlayAnimation(anim, CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, clonk->GetAnimationLength(anim), delay, ANIM_Hold), Anim_Const(1000));

		this->CreateEffect(IronsightHelper, 1, delay, clonk, x, y, number);
	} else if (trans_type == WEAPON_FM_IronsightBlend)
	{
		var delay = GetFiremode()->GetIronsightDelay();
		var current_anim = clonk->GetRootAnimation(CLONK_ANIM_SLOT_Arms);
		var anim = GetFiremode()->GetAimingAnimation();
		var delay = GetFiremode()->GetIronsightDelay();
		var length = clonk->GetAnimationLength(anim);
		var y_offset = GetFiremode()->GetYOffset();
		var angle = Abs(Normalize(clonk->Angle(0,0, x, y + y_offset), -180)) * 10;
		// Just to be sure, end all arms animations
		if (current_anim != nil)
			clonk->StopAnimation(current_anim);
		// Blend current animation (most likely a movement animation) into aiming animation
		var number = clonk->PlayAnimation(anim, CLONK_ANIM_SLOT_Movement, Anim_Const(angle * length / 1800), Anim_Linear(0, 0, 999, delay, ANIM_Remove));

		this->CreateEffect(IronsightHelper, 1, delay, clonk, x, y, number);
	}
}

// Something happened, so that aiming is now not possible. Clean up variables
public func FailedIronsight()
{
	ironsight = false;
	is_in_ironsight = false;
}

// Transition into ironsight successful, start aiming
public func FinishIronsight(object clonk, int x, int y)
{
	clonk->StartAim(this);

	var angle = Angle(0, 0, x, y + GetFiremode()->GetYOffset());
	angle = Normalize(angle, -180);
	clonk->SetAimPosition(angle);

	is_in_ironsight = true;
}

local IronsightHelper = new Effect {
	Construction = func(object clonk, int x, int y, int anim)
	{
		this.clonk = clonk;
		this.x = x;
		this.y = y;
		this.anim = anim;
	},
	Timer = func()
	{
		// Ironsight failed because of unknown things
		if (!this.clonk->IsWalking() && !this.clonk->IsJumping())
		{
			this.Target->FailedIronsight();
			return FX_Execute_Kill;
		}
		this.clonk->ResetAnimationEffects();
		this.Target->FinishIronsight(this.clonk, this.x, this.y);
		return FX_Execute_Kill;
	},
	Cancel = func()
	{
		this.clonk->StopAnimation(this.anim);
	}
};

/* --- Ammo handling --- */

public func GetAmmoSource(ammo)
{
	return AMMO_Source_Local; // Has an internal ammo counter
}

public func GetAmmoReloadContainer()
{
	return Contained(); // Reload from this container
}

public func OnNoAmmo(object user, proplist firemode)
{
	_inherited(user, firemode);
	
	Sound("Weapon::Shared::LastRound?", {player = user->GetOwner()});
}

/* --- Reloading --- */

public func NeedsReload(object user, proplist firemode)
{
	return !HasAmmo(firemode);
}

/* --- Cooldown --- */

public func NeedsRecovery(object user, proplist firemode)
{
	return !NeedsReload(user, firemode); // No recovery necessary when reloading, so that reload can happen instantly
}

/* --- Misc --- */

public func GetAnimationSet()
{
	var ret = _inherited();

	var aim_animation = GetFiremode()->GetAimingAnimation();
	if (aim_animation != nil)
		ret.AnimationAim = aim_animation;
	return ret;
}