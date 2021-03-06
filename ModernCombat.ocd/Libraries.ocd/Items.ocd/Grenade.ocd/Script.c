/**
	Library for grenades

	Represents grenade item functionality.

	@author Marky
*/

#include Library_RangedWeapon
#include CMC_Library_Grenade

/* --- Properties --- */

local animation_set;

local Grenade_ThrowSpeed = 20; // Multiplication factor to clonk.ThrowSpeed
local Grenade_ThrowDelay = 10; // Time aiming and throwing in case of holding-enabled grenades

local DefaultShootTime = 16;
local DefaultShootTime2 = 8;


/* --- Using the grenade --- */

// Use holding controls?
public func HoldingEnabled()
{
	return true;
}


public func RejectUse(object user)
{
	return !user->HasActionProcedure(false); // The clonk must be able to use the hands
}

// --- Left click

public func ControlUseStart(object user, int x, int y)
{
	if (IsGrenadeHoldEnabled(user->GetController()))
	{
		Fuse();
		StartAim(user, GetAimPosition(user, x, y));
	}
	else if (user->~IsAiming()) // The usual mode
	{
		ThrowAimed(user, GetAimPosition(user, x, y));
	}
	else if (IsActive())// If fused via alt use
	{
		StartAim(user, GetAimPosition(user, x, y));
	}
	return true;
}

public func ControlUseHolding(object user, int x, int y)
{
	return true;
}

public func ControlUseCancel(object user, int x, int y)
{
	CancelAim(user);
	return true;
}

public func ControlUseStop(object user, int x, int y)
{
	if (IsGrenadeHoldEnabled(user->GetController()) || IsActive())
	{
		SetDelayedAction(this.ThrowAimed, user, GetAimPosition(user, x, y));
	}
	else
	{
		Fuse(); // Allows better timing if holding is disabled
		StartAim(user, GetAimPosition(user, x, y));
	}
	return true;
}

// --- Right click

public func ControlUseAltStart(object user, int x, int y)
{
	if (IsGrenadeHoldEnabled(user->GetController()))
	{
		Fuse();
	}
	else if (IsActive())
	{
		StartLob(user, GetAimPosition(user, x, y));
	}
	return true;
}

public func ControlUseAltHolding(object user, int x, int y)
{
	return true;
}

public func ControlUseAltCancel(object user, int x, int y)
{
	CancelAim(user);
	return true;
}

public func ControlUseAltStop(object user, int x, int y)
{
	if (IsGrenadeHoldEnabled(user->GetController()))
	{
		SetDelayedAction(this.StartLob, user, GetAimPosition(user, x, y));
	}
	else
	{
		Fuse(); // Allows better timing if holding is disabled
	}
	return true;
}


/* --- Aiming --- */


// Right click will by default be a toggle control but can be switched to a holding control
public func IsGrenadeHoldEnabled(int player)
{
	return CMC_Player_Settings->GetConfigurationValue(player, CMC_GRENADE_HOLD, true);
}


// Called by the CMC modified clonk, see ModernCombat.ocd\System.ocg\Mod_Clonk.c
public func ControlUseAiming(object user, int x, int y)
{
	user->SetAimPosition(GetAimPosition(user, x, y), true);

	if (!user->HasActionProcedure(false))
	{
		CancelAim(user);
	}
}

func GetAimPosition(object user, int x, int y)
{
	// Save new angle
	var angle = Angle(0, 0, x, y);
	angle = Normalize(angle, -180);

	if (angle >  160) angle =  160;
	if (angle < -160) angle = -160;

	// Update delayed effect
	var fx = GetEffect("BlockGrenadeThrow", user);
	if (fx)
	{
		fx.Angle = angle;
	}

	return angle;
}


func StartAim(object user, int angle)
{
	if (!user->IsAiming())
	{
		user->StartAim(this, angle);
	}
	SetAimingCursor(user, true);
}


func StopAim(object user)
{
	if (user->IsAiming())
	{
		user->StopAim();
	}
	SetAimingCursor(user, false);
}


func CancelAim(object user)
{
	CancelDelayedAction(user);
	user->CancelAiming(this);
	if (IsActive())
	{
		DoDrop(user);
	}
	SetAimingCursor(user, false);
}


func SetDelayedAction(call, object user, int angle)
{
	var fx = GetEffect("BlockGrenadeThrow", user);
	if (fx)
	{
		fx.OnFinish = call;
		fx.Grenade = this;
		fx.Angle = angle;
		if (user->IsAiming())
		{
			user->SetAimPosition(angle);
		}
	}
	else
	{
		Call(call, user, angle);
	}
}


func CancelDelayedAction(user)
{
	var fx = GetEffect("BlockGrenadeThrow", user);
	if (fx)
	{
		fx.OnFinish = nil;
		RemoveEffect("BlockGrenadeThrow", user);
	}	
}


func ThrowAimed(object user, int angle)
{
	// The actual throw, only while walking
	if (user->~IsWalking() || user->GetProcedure() == "KNEEL")
	{
		// Nothing; might seem confusing, but: Aiming has to be stopped anyway,
		// we just do other things first in the other cases, and those lead to
		// the grenade not being thrown
	}
	// While jumping you do the short throw
	else if (user->~IsJumping())
	{
		StartLob(user, angle);
	}
	else if (user->~HasActionProcedure(true))
	{
		DoDrop(user);
	}
	StopAim(user);
	return true;
}


func SetAimingCursor(object user, bool value, bool forced)
{
	var controller = user->GetController();

	// Mouse move will adjust aim angle
	SetPlayerControlEnabled(controller, CON_CMC_AimingCursor, value);
	// Disable OC default
	SetPlayerControlEnabled(controller, CON_Aim, !value);
}

func RemoveAimAnimation(object user)
{
	user->StopAnimation(user->GetRootAnimation(CLONK_ANIM_SLOT_Arms));
}


/* --- Engine callbacks --- */

func Initialize()
{
	DefaultLoadTime = this.Grenade_ThrowDelay;
	animation_set = {
		AimMode         = AIM_Position, // The aiming animation is done by adjusting the animation position to fit the angle
		AnimationAim    = "SpearAimArms",
		AnimationShoot  = "SpearThrowArms",
		AnimationShoot2 = "SpearThrow2Arms",
		AnimationShoot3 = "SpearThrow3Arms",
		WalkBack        =  56,
	};
	_inherited(...);
}


func Selection(object container)
{
	if (container && container->~IsClonk())
	{
		PlaySoundDeploy();
	}
	return _inherited(container, ...);
}


func Deselection(object container)
{
	var self = this;
	if (container->~IsClonk())
	{
		TryStashGrenade(container);
	}
	if (self)
	{
		return _inherited(container, ...);
	}
	else
	{
		return false;
	}
}


func Departure(object from)
{
	if (IsActive())
	{
		from->PlayerMessage(from->GetController(), "");
	}
	_inherited(from, ...);
}


func RejectEntrance(object into)
{
	var other_grenade = FindObject(Find_Func("IsGrenade"), Find_Container(into));
	if (other_grenade)
	{
		return true;
	}
	return _inherited(into, ...);	
}


/* --- Callbacks from loading/aiming system --- */

func GetAnimationSet() { return animation_set; }


// Callback from the clonk, when he actually has stopped aiming
func FinishedAiming(object user, int angle)
{
	if (user == Contained() && !GetEffect("ThrowingAnimation", user))
	{
		user->StartShoot(this);
		return true;
	}
	else
	{
		return false;
	}
}


// Called in the half of the shoot animation (when ShootTime2 is over)
public func DuringShoot(object user, int angle)
{
	DoThrow(user, angle);
}


/* --- Sounds --- */


func PlaySoundActivate()
{
	Sound("Items::Grenades::Shared::Activate?");
}


func PlaySoundDeploy()
{
	Sound("Items::Grenades::Shared::Deploy?");
}


func PlaySoundThrow()
{
	Sound("Items::Grenades::Shared::Throw?", {multiple = true});
}


/* --- Countdown until explosion --- */

func Fuse()
{
	if (!IsActive())
	{
		PlaySoundActivate();
  		SetGraphics("Active");
		this.Collectible = false;
		SetCategory(C4D_Vehicle);

		var user = Contained();
		if (user)
		{
			user->CreateEffect(BlockGrenadeThrow, 1, 1, this, this.Grenade_ThrowDelay);
		}
	}
	inherited(...);
}


local BlockGrenadeThrow = new Effect
{
	OnFinish = nil, // Function call when finished
	Grenade = nil,  // Object that will do the function call
	Angle = nil,    // Angle for the function call

	Start = func (int temp, object grenade, int lifetime)
	{
		if (!temp)
		{
			this.Grenade = grenade;
			this.Lifetime = lifetime;
		}
		return FX_OK;
	},

	Timer = func (int time)
	{
		if (!this.Grenade || time > this.Lifetime)
		{
			return FX_Execute_Kill;
		}
		if (this.Grenade->RejectUse(this.Target))
		{
			this.Grenade->CancelAim(this.Target);
			return FX_Execute_Kill;
		}
		return FX_OK;
	},

	Stop = func (int temp)
	{
		if (!temp && this.Target && this.OnFinish && this.Grenade)
		{
			this.Grenade->Call(this.OnFinish, this.Target, this.Angle);
			this.OnFinish = nil;
		}
	},
};


func HandleTrail(int time)
{
	var container = Contained();
	if (container)
	{
		if (container->~IsClonk() && !container->GetAlive() || container->~IsIncapacitated())
		{
  			this->DoDrop(container);
  		}
		else if ((container.GetHandItem && this == container->GetHandItem(0)) // Has inventory control? => Get correct item
		     || (!container.GetHandItem && this == container->Contents(0)))   // No inventory control? => First item
		{
		    var progress = BoundBy(time * 1000 / Max(1, this.Grenade_FuseTime), 0, 1000);
			var color = InterpolateRGBa(progress,   0, RGB(0, 255, 0),
			                                      500, RGB(255, 255, 0),
			                                     1000, RGB(255, 0, 0));

			container->PlayerMessage(container->GetController(),"<c %x>{{Rock}}</c>", color); // FIXME: placeholder for graphics
		}
	}

	inherited(time);
}

/* --- Animations --- */

func Launch(object user)
{
	user = user ?? Contained();
	Exit();
	if (user)
	{
		SetController(user->GetController());
		RemoveEffect("BlockGrenadeThrow", user);
		user->UpdateAttach();
	}
	SetRDir(RandomX(-6, +6));
}

func StartLob(object user, int angle)
{
	user->CancelAiming(this);
	if (Normalize(angle, -180) >= 0)
		user->SetDir(DIR_Right);
	else
		user->SetDir(DIR_Left);

	if (user->~IsClonk())
	{
		user->~SetHandAction(1); // Set hands ocupied
		user->CreateEffect(ThrowingAnimation, 1, 1, this, angle);
	}
}

func FinishLob(object user, int angle)
{
	Launch(user);
	var div = 60;
	SetVelocity(angle, 30);
	AddSpeed(div * user->GetXDir(1000) / 100, 
	         div * user->GetYDir(1000) / 100 - 50,
	         1000);
	SetPosition(user->GetX(), user->GetY() + 2);
	if (!GetEffect("RollingFriction", this))
	{
		CreateEffect(RollingFriction, 1, 70); // Roll with reduced friction for 2 seconds
	}
	SetRDir(Sign(GetXDir()) * 10);
 }

func DoThrow(object user, int angle)
{
	var div = 60; // 40% is converted to the direction of the throwing angle.
	var xdir = user->GetXDir(1000);
	var ydir = user->GetYDir(1000);
	var speed = user.ThrowSpeed * Grenade_ThrowSpeed + (100 - div) * Distance(xdir, ydir) / 100;
	var grenade_x = div * xdir / 100 + Sin(angle, speed);
	var grenade_y = div * ydir / 100 - Cos(angle, speed);

	Launch(user);
	SetXDir(grenade_x, 1000);
	SetYDir(grenade_y, 1000);
	SetPosition(user->GetX(), user->GetY() - 6);
}

func DoDrop(object user)
{
	Launch(user);
	if (user)
	{
		SetSpeed(user->GetXDir(), user->GetYDir());
	}
}


func TryStashGrenade(object user)
{
	// User has a grenade belt? Stash it!
	if (user && user.StashGrenade)
	{
		if (IsActive())
		{
			DoDrop(user);
		}
		else
		{
			user->StashGrenade(this);
		}
	}
}

local ThrowingAnimation = new Effect
{
	Start = func (int temporary, object thrown, int angle)
	{
		if (!temporary)
		{
			this.Throw_Time = 16;
			this.Throw_Object = thrown;
			this.Throw_Angle = angle;
			this.Target->PlayAnimation("ThrowArms", CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, this.Target->GetAnimationLength("ThrowArms"), this.Throw_Time));
		}
	},

	Timer = func(int time)
	{
		if (this.Throw_Object)
		{
			if (time == this.Throw_Time * 8 / 15)
			{
				this.Throw_Object->FinishLob(this.Target, this.Throw_Angle);
			}
			if (time < this.Throw_Time)
			{
				return FX_OK;
			}
		}
		return FX_Execute_Kill;
	},

	Stop = func (int temporary)
	{
		if(!temporary)
		{
			this.Target->StopAnimation(this.Target->GetRootAnimation(CLONK_ANIM_SLOT_Arms));
			this.Target->~SetHandAction(0);
		}
	},
};


local RollingFriction = new Effect
{
	Start = func (int temporary)
	{
		if (!temporary)
		{
			this.Friction = [];
			for (var i = 0; i < this.Target->GetVertexNum(); ++i)
			{
				this.Friction[i] = this.Target->GetVertex(i, VTX_Friction);
				this.Target->SetVertex(i, VTX_Friction, this.Friction[i] / 4, 2);
			}
		}
	},

	Stop = func (int temporary)
	{
		if (!temporary)
		{
			for (var i = 0; i < this.Target->GetVertexNum(); ++i)
			{
				this.Target->SetVertex(i, VTX_Friction, this.Target->GetID()->GetVertex(i, VTX_Friction), 2);
			}
		}
	},
};
