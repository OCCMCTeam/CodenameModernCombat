/*
	Shooter Library Interface for CMC

	Basic functionality for all CMC firearms
*/

#include Library_Firearm // Basic functionality
#include Library_AmmoManager // Provide functions for managing ammo
#include Plugin_Firearm_AmmoLogic // For having an ammo logic in general
#include Plugin_Firearm_ReloadFromAmmoSource // For ammo transfer between weapon and Clonk
#include Plugin_Firearm_ReloadProgressBar // Temporary include, because the weapon needs to reload and there is no proper HUD yet
#include Library_ListSelectionMenu // Menu for weapon configuration

/* --- Constants --- */

// Different types of aiming
static const WEAPON_AIM_TYPE_PRONE     = "Prone";
static const WEAPON_AIM_TYPE_IRONSIGHT = "Ironsight";
static const WEAPON_AIM_TYPE_HIPFIRE   = "HipFire";

// Different types of transition into aiming
static const WEAPON_AIM_TRANS_INST = "Instant";   // Aiming is instant
static const WEAPON_AIM_TRANS_ANIM = "Animation"; // Play an animation to transition into aiming
static const WEAPON_AIM_TRANS_BLND = "Blend";     // Blend into aiming with a delay

// Hip shooting stop delay when releasing the firing button
static const WEAPON_HipFireDelay = 6 * 35;

/* --- Variables --- */

// Currently aiming
local is_aiming = false;
// True as long as in a transitional state between not aiming and aiming (is_aiming will be true as well)
local aim_transition = false;
// Saves the aiming type
local current_aim_type;
// True when the current aim stance changes into another (current_aim_type will be the old type)
local change_aiming = false;
// The shooting target position (in global coordinates)
local aim_target;
// The firing button is pressed during hip shooting (special case for this aiming stance)
local hipfire_pressed = false;
// Menu for firemode selection
local cmc_firemode_menu = nil;

/* --- Left click controls --- */

// Called by the shooter library in ControlUseStart
func OnPressUse(object clonk, int x, int y)
{
	// If already aiming, just pass this button press along
	if (IsAiming())
	{
		ContinueAiming(clonk, x, y, true);
		// Call library default firing mechanic
		DoFireCycle(clonk, x, y, false);
	}
	// Otherwise, start hip firing
	else
	{
		StartAiming(clonk, WEAPON_AIM_TYPE_HIPFIRE, x, y);
	}
	return true;
}

func OnHoldingUse(object clonk, int x, int y)
{
	// Autofiring weapons will continue to fire
	if (IsAutoFiring())
	{
		// Call library default firing mechanic
		DoFireCycle(clonk, x, y, false);
		// Adjust cursor
		clonk->~UpdateCmcVirtualCursor(this, x, y);
	}
	return true;
}

// Called by the shooter library in ControlUseStop
func OnUseStop(object clonk, int x, int y)
{
	return true;
}

// Called by the shooter library in ControlUseCancel
// Because both mouse buttons are used in CMC, cancelling manually is not possible
// but this will nonetheless be called by PauseAim() from the Aim Manager
func OnUseCancel(object clonk, int x, int y)
{
	StopAiming(clonk);
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
	if (IsAiming())
	{
		if (GetAimType() == WEAPON_AIM_TYPE_HIPFIRE)
		{
			ChangeAiming(clonk, WEAPON_AIM_TYPE_IRONSIGHT, x, y);
		}
		if (GetAimType() == WEAPON_AIM_TYPE_IRONSIGHT ||
		    GetAimType() == WEAPON_AIM_TYPE_PRONE)
		{
			if (IsIronsightToggled(clonk->GetOwner()))
				StopAiming(clonk);
		}
	}
	else
	{
		if (clonk->IsProne())
		{
			StartAiming(clonk, WEAPON_AIM_TYPE_PRONE, x, y);
		}
		else
		{
			StartAiming(clonk, WEAPON_AIM_TYPE_IRONSIGHT, x, y);
		}
	}

	return true;
}

// Called by the shooter library in ControlUseAltHolding
func OnHoldingUseAlt(object clonk, int x, int y)
{
	// Check if right click is not a toggle
	if (IsIronsightToggled(clonk->GetOwner()))
		return true; // Do nothing but prevent library default

	if (IsAiming())
		ContinueAiming(clonk, x, y, true);

	return true;
}

// Called by the shooter library in ControlUseAltStop
func OnUseAltStop(object clonk, int x, int y)
{
	// Check if right click is not a toggle
	if (IsIronsightToggled(clonk->GetOwner()))
		return true; // Do nothing but prevent library default

	StopAiming(clonk);
	return true;
}

/* --- Aiming --- */

func Setting_AimOnUseStart()
{
	return false;
}

// Called by the CMC modified clonk, see ModernCombat.ocd\System.ocg\Mod_Clonk.c
public func ControlUseAiming(object clonk, int x, int y)
{
	if (!IsAiming())
		return true;

	if (aim_transition)
		return true;

	if (change_aiming)
		return true;
		
	var button_pressed = (GetPlayerControlState(clonk->GetOwner(), CON_Use) > 0);
	ContinueAiming(clonk, x, y, button_pressed);
	clonk->SetAimViewOffset(Distance(x, y));
	return true;
}

// Begin aiming
public func StartAiming(object clonk, string aim_type, int x, int y)
{
	AssertNotNil(clonk, "CMC Firearm Library: StartAiming was called without a valid clonk parameter.");
	AssertNotNil(aim_type, "CMC Firearm Library: StartAiming was called without a valid aim_type parameter.");

	// Do not start a new aiming procedure if already aiming (this is what ChangeAiming is for)
	if (is_aiming)
		return;

	// Check necessary requirements
	if (!Call(Format("Can%sAim", aim_type), clonk))
		return;

	is_aiming = true;
	aim_transition = true;

	// Transition into aiming
	var trans = GetFiremode()->Call(Format("Get%sTransition", aim_type));
	var number;
	if (trans == WEAPON_AIM_TRANS_INST)
	{
		FinishAiming(clonk, aim_type, x, y);
		return;
	}
	else if (trans == WEAPON_AIM_TRANS_ANIM)
	{
		var anim = GetFiremode()->Call(Format("Get%sAnimation", aim_type));
		var delay = GetFiremode()->Call(Format("Get%sDelay", aim_type));

		// Play animation for set amount of time
		number = clonk->PlayAnimation(anim, CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, clonk->GetAnimationLength(anim), delay, ANIM_Hold), Anim_Const(1000));
	}
	else if (trans == WEAPON_AIM_TRANS_BLND)
	{
		var delay = GetFiremode()->Call(Format("Get%sDelay", aim_type));
		var current_anim = clonk->GetRootAnimation(CLONK_ANIM_SLOT_Arms);
		var anim = GetFiremode()->Call(Format("Get%sAimingAnimation", aim_type));
		var length = clonk->GetAnimationLength(anim);
		var angle = Abs(Normalize(clonk->Angle(0,0, x, y), -180)) * 10;
		// Just to be sure, end all arms animations
		if (current_anim != nil)
			clonk->StopAnimation(current_anim);

		// Blend current animation (most likely a movement animation) into aiming animation
		number = clonk->PlayAnimation(anim, CLONK_ANIM_SLOT_Movement, Anim_Const(angle * length / 1800), Anim_Linear(0, 0, 999, delay, ANIM_Remove));
	}
	else
	{
		FatalError(Format("Invalid transition type: %s", trans));
	}

	this->CreateEffect(AimingHelper, 1, delay, clonk, aim_type, [x, y], number);
}

// Change one aiming stance into another
public func ChangeAiming(object clonk, string new_type, int x, int y)
{
	AssertNotNil(clonk, "CMC Firearm Library: ChangeAiming was called without a valid clonk parameter.");
	AssertNotNil(new_type, "CMC Firearm Library: ChangeAiming was called without a valid new_type parameter.");

	// Should have called StartAiming instead!
	if (!is_aiming)
		return StartAiming(clonk, new_type, x, y);

	// Check necessary requirements
	if (!Call(Format("Can%sAim", new_type), clonk))
		return;

	aim_transition = true;
	change_aiming = true;
	Call(Format("Unset%sAim", current_aim_type), clonk);

	// Transition into new aiming
	var trans = GetFiremode()->Call(Format("Get%sTransition", new_type));
	var number;
	if (trans == WEAPON_AIM_TRANS_INST)
	{
		FinishAiming(clonk, new_type, x, y);
		return;
	}
	else if (trans == WEAPON_AIM_TRANS_ANIM)
	{
		var anim = GetFiremode()->Call(Format("Get%sAnimation", new_type));
		var delay = GetFiremode()->Call(Format("Get%sDelay", new_type));

		// Play animation for set amount of time
		number = clonk->PlayAnimation(anim, CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, clonk->GetAnimationLength(anim), delay, ANIM_Hold), Anim_Const(1000));
	}
	else if (trans == WEAPON_AIM_TRANS_BLND)
	{
		var delay = GetFiremode()->Call(Format("Get%sDelay", new_type));
		var anim = GetFiremode()->Call(Format("Get%sAimingAnimation", new_type));
		var length = clonk->GetAnimationLength(anim);
		var angle = Abs(Normalize(clonk->Angle(0,0, x, y), -180)) * 10;

		// Blend current aiming animation into new aiming animation
		number = clonk->PlayAnimation(anim, CLONK_ANIM_SLOT_Arms, Anim_Const(angle * length / 1800), Anim_Linear(0, 0, 999, delay, ANIM_Remove));
	}
	else
	{
		FatalError(Format("Invalid transition type: %s", trans));
	}

	this->CreateEffect(AimingHelper, 1, delay, clonk, new_type, [x, y], number);
}

// Start the actual aiming procedure
func FinishAiming(object clonk, string aim_type, int x, int y)
{
	if (change_aiming)
	{
		clonk->ChangeAim(this, aim_type);
	}
	else
	{
		clonk->StartAim(this, 0, aim_type);
	}
	ScheduleCall(clonk, "UpdateAttach", 1);

	aim_transition = false;
	current_aim_type = aim_type;
	change_aiming = false;

	Call(Format("Set%sAim", aim_type), clonk, x, y);

	ContinueAiming(clonk, x, y, true);
}

// To be called every frame (preferably) during aiming
func ContinueAiming(object clonk, int x, int y, bool button_pressed)
{
	var angle = GetAngle(x, y);
	clonk->SetAimPosition(angle);
	aim_target = [clonk->GetX() + x, clonk->GetY() + y];

	Call(Format("~Continue%sAim", current_aim_type), clonk, button_pressed);
}

func FailedAiming(object clonk, string aim_type)
{
	is_aiming = false;
	aim_transition = false;
	current_aim_type = nil;
}

func StopAiming(object clonk)
{
	if (clonk->IsAiming())
	{
		clonk->StopAim();
	}

	var aim_type = current_aim_type;
	if (aim_type)
	{
		Call(Format("Unset%sAim", current_aim_type), clonk);
	}
	else
	{
		if (GetEffect(AimingHelper.Name, this))
		{
			var effect = GetEffect(AimingHelper.Name, this);
			effect->Cancel();
			effect.end_if_not_ended = true;
		}
	}

	is_aiming = false;
	aim_transition = false;
	current_aim_type = nil;
}

local AimingHelper = new Effect 
{
	Name = "AimingHelper", 

	Construction = func(object clonk, string aim_type, array pos, int anim)
	{
		this.clonk = clonk;
		this.type = aim_type;
		this.x = pos[0];
		this.y = pos[1];
		this.anim = anim;
	},

	Timer = func()
	{
		// This effect should end now if it hasn't ended (see Reset())
		if (this.end_if_not_ended)
		{
			return FX_Execute_Kill;
		}
		// Aiming failed because of unknown things
		if (!this.Target->Call(Format("Check%sTransition", this.type), this.clonk))
		{
			this.Target->FailedAiming(this.clonk, this.type);
			return FX_Execute_Kill;
		}
		this.clonk->ResetAnimationEffects();
		this.Target->FinishAiming(this.clonk, this.type, this.x, this.y);
		return FX_Execute_Kill;
	},

	Cancel = func()
	{
		this.clonk->StopAnimation(this.anim);
	}
};

/* --- Aim settings --- */

// --- Hip fire aiming

func CanHipFireAim(object clonk)
{
	if (!clonk || !clonk->GetAlive())
		return false; // ???

	// Check clonk
	if (!clonk->IsWalking() && !clonk->IsJumping())
		return false;

	// Check weapon
	if (GetFiremode()->~GetHipShootingDisabled())
		return false;

	return true;
}

func CheckHipFireTransition(object clonk)
{
	if (!clonk)
		return false;
	if (!clonk->GetAlive())
		return false;
	if (!clonk->IsWalking() && !clonk->IsJumping())
		return false;

	return true;
}

func SetHipFireAim(object clonk, int x, int y)
{
	ActivateAimingCursor(clonk);
	this->CreateEffect(HipFireEffect, 1, 1, clonk);
	// Fire away!
	// Call library default firing mechanic
	DoFireCycle(clonk, x, y, false);
}

func ContinueHipFireAim(object clonk, bool button_pressed)
{
	// Set this to true, to stop the effect from ending
	// The effect sets this to false every frame
	if (button_pressed)
		hipfire_pressed = true;
}

func UnsetHipFireAim(object clonk)
{
	DeactivateAimingCursor(clonk);
	RemoveEffect(HipFireEffect.Name, this, nil, true);
}

local HipFireEffect = new Effect
{
	Name = "HipFireEffect",

	Construction = func (object clonk)
	{
		// Time duration in which no firing command was issued
		this.timeout = 0;

		this.clonk = clonk;
	},

	Timer = func()
	{
		// If no firing button is pressed: schedule stop
		if (!this.Target.hipfire_pressed)
		{
			this.timeout++;
			if (this.timeout >= WEAPON_HipFireDelay)
				return FX_Execute_Kill;
		} else
		{
			this.timeout = 0;
			this.Target.hipfire_pressed = false;
		}
		// Clonk cannot aim anymore
		if (!this.clonk || (!this.clonk->IsWalking() && !this.clonk->IsJumping()))
			return FX_Execute_Kill;
		return FX_OK;
	},

	Stop = func(int reason, bool temp)
	{
		if (reason != FX_Call_Normal)
			return;
		if (this.Target)
			this.Target->StopAiming(this.clonk);
	}
};

// --- Ironsight aiming

func CanIronsightAim(object clonk)
{
	if (!clonk || !clonk->GetAlive())
		return false; // ???

	// Check clonk
	if (!clonk->IsWalking())
		return false;

	return true;
}

func CheckIronsightTransition(object clonk)
{
	if (!clonk)
		return false;
	if (!clonk->GetAlive())
		return false;
	if (!clonk->IsWalking())
		return false;

	return true;
}

func SetIronsightAim(object clonk)
{
	if (IsIronsightToggled(clonk->GetOwner()))
		ActivateAimingCursor(clonk);

	clonk->SetCancelOnJump(true);
	// TODO: Test & maybe make depended on weapon shoot mode?
	clonk->SetAimViewOffsetMin(50);
	clonk->SetAimViewOffsetMax(150);

	PlaySoundAim(clonk);
}

func UnsetIronsightAim(object clonk)
{
	if (IsIronsightToggled(clonk->GetOwner()))
		DeactivateAimingCursor(clonk);
}

// --- Prone aiming

func CanProneAim(object clonk)
{
	if (!clonk || !clonk->GetAlive())
		return false; // ???

	// Check clonk
	if (!clonk->IsProne())
		return false;

	return true;
}

func CheckProneTransition(object clonk)
{
	if (!clonk)
		return false;
	if (!clonk->GetAlive())
		return false;
	if (!clonk->IsProne())
		return false;

	return true;
}

func SetProneAim(object clonk)
{
	if (IsIronsightToggled(clonk->GetOwner()))
		ActivateAimingCursor(clonk);

	clonk->SetCancelOnJump(true);
	// TODO: Test & maybe make depended on weapon shoot mode?
	clonk->SetAimViewOffsetMin(50);
	clonk->SetAimViewOffsetMax(150);
}

func UnsetProneAim(object clonk)
{
	if (IsIronsightToggled(clonk->GetOwner()))
		DeactivateAimingCursor(clonk);
}

// Deactivate the default OC aiming cursor and activate the CMC aiming cursor
// The CMC cursor will send the cursor location continously until turned off
func ActivateAimingCursor(object clonk)
{
	SetPlayerControlEnabled(clonk->GetOwner(), CON_CMC_AimingCursor, true);
	SetPlayerControlEnabled(clonk->GetOwner(), CON_Aim, false);
}

func DeactivateAimingCursor(object clonk)
{
	SetPlayerControlEnabled(clonk->GetOwner(), CON_CMC_AimingCursor, false);
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

public func OnNoAmmo(object user, proplist firemode) // FIXME: Wrong location
{
	_inherited(user, firemode);

	this->PlaySoundNoAmmo(user);
}

public func OnAmmoChange(id ammo_type)
{
	NotifyContainer();
	_inherited(ammo_type, ...);
}

/* --- Selection --- */

public func Selection(object user)
{
	var firemode = GetFiremode();
	if (firemode)
	{
		user->~RaiseFirearmSpread(firemode->~GetSpreadBySelection());
	}
	if (user && user->~IsClonk())
	{
		PlaySoundDeploy(user);
	}
	_inherited(user, ...);
}

public func Deselection(object user)
{
	// Properly stop all aiming procedures
	if (user && user->IsAiming())
	{
		user->CancelAiming(this);
	}
	if (IsAiming())
	{
		StopAiming(user);
	}
	_inherited(user, ...);
}

/* --- Reloading --- */

public func NeedsReload(object user, proplist firemode, bool user_requested)
{
	var out_of_ammo = !HasAmmo(firemode);
	var loaded = true;
	if (this->~AmmoChamberCapacity(firemode->GetAmmoID()))
	{
		loaded = this->AmmoChamberIsLoaded(firemode->GetAmmoID());
	}
	return user_requested || out_of_ammo || !loaded;
}

func OnReloading(object user, int x, int y, proplist firemode, proplist reload_state)
{
	if (reload_state && reload_state.RaiseSpread)
	{
		user->~RaiseFirearmSpread(firemode->~GetSpreadLimit());
	}
	_inherited(user, x, y, firemode, reload_state, ...);
}

/* --- Cooldown --- */

public func NeedsRecovery(object user, proplist firemode) // TODO: Make a custom callback instead in the library
{
	var out_of_ammo = !HasAmmo(firemode);
	if (out_of_ammo)
	{
		this->PlaySoundLastRound(user);
		this->~AmmoChamberEject(firemode->GetAmmoID());
	}
	return true;
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
			status->SetObjectConfiguration(GuiGetFiremodeString(firemode));
	}

	return status;
}

func GuiGetFiremodeString(proplist firemode, id ammo_type)
{
	ammo_type = ammo_type ?? firemode->GetAmmoID();
	return Format("<c %x>%s</c> - %s", GUI_CMC_Text_Color_Highlight, firemode->GetAmmoName() ?? ammo_type->GetName(), firemode->GetName());
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
		ret.WalkBack = nil;
		ret.WalkSpeed = nil;
	} else if (type == WEAPON_AIM_TYPE_IRONSIGHT)
	{
		var aim_animation = GetFiremode()->GetIronsightAimingAnimation();
		if (aim_animation != nil)
			ret.AnimationAim = aim_animation;
	} else if (type == WEAPON_AIM_TYPE_PRONE) {
		// Aiming animation
		var aim_animation = GetFiremode()->GetProneAimingAnimation();
		if (aim_animation != nil)
			ret.AnimationAim = aim_animation;
		// No walking speed
		ret.WalkBack = 0;
		ret.WalkSpeed = 0;
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
	if (current_aim_type)
		Call(Format("Unset%sAim", current_aim_type), clonk);

	is_aiming = false;
	aim_transition = false;
	change_aiming = false;
	current_aim_type = nil;
	aim_target = nil;

	var effect = GetEffect(AimingHelper.Name, this);
	if (effect)
		effect.end_if_not_ended = true;
}

/* --- Status --- */

public func IsAiming()
{
	return is_aiming;
}

public func GetAimTarget()
{
	if (!aim_target)
		return [nil, nil];
	return aim_target[:];
}

public func GetAimType()
{
	return current_aim_type;
}

public func IsAutoFiring()
{
	return GetFiremode()->GetMode() == WEAPON_FM_Auto;
}

/* --- Firemode selection --- */

public func ControlUseItemConfig(object user, int x, int y, int status)
{
	var ready = !(this->~RejectUse(user));
	if (status == CONS_Up)
	{
		CloseListSelectionMenu(ready);
	}
	else if (ready)
	{
		OpenListSelectionMenu(user, CMC_LIST_MENU_TYPE_FIREMODE_SELECTION);
	}
	return true;
}


// Opens the 
public func GetListSelectionMenuEntries(object user, string type, proplist main_menu)
{
	if (type == CMC_LIST_MENU_TYPE_FIREMODE_SELECTION)
	{
		main_menu->SetHeaderCaption("$ConfigureFirearm$");

		// Fill with contents
		var available_modes = GetAvailableFiremodes();
		if (GetLength(available_modes) == 0)
		{
			FatalError("CMC Firearm Library: No firemodes available...");
		}
		else
		{
			var list = main_menu->GetList();
			var current_mode = GetFiremode();

			// Add entry for changing the current fire technique
			var next_index = -1;
			var toggle_modes = [];
			for (var mode in available_modes)
			{
				if (mode->GetAmmoID() == current_mode->GetAmmoID())
				{
					PushBack(toggle_modes, mode->GetIndex());
				}
			}
			if (GetLength(toggle_modes) > 0)
			{
				var current_index = GetIndexOf(toggle_modes, current_mode->GetIndex());
				var next = (current_index + 1) % GetLength(toggle_modes);
				next_index = toggle_modes[next];
			}

			var default_entry = list->MakeEntryProplist();
			var default_action = "$ChangeFireTechnique$";
			if (next_index > -1)
			{
				default_action = Format("%s (<c %x>%s</c>)", default_action, GUI_CMC_Text_Color_Highlight, GetFiremode(next_index)->GetName());
			    default_entry->SetCallbackOnMouseIn(list->DefineCallback(list.SelectEntry, default_action))             // Select the entry by hovering; the other possibilities are scrolling and hotkey
			                 ->SetCallbackOnClick(this->DefineCallback(this.CloseListSelectionMenu, true))              // Clicking the entry closes the menu; It is automatically selected, because you hover the entry to click it; 'false' means that the selection is not cancelled
			                 ->SetCallbackOnMenuClosed(this->DefineCallback(this.DoMenuFiremodeSelection, next_index)); // Closing the menu selects the entry
			}
			else
			{
				default_action = Format("<c %x>%s ($OnlyOneFireTechnique$)</c>", GUI_CMC_Text_Color_Inactive, default_action);
				default_entry.SetSelected = CMC_GUI_SelectionListSeparator.SetSelected; // Overwrite selection thing, so that the entry cannot be selected
			}
			default_entry->SetCaption(default_action)
			             ->SetScrollHint(true);
			list->AddEntry(default_action, default_entry);
			this->~SetListSelectionMenuHotkey(default_entry, 9);

			var last_ammo_type = nil;
			for (var firemode in available_modes) 
			{
				// Separate ammo types by an empty line
				var current_ammo_type = firemode->GetAmmoID();
				if (last_ammo_type && last_ammo_type != current_ammo_type)
				{
					list->AddEntry(nil, list->MakeSeparatorProplist());
				}
				last_ammo_type = current_ammo_type;

				// Text and description
				var name = GuiGetFiremodeString(firemode);
				var index = firemode->GetIndex();

				var entry = list->MakeEntryProplist();
				entry->SetIcon(current_ammo_type)
				     ->SetCaption(name)
				     ->SetCallbackOnMouseIn(list->DefineCallback(list.SelectEntry, name))           // Select the entry by hovering; the other possibilities are scrolling and hotkey
				     ->SetCallbackOnClick(DefineCallback(this.CloseListSelectionMenu, true))        // Clicking the entry closes the menu; It is automatically selected, because you hover the entry to click it; 'false' means that the selection is not cancelled
				     ->SetCallbackOnMenuClosed(DefineCallback(this.DoMenuFiremodeSelection, index)) // Closing the menu selects the entry
				     ->SetScrollHint(true);
				list->AddEntry(name, entry);
				SetListSelectionMenuHotkey(entry, index);
			}
		}
	}
	return _inherited(user, type, main_menu, ...);
}

private func DelayListSelectionMenu(object user, string type)
{
	if (type == CMC_LIST_MENU_TYPE_FIREMODE_SELECTION)
	{
		return CMC_LIST_MENU_DELAY_SHORT;
	}
	return _inherited(user, type);
}

public func DoMenuFiremodeNothing(){}

public func DoMenuFiremodeSelection(int index)
{
	AssertNotNil(index);
	ScheduleSetFiremode(index);
}

public func SetFiremode(int number, bool force)
{
	if (Contained())
	{
		Contained()->~OnInventoryChange();
		PlaySoundSwitchFiremode(Contained());
	}
	return inherited(number, force);
}

/* --- Effects --- */

func FireSound(object user, proplist firemode)
{
	Sound(firemode->GetCurrentFireSound(), {multiple = true});
}

func PlaySoundDeploy(object user)
{
	Sound("Items::Weapons::Shared::Deploy?", {player = user->GetOwner()});
}

func PlaySoundAim(object user)
{
	Sound("Items::Weapons::Shared::Aim?", {player = user->GetOwner()});
}

func PlaySoundResupplyAmmo()
{
	Sound("Items::Tools::AmmoBox::ResupplyIn?");
}

func PlaySoundSwitchFiremode(object user)
{
	Sound("Items::Weapons::Shared::Switch?");
}

func PlaySoundLastRound(object user)
{
	Sound("Items::Weapons::Shared::LastRoundIndicator?", {player = user->GetOwner()});
}

func PlaySoundNoAmmo(object user)
{
	Sound("Items::Weapons::Shared::Empty", {player = user->GetOwner()});
}
		
func MuzzleFlash(object user, int angle, int size)
{
	var muzzle = GetWeaponPosition(user, WEAPON_POS_Muzzle, angle);
	EffectMuzzleFlash(user, muzzle.X, muzzle.Y, angle, size ?? 20, false, true);
}

func EjectCasing(object user, int angle, string type, int size, int xdir, int ydir)
{
	var chamber = GetWeaponPosition(user, WEAPON_POS_Chamber, angle);
	xdir = xdir ?? (user->GetCalcDir() * 14 * RandomX(-2, -1));
	ydir = ydir ?? RandomX(-11, -13);
	CreateCartridgeEffect(type, size, chamber.X, chamber.Y, user->GetXDir() + xdir, user->GetYDir() + ydir);
}


/* --- Reloading --- */

func IsUserReadyToReload(object user)
{
	return user->HasActionProcedure(false) // Can act
	    && user->GetHandItem(0) == this;   // Item is selected
}

func StartLoaded()
{
	var ammo_types = [];
	for (var firemode in GetFiremodes())
	{
		// Do it only once per ammo type
		var type = firemode->GetAmmoID();
		var amount = firemode->GetAmmoAmount();
		if (!IsValueInArray(ammo_types, type))
		{
			// Loaded
			SetAmmo(type, amount);
			// Bullet is chambered (library call already checks whether it is necessary, etc.)
			this->~AmmoChamberInsert(type);
			PushBack(ammo_types, type);
		}
	}
}
