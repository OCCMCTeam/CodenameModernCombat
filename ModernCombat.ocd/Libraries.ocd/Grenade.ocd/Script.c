/**
	Library for grenades

	@author Marky
*/


/* --- Properties --- */

local grenade_detonated = false; // bool: already detonated? Important for preventing multiple explosions, etc.
local grenade_active = false;    // bool: active?

local Grenade_ContainedDamage = 60;
local Grenade_FuseTime = 105; // 3 seconds
local Grenade_ThrowSpeed = 60;
local Grenade_ThrowDelay = 20; // Time between consecutive throws
local Grenade_MaxDamage = 10; // Take this many damage and it activates itself

func NoDecoDamage(){ return true; } // Forgot what this does, but lets leave it in there for now
func IsBouncy(){ return true; } // Gets launched by jump pad

/* --- Using the grenade --- */

// Use holding controls?
public func HoldingEnabled()
{
	/*if (Contained())
	{
		return IsIronsightToggled(Contained()->GetController());
	}*/
	return true;
}


public func ControlUseStart(object user, int x, int y)
{
	return user->~IsWalking();
}

public func ControlUseHolding(object clonk, int x, int y)
{
	return true;
}

public func ControlUseCancel(object user, int x, int y)
{
	return true;
}

public func ControlUseStop(object user, int x, int y)
{
	if (GetEffect("BlockGrenadeThrow", user))
	{
		return true;
	}
	if (grenade_active)
	{
		var throwAngle = Angle(0, 0, x, y);

		if (user->~IsWalking() && user->~HasHandAction() && user->~IsClonk())
		{
			if (throwAngle < 180)
				user->SetDir(DIR_Right);
			else
				user->SetDir(DIR_Left);

			user->~SetHandAction(1); // Set hands ocupied
			
			user->CreateEffect(ThrowingAnimation, 1, 1, this, throwAngle, true);
		}
	}
	else
	{
		Fuse();
	}
	return true;
}


// Right click will by default be a toggle control but can be switched to a holding control
public func IsIronsightToggled(int player)
{
	return CMC_Player_Settings->GetConfigurationValue(player, CMC_IRONSIGHT_TOGGLE, true);
}


/* --- Engine callbacks --- */


func Hit(int xdir, int ydir)
{
	PlaySoundHit();
	if (GBackSolid( 0, +5)) return SetYDir(-ydir/26);
	if (GBackSolid( 0, -5)) return SetYDir(-ydir/26);
	if (GBackSolid(-5,  0)) return SetXDir(-xdir/16);
	if (GBackSolid(+5,  0)) return SetXDir(-xdir/16);
}


func Collection(object by_object)
{
 	if (by_object->GetCategory() & C4D_Living)
 	{
    	PlaySoundCharge();
	}
	_inherited(by_object, ...);
}


func Departure(object from)
{
	if (grenade_active)
	{
		SetCategory(C4D_Vehicle);
		from->PlayerMessage(from->GetController(), "");
	}
	_inherited(from, ...);
}


func Damage(int change, int cause, int cause_plr)
{
	// Only do stuff if the object has the HitPoints property.
	if (this && this.Grenade_MaxDamage != nil)
	{
		if (GetDamage() >= this.Grenade_MaxDamage)
		{
			Fuse();
		}
	}
	return _inherited(change, cause, cause_plr, ...);
}


/* --- Sounds --- */


func PlaySoundActivate()
{
	Sound("Grenade::Activate");
}


func PlaySoundCharge()
{
	Sound("Grenade::Charge");
}


func PlaySoundDetonation()
{
	Sound("Grenade::Explosion?");
}


func PlaySoundHit()
{
	Sound("Grenade::Hit?", {multiple = true});
}


func PlaySoundThrow()
{
	Sound("Grenade::Throw?", {multiple = true});
}


/* --- Overloadable callbacks --- */

// What happens when the grenade explodes
public func OnDetonation()
{
	Explode(30, true, 60);
}

// How the trail is drawn
public func HandleTrail()
{
	if (!Contained())
	{
		var speed = Abs(GetXDir()) + Abs(GetYDir());
		var alpha = Min(205, 105 + speed);
		var lifetime_base = speed / 3;
		
		var color = SplitRGBaValue(this.Grenade_SmokeColor ?? RGB(100, 100, 100));
		
		CreateParticle("Smoke",
		               -GetXDir() / 6, -GetYDir() / 6,
		                PV_Random(-10, 10), -5,
		                PV_Random(10, 20),
		               {
			               Prototype = Particles_Thrust(),
			               Size = PV_Linear((lifetime_base + 20) / 10, (lifetime_base + 60) / 10),
			               R = color.R, G = color.G, B = color.B, Alpha = alpha
		               });
	}
}


/* --- Explosion logic --- */

// Callback that triggers detonation.
func Detonate() 
{
	RemoveEffect("GrenadeFuse", this, nil, true);

	if (!grenade_detonated)
	{
		grenade_detonated = true;
		PlaySoundDetonation();
		this->OnDetonation();
	}
}

// Callback for giving the container damage.
func DetonateInContainer()
{
	var container = Contained();
	if (container && this.Grenade_ContainedDamage > 0)
	{
		if (container->GetOCF() & OCF_Alive)
		{
			if (!container->Contained())
			{
				container->Fling(container->GetXDir()/10, container->GetYDir()/10 - 1);
			}
			container->DoEnergy(this.Grenade_ContainedDamage, false, FX_Call_EngBlast, this->GetController()); // FIXME: may be replaced with custom damage system
		}
	}
	Exit();
}

/* --- Countdown until explosion --- */

func Fuse()
{
	if (!grenade_active)
	{
		PlaySoundActivate();
		grenade_active = true;
		grenade_detonated = false;
  		SetGraphics("Active");
		CreateEffect(GrenadeFuse, 200, 1);
		this.Collectible = false;
	}
}

local GrenadeFuse = new Effect
{
	Timer = func (int time)
	{
		if (time > this.Target.Grenade_FuseTime)
		{
			this.Target->DetonateInContainer();
			this.Target->Detonate();
			return FX_Execute_Kill;
		}

		var container = this.Target->Contained();
		if (container)
		{
			if (container->~IsClonk() && !container->GetAlive() || container->~IsIncapacitated()) // FIXME: callback for wounded clonk
			{
      			Exit(0, 0, 8);
      		}
			else if ((container.GetHandItem && this.Target == container->GetHandItem(0)) // Has inventory control? => Get correct item
			     || (!container.GetHandItem && this.Target == container->Contents(0)))   // No inventory control? => First item
			{
			    var progress = BoundBy(time * 1000 / Max(1, this.Target.Grenade_FuseTime), 0, 1000);
				var color = InterpolateRGBa(progress,   0, RGB(0, 255, 0),
				                                      500, RGB(255, 255, 0),
				                                     1000, RGB(255, 0, 0));

				container->PlayerMessage(container->GetController(),"<c %x>{{Rock}}</c>", color); // FIXME: placeholder for graphics
			}
		}
		else
		{
			this.Target->~HandleTrail();
		}

		return FX_OK;
	},
};

/* --- Animations --- */

func Launch()
{
	if (Contained())
	{
		RemoveEffect("BlockGrenadeThrow", Contained());
		AddEffect("BlockGrenadeThrow", Contained(), 1, this.Grenade_ThrowDelay, Contained());
	}
	SetRDir(RandomX(-6, +6));
}

func DoLob(int angle)
{
	Exit();
	SetVelocity(angle, 20);
}

local ThrowingAnimation = new Effect
{
	Start = func (int temporary, object thrown, int angle, bool lobbed)
	{
		if (!temporary)
		{
			this.Throw_Time = 16;
			this.Throw_Object = thrown;
			this.Throw_Angle = angle;
			this.Throw_Lobbed = lobbed;
			this.Target->PlayAnimation("ThrowArms", CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, this.Target->GetAnimationLength("ThrowArms"), this.Throw_Time));
		}
	},

	Timer = func(int time)
	{
		if (this.Throw_Object)
		{
			if (time == this.Throw_Time * 8 / 15)
			{
				if (this.Throw_Lobbed)
				{
					this.Throw_Object->DoLob(this.Throw_Angle);
				}
				else
				{
					this.Target->DoThrow(this.Throw_Object, this.Throw_Angle);
				}
				this.Throw_Object->Launch();
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

