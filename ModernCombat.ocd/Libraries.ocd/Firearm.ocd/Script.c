/*
	Shooter Library Interface for CMC

	Basic functionality for all CMC firearms
*/

#include Library_Firearm // Basic functionality
#include Library_AmmoManager // Provide functions for managing ammo
#include Plugin_Firearm_AmmoLogic // For having an ammo logic in general
#include Plugin_Firearm_ReloadFromAmmoSource // For ammo transfer between weapon and Clonk
#include Plugin_Firearm_ReloadProgressBar // Temporary include, because the weapon needs to reload and there is no proper HUD yet

/* --- Constants --- */

// Different types of aiming
static const WEAPON_AIM_TYPE_IRONSIGHT = "Ironsight";
static const WEAPON_AIM_TYPE_HIPFIRE   = "HipFire";

// Transition from carrying to ironsight
static const WEAPON_FM_IronsightAnim =  0; // With an animtion
static const WEAPON_FM_IronsightBlend = 1; // Animation blending
static const WEAPON_FM_IronsightInst  = 2; // Instant ironsight

// Hip shooting stop delay when releasing the firing button
static const WEAPON_HipFireDelay = 6 * 35;

/* --- Variables --- */

// Currently aiming / shooting from hips
local hipfire = false;
// The hip shooting target position (in global coordinates)
local hipfire_target;
// The firing button is pressed during hip shooting
local hipfire_pressed = false;
// Ironsight is done or initiatied
local ironsight = false;
// Transition from carrying to ironsight is done, ready to fire
local is_in_ironsight = false;

/* --- Left click controls --- */

// Called by the shooter library in ControlUseStart
func OnPressUse(object clonk, int x, int y)
{
	// Check if currently in ironsight aiming and if yes, fire a controlled shot
	if (ironsight)
		DoIronsightFireCycle(clonk, x, y);
	else
		StartHipShooting(clonk, x, y);

	return true;
}

func OnHoldingUse(object clonk, int x, int y)
{
	return true;
}

// Called by the shooter library in ControlUseStop
func OnUseStop(object clonk, int x, int y)
{
	CheckHipShootingStop();

	return true;
}

/* --- Right click controls --- */

// Right click will by default be a toggle control but can be switched to a holding control
public func IsIronsightToggled(int player)
{
	return CMC_Player_Settings->GetConfigurationValue(player, CMC_IRONSIGHT_TOGGLE, true);
}

// Called by the shooter library in ControlUseAltStart
func OnPressUseAlt(object clonk, int x, int y)
{
	if (ironsight)
	{
		// If toggle setting is used, stop ironsight now
		if (IsIronsightToggled(clonk->GetOwner()))
			StopIronsight(clonk);
		return true;
	}

	// Check if the clonk can currently perform an action with hands
	if (!clonk->HasHandAction(true, nil, true))
		return true;

	// Great. Go into ironsight aiming
	StartIronSight(clonk, x, y);

	return true;
}

// Called by the shooter library in ControlUseAltHolding
func OnHoldingUseAlt(object clonk, int x, int y)
{
	// Check if right click is not a toggle
	if (IsIronsightToggled(clonk->GetOwner()))
		return true; // Do nothing but prevent library default

	// Shouldn't happen
	if (!ironsight)
		return true;

	// Still transitioning into ironsight: wait
	if (!is_in_ironsight)
		return true;

	// Aim!
	var angle = Angle(0, 0, x, y + GetFiremode()->GetYOffset());
	angle = Normalize(angle, -180);
	clonk->SetAimPosition(angle);
}

// Called by the CMC modified clonk, see ModernCombat.ocd\System.ocg\Mod_Clonk.c
public func ControlUseAiming(object clonk, int x, int y)
{
	if (!ironsight)
		return true;

	if (!is_in_ironsight)
		return true;

	if (!IsIronsightToggled())
	{
		// In this case, the control should never fire
		SetPlayerControlEnabled(clonk->GetOwner(), CON_CMC_AimingCursor, false);
		return true;
	}

	// Aim!
	var angle = Angle(0, 0, x, y + GetFiremode()->GetYOffset());
	angle = Normalize(angle, -180);
	clonk->SetAimPosition(angle);
}

// Called by the shooter library in ControlUseAltStop
func OnUseAltStop(object clonk, int x, int y)
{
	// Check if right click is not a toggle
	if (IsIronsightToggled(clonk->GetOwner()))
		return true; // Do nothing but prevent library default

	StopIronsight(clonk);
	return true;
}

/* --- Hip firing --- */

func StartHipShooting(object clonk, int x, int y)
{
	if (hipfire)
		return;

	hipfire = true;
	if (hipfire_target)
		hipfire_target = CreateArray(2);
	hipfire_target = [clonk->GetX() + x, clonk->GetY() + y];
	hipfire_pressed = true;

	var angle = Angle(0, 0, x, y + GetFiremode()->GetYOffset());
	angle = Normalize(angle, -180);
	clonk->StartAim(this, angle, WEAPON_AIM_TYPE_HIPFIRE);
	clonk->SetAimPosition(angle);
	clonk->UpdateAttach();

	this->CreateEffect(HipShootingEffect, 1, 1, clonk);
}

func CheckHipShootingStop()
{
	if (!hipfire)
		return;

	// Take notice that the firing button was released
	hipfire_pressed = false;
	// The firing process will do the rest and stop aiming after a delay
}

func StopHipShooting(object clonk)
{
	if (clonk && clonk->IsAiming())
		clonk->StopAim();

	hipfire = false;
	hipfire_pressed = false;
}

local HipShootingEffect = new Effect {
	Construction = func (object clonk) {
		// Time duration in which no firing command was issued
		this.timeout = 0;

		this.clonk = clonk;
	},
	Timer = func() {
		// If no firing button is pressed: schedule stop
		if (!this.Target.hipfire_pressed)
		{
			this.timeout++;
			if (this.timeout >= WEAPON_HipFireDelay)
				return FX_Execute_Kill;
		} else if (this.timeout > 0)
			this.timeout = 0;
		// Clonk cannot aim anymore
		if (!this.clonk || (!this.clonk->IsWalking() && !this.clonk->IsJumping()))
			return FX_Execute_Kill;
		// Keep the clonk on target
		var x_target = this.Target.hipfire_target[0] - this.clonk->GetX();
		var y_target = this.Target.hipfire_target[1] - this.clonk->GetY();
		var angle = Angle(0, 0, x_target, y_target + this.Target->GetFiremode()->GetYOffset());
		angle = Normalize(angle, -180);
		this.clonk->SetAimPosition(angle);
		return FX_OK;
	},
	Destruction = func() {
		if (this.Target)
			this.Target->StopHipShooting(this.clonk);
	}
};

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
		var anim = GetFiremode()->GetIronsightAimingAnimation();
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
	var angle = Angle(0, 0, x, y + GetFiremode()->GetYOffset());
	angle = Normalize(angle, -180);
	clonk->StartAim(this, angle, WEAPON_AIM_TYPE_IRONSIGHT);
	clonk->SetAimPosition(angle);
	clonk->SetCancelOnJump(true);
	ScheduleCall(clonk, "UpdateAttach", 1);

	is_in_ironsight = true;

	// TODO: Test & maybe make depended on weapon shoot mode?
	clonk->SetAimViewOffset(50);
	if (IsIronsightToggled())
	{
		// Mouse move will adjust aim angle
		SetPlayerControlEnabled(clonk->GetOwner(), CON_CMC_AimingCursor, true);
		// Disable OC default
		SetPlayerControlEnabled(clonk->GetOwner(), CON_Aim, false);
	}
}

// Stop ironsight aiming, regardless of the current state (aiming or transitional state)
public func StopIronsight(object clonk)
{
	if (!ironsight)
		return;

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
	// Disable CMC Aiming control if necessary
	if (IsIronsightToggled())
		SetPlayerControlEnabled(clonk->GetOwner(), CON_CMC_AimingCursor, false);
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
		// This effect should end now if it hasn't ended (see Reset())
		if (this.end_if_not_ended)
			return FX_Execute_Kill;
		// Ironsight failed because of unknown things
		if (!this.clonk->IsWalking())
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

/* --- Ironsight Firing --- */

func DoIronsightFireCycle(object clonk, int x, int y)
{
	if (!IsReadyToFire())
		return;

	var angle = GetAngle(x, y);
	clonk->~SetAimPosition(angle);

	// Check if reload is necessary
	if (!StartReload(clonk, x, y))
		// Check if the weapon still needs charging
		if (!StartCharge(clonk, x, y))
			// Fire away
			Fire(clonk, x, y);
}

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

public func OnAmmoChange(id ammo_type)
{
	NotifyContainer();
	_inherited(ammo_type, ...);
}

/* --- Selection --- */

func Selection(object user)
{
	var firemode = GetFiremode();
	if (firemode)
	{
		user->~RaiseFirearmSpread(firemode->~GetSpreadBySelection());
	}
	_inherited(user, ...);
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

/* --- HUD information --- */

// Callback from the item status HUD element: What should be displayed?
public func GetGuiItemStatusProperties(object user)
{
	var status = new GUI_Item_Status_Properties {};

	var firemode = GetFiremode();
	var ammo_type = firemode->GetAmmoID();

	// Object count
	status->SetObjectCount(GetAmmo(ammo_type));
	
	// Total count
	if (user == Contained() && user->~IsAmmoManager())
	{
		status->SetTotalCount(user->GetAmmo(ammo_type));
	}

	// Object configuration
	if (firemode && ammo_type)
	{
			// This should display a colored "fire mode" - "fire technique"
			// In the shooter library the CMC fire technique is named firemode
			// whereas the CMC fire mode is actually the ammo type that the weapon uses
			status->SetObjectConfiguration(Format("<c %x>%s</c> - %s", GUI_CMC_Text_Color_Highlight, ammo_type->GetName(), firemode->GetName()));
	}

	return status;
}

/**
 * Tells a possible container that the firearm was
 * changed.
 * Calls NotifyHUD() in containers with extra slots,
 * or OnInventoryChange() otherwise. 
 */
func NotifyContainer()
{
	// notify hud
	var container = Contained();
	if (container)
	{
		// has an extra slot
		if (container->~HasExtraSlot())
		{
			container->~NotifyHUD();
		}
		// is a clonk with new inventory system
		else
		{
			container->~OnInventoryChange();
		}
	}
}

/* --- Aim Manager Interface --- */

public func GetAnimationSet(object clonk)
{
	if (!clonk)
		return FatalError("CMC Firearm Library: GetAnimationSet was called without a parameter."); // Don't what is wanted

	var ret = _inherited();

	// Different aim sets for different aiming styles
	var type = clonk->GetAimType();

	if (type == WEAPON_AIM_TYPE_HIPFIRE)
	{
		// Aiming animation
		var aim_animation = GetFiremode()->GetHipFireAimingAnimation();
		if (aim_animation != nil)
			ret.AnimationAim = aim_animation;
		// No hindrance on walking speed
		ret.walk_speed_front = nil;
		ret.walk_speed_back  = nil;
	} else if (type == WEAPON_AIM_TYPE_IRONSIGHT)
	{
		var aim_animation = GetFiremode()->GetIronsightAimingAnimation();
		if (aim_animation != nil)
			ret.AnimationAim = aim_animation;
	} else if (type == "Default")
	{
		// Do nothing here but also don't fail!
	} else {
		FatalError("CMC Firearm Library: GetAnimationSet with an unknown aiming mode set.");
	}
	
	return ret;
}

// The clonk stops aiming. Just to be sure, reset all variables (under regular circumstances, this will probably set variables two times)
public func Reset(object clonk)
{
	ironsight = false;
	is_in_ironsight = false;

	hipfire = false;
	hipfire_pressed = false;
	hipfire_target = nil;

	var effect = GetEffect("IronsightHelper", this);
	// This will end the effect on the next Timer call.
	// Because the effect might just be regularly removed by StopIronsight()
	if (effect)
		effect.end_if_not_ended = true;

	effect = GetEffect("HipShootingEffect", this);
	if (effect)
		RemoveEffect(nil, nil, effect);
}

/* --- Status --- */

public func IsAiming()
{
	return is_in_ironsight || hipfire;
}