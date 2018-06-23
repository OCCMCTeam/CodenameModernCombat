/**
	Library for grenades
	
	Represents basic grenade functionality.

	@author Marky
*/

/* --- Properties --- */

local grenade_detonated = false; // bool: already detonated? Important for preventing multiple explosions, etc.
local grenade_active = false;    // bool: active?
//local grenade_aiming = false;    // bool: aiming? Used for separating "ironsight" toggle from normal clonk aiming


local Grenade_ContainedDamage = 60;
local Grenade_FuseTime = 105; // 3 seconds
local Grenade_MaxDamage = 10; // Take this many damage and it activates itself


func NoDecoDamage() { return true; } // Forgot what this does, but lets leave it in there for now
func IsBouncy()     { return true; } // Gets launched by jump pad
func IsGrenade()    { return true; } // Identification by the grenade belt
func IsActive()     { return grenade_active; }

/* --- Engine callbacks --- */

func Hit(int xdir, int ydir)
{
	_inherited(xdir, ydir, ...);
	if (this)
	{
		PlaySoundHit();
		if (GBackSolid( 0, +5)) return SetYDir(-ydir/26);
		if (GBackSolid( 0, -5)) return SetYDir(-ydir/26);
		if (GBackSolid(-5,  0)) return SetXDir(-xdir/16);
		if (GBackSolid(+5,  0)) return SetXDir(-xdir/16);
	}
}


func Damage(int change, int cause, int cause_plr)
{
	// Only do stuff if the object has the HitPoints property.
	var self = this;
	if (self && self.Grenade_MaxDamage != nil)
	{
		if (GetDamage() >= this.Grenade_MaxDamage)
		{
			self->~OnMaxDamage(change, cause, cause_plr);
		}
	}
	return _inherited(change, cause, cause_plr, ...);
}


/* --- Sounds --- */


func PlaySoundDetonation()
{
	Sound("Items::Grenades::Explosion?");
}


func PlaySoundHit()
{
	Sound("Items::Grenades::Hit?", {multiple = true});
}


/* --- Overloadable callbacks --- */

// What happens when the grenade explodes
public func OnDetonation()
{
	Explode(30, true, 60);
}

// If max damage is acquired
public func OnMaxDamage(int change, int cause, int cause_player)
{
	Detonate();
}

// How the trail is drawn
public func HandleTrail(int time)
{
	if (!Contained())
	{
		var speed = Abs(GetXDir()) + Abs(GetYDir());
		var alpha = Min(155, 55 + speed);
		
		var color = SplitRGBaValue(this.Grenade_SmokeColor ?? RGB(100, 100, 100));
		
		CreateParticle("Smoke",
		               -GetXDir() / 6, -GetYDir() / 6,
		                PV_Random(-10, 10), -5,
		                PV_Random(18, 36),
		               {
			               Prototype = Particles_Smoke(),
			               Size = PV_Linear(PV_Random(3, 5), PV_Random(10, 20)),
			               R = color.R, G = color.G, B = color.B, Alpha = alpha
		               });
	}
}


/* --- Explosion logic --- */

func HasDetonated()
{
	return grenade_detonated;
}

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
		if (container->GetAlive())
		{
			if (!container->Contained())
			{
				container->Fling(container->GetXDir()/10, container->GetYDir()/10 - 1);
			}
			container->DoEnergy(-this.Grenade_ContainedDamage, false, FX_Call_EngBlast, this->GetController()); // FIXME: may be replaced with custom damage system
		}
	}
	Exit();
}

/* --- Countdown until explosion --- */

func Fuse()
{
	if (!IsActive())
	{
		grenade_active = true;
		grenade_detonated = false;
		CreateEffect(GrenadeFuse, 200, 1);
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
		this.Target->~HandleTrail(time);
		return FX_OK;
	},
};
