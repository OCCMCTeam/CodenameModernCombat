#appendto Library_AimManager

// Modify the aim procedure check, to cancel aiming on certain other events
// if wanted.
// Further modify to accomodate for prone aiming

local aim_cancel_on_jump;

func FxIntAimCheckProcedureTimer()
{
	// Care about the aim schedules
	if(aim_schedule_timer != nil)
	{
		aim_schedule_timer--;
		if(aim_schedule_timer == 0)
		{
			Call(aim_schedule_call);
			aim_schedule_call = nil;
			aim_schedule_timer = nil;
		}
	}
	if(aim_schedule_timer2 != nil)
	{
		aim_schedule_timer2--;
		if(aim_schedule_timer2 == 0)
		{
			Call(aim_schedule_call2);
			aim_schedule_call2 = nil;
			aim_schedule_timer2 = nil;
		}
	}

	// check procedure
	if(!ReadyToAction() && aim_type != WEAPON_AIM_TYPE_PRONE)
		PauseAim();
	if (aim_type == WEAPON_AIM_TYPE_PRONE)
		if (this->GetAction() != "Crawl")
			return CancelAiming();

	if (this->IsAiming())
		if (aim_cancel_on_jump)
			if (this->IsJumping())
				this->CancelAiming();
}

public func SetCancelOnJump(bool cancel)
{
	aim_cancel_on_jump = cancel;
}

func PauseAim()
{
	if (aim_cancel_on_jump) return CancelAiming();
	return _inherited();
}

// Modify the aim manager to take in an 'aim type' variable.
// Because in CMC one weapon has different styles of aiming (ironsight, hip shooting, lying down), the aim manager
// should preserve a value to distinguish these.
// Unfortunately, with a clear approach this requires a lot of overriding functions from the aim manager, mostly
// so GetAnimationSet can take different aiming styles into account (something the original aiming manager isn't)
// equipped for.
// The actual change is to pass on this everytime GetAnimationSet is called.

local aim_type;

// If type is not set, it will be "Default"
public func StartLoad(object weapon, string type)
{
	// only if we aren't adjusted to this weapon already
	if(weapon != aim_weapon)
	{
		// Reset old
		if(aim_weapon != nil) aim_weapon->~Reset();
		if(aim_set    != nil) ResetHands();

		// Remember new
		aim_weapon = weapon;
		aim_type = type;
		if (!type)
			aim_type = "Default";
		aim_set = weapon->~GetAnimationSet(this);

		// Apply the set
		ApplySet(aim_set);

		// Add effect to ensure procedure
		AddEffect("IntAimCheckProcedure", this, 1,  1, this);
	}

	if(aim_set["AnimationLoad"] != nil)
		PlayAnimation(aim_set["AnimationLoad"], CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationLoad"]), aim_set["LoadTime"], ANIM_Remove), Anim_Const(1000));

	aim_schedule_timer = aim_set["LoadTime"];
	aim_schedule_call  = "StopLoad";
	
	if(aim_set["LoadTime2"] != nil)
	{
		aim_schedule_timer2 = aim_set["LoadTime2"];
		aim_schedule_call2  = "DuringLoad";
	}
}

// If type is not set, it will be "Default"
// angle does nothing here but this is copied from the original behaviour
public func StartAim(object weapon, int angle, string type)
{
	// only if we aren't adjusted to this weapon already
	if(weapon != aim_weapon)
	{
		// Reset old
		if(aim_weapon != nil) aim_weapon->~Reset();
		if(aim_set    != nil) ResetHands();

		// Remember new
		aim_weapon = weapon;
		aim_type = type;
		if (!type)
			aim_type = "Default";
		aim_set = weapon->~GetAnimationSet(this);

		// Apply the set
		ApplySet(aim_set);

		// Add effect to ensure procedure
		AddEffect("IntAimCheckProcedure", this, 1,  1, this);
	}

	if(aim_set["AnimationAim"] != nil)
	{
		if(aim_set["AimMode"] == AIM_Position)
			aim_animation_index = PlayAnimation(aim_set["AnimationAim"], CLONK_ANIM_SLOT_Arms, Anim_Const(GetAnimationLength(aim_set["AnimationAim"])/2), Anim_Const(1000));
		if(aim_set["AimMode"] == AIM_Weight)
		{
			aim_animation_index = PlayAnimation(aim_set["AnimationAim"],  CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationAim"]),  aim_set["AimTime"], ANIM_Loop), Anim_Const(1000));
			aim_animation_index = PlayAnimation(aim_set["AnimationAim2"], CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationAim2"]), aim_set["AimTime"], ANIM_Loop), Anim_Const(1000), aim_animation_index);
			aim_animation_index++;
			SetAnimationWeight(aim_animation_index, Anim_Const(500));
		}
	}
	AddEffect("IntAim", this, 1, 1, this);
}

// If type is not set, it will be "Default"
public func StartShoot(object weapon, string type)
{
	// only if we aren't adjusted to this weapon already
	if(weapon != aim_weapon)
	{
		// Reset old
		if(aim_weapon != nil) aim_weapon->~Reset();
		if(aim_set    != nil) ResetHands();

		// Remember new
		aim_weapon = weapon;
		aim_type = type;
		if (!type)
			aim_type = "Default";
		aim_set = weapon->~GetAnimationSet();

		// Applay the set
		ApplySet(aim_set);

		// Add effect to ensure procedure
		AddEffect("IntAimCheckProcedure", this, 1,  1, this);
	}

	if(aim_set["AnimationShoot"] != nil)
	{
		// Do we just have one animation? Then just play it
		if(aim_set["AnimationShoot2"] == nil)
			PlayAnimation(aim_set["AnimationShoot"], CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationShoot"]), aim_set["ShootTime"], ANIM_Remove), Anim_Const(1000));
		// Well two animations blend betweend them (animtion 1 is 0° animation2 for 180°)
		else if(aim_set["AnimationShoot3"] == nil)
		{
			var iAim;
			iAim = PlayAnimation(aim_set["AnimationShoot"],  CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationShoot"] ), aim_set["ShootTime"], ANIM_Remove), Anim_Const(1000));
			iAim = PlayAnimation(aim_set["AnimationShoot2"], CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationShoot2"]), aim_set["ShootTime"], ANIM_Remove), Anim_Const(1000), iAim);
			SetAnimationWeight(iAim+1, Anim_Const(1000*Abs(aim_angle)/180));
		}
		// Well then we'll have three to blend (animation 1 is 90°, animation 2 is 0°, animation 2 for 180°)
		else
		{
			var iAim;
			if(Abs(aim_angle) < 90)
			{
				iAim = PlayAnimation(aim_set["AnimationShoot2"], CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationShoot2"]), aim_set["ShootTime"], ANIM_Remove), Anim_Const(1000));
				iAim = PlayAnimation(aim_set["AnimationShoot"],  CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationShoot"] ), aim_set["ShootTime"], ANIM_Remove), Anim_Const(1000), iAim);
				SetAnimationWeight(iAim+1, Anim_Const(1000*Abs(aim_angle)/90));
			}
			else
			{
				iAim = PlayAnimation(aim_set["AnimationShoot"],  CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationShoot"] ), aim_set["ShootTime"], ANIM_Remove), Anim_Const(1000));
				iAim = PlayAnimation(aim_set["AnimationShoot3"], CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationShoot3"]), aim_set["ShootTime"], ANIM_Remove), Anim_Const(1000), iAim);
				SetAnimationWeight(iAim+1, Anim_Const(1000*(Abs(aim_angle)-90)/90));
			}
		}
	}

	aim_schedule_timer = aim_set["ShootTime"];
	aim_schedule_call  = "StopShoot";

	if(aim_set["ShootTime2"] != nil)
	{
		aim_schedule_timer2 = aim_set["ShootTime2"];
		aim_schedule_call2  = "DuringShoot";
	}
}

public func GetAimType()
{
	return aim_type;
}

// Modify the aiming effect to shift the camera if wanted.
// With this, aiming can shift according to the viewing angle.

public func SetAimViewOffset(int distance)
{
	var aim_effect = GetEffect("IntAim", this);
	if (!aim_effect) // Need to call StartAim first!
		return;

	aim_effect.view_offset = distance;
}

func FxIntAimTimer(target, effect, time)
{
	// Call the main function first, to set everything up
	var die = _inherited(target, effect, time);
	if (die == -1) // X_X
		return FX_Execute_Kill;
	// Check if a view offset is set
	if (effect.view_offset)
	{
		var x_offset = +Sin(this.aim_angle, effect.view_offset);
		var y_offset = -Cos(this.aim_angle, effect.view_offset);
		SetViewOffset(this->GetOwner(), x_offset, y_offset);
	}
}

func FxIntAimStop(object target, effect, int reason, bool temp)
{
	_inherited(target, effect, reason, temp);

	if (temp)
		return;

	// Reset viewing offset!
	if (effect.view_offset)
		SetViewOffset(this->GetOwner(), 0,0);
}

// Also reset all new settings

public func ResetHands()
{
	_inherited();

	aim_cancel_on_jump = nil;
	aim_type = nil;
}