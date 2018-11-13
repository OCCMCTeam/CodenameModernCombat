/**
	Booby Trap
*/

#include Library_Stackable
#include Library_ObjectLimit

/* --- Engine callbacks --- */

func Construction(object creator)
{
	SetOwner(NO_OWNER); // Circumvent object limit
	
	_inherited(creator, ...);
}

func Destruction()
{
	if (booby_trap_laser)
	{
		booby_trap_laser->RemoveObject();
	}
	_inherited(...);
}

func Entrance(object into)
{
	Defuse();
	_inherited(into, ...);
}

func Damage(int change)
{
	if (Contained() && OnFire())
	{
		Extinguish();
	}

	if (GetDamage() >= 10)
	{
		Detonate();
		return;
	}
	else if (GetDamage() >= 5)
	{
		SetObjectLayer(this);
		ScheduleCall(this, this.Detonate, 20);
	}
}

func Hit()
{
	Sound("Items::Tools::BoobyTrap::Hit?");
}

/* --- Control & Placement --- */

static const BOOBY_TRAP_PLACEMENT_Invalid = 0;
static const BOOBY_TRAP_PLACEMENT_Wall = 1;
static const BOOBY_TRAP_PLACEMENT_Background = 2;

public func HoldingEnabled() { return true; }

public func ControlUseStart(object user, int x, int y)
{
	var preview = GetBoobyTrapPreview();
	if (!preview)
	{
		preview = CreateEffect(IntBoobyTrapPreview, 1, 1, user);
	}
	return true;
}

public func RejectUse(object user)
{
	return !user->HasHandAction(false, false, true);
}


public func ControlUseHolding(object user, int x, int y)
{
	var preview = GetBoobyTrapPreview();
	if (preview) preview->Update(x, y);
	return true;
}

public func ControlUseStop(object user, int x, int y)
{
	var preview = GetBoobyTrapPreview();
	if (preview) preview->TryPlaceBoobyTrap(user, x, y);
	return true;
}

public func ControlUseCancel(object user, int x, int y)
{
	var preview = GetBoobyTrapPreview();
	if (preview) preview->CancelPlaceBoobyTrap(user, x, y);
	return true;
}

func GetBoobyTrapPreview()
{
	return GetEffect("IntBoobyTrapPreview", this);
}

local IntBoobyTrapPreview = new Effect
{
	Start = func (int temporary, object user)
	{
		if (temporary)
		{
			return;
		}
		this.booby_trap_preview = this.booby_trap_preview ?? CreateObject(Dummy);
		this.booby_trap_preview.Visibility = VIS_Owner;
		this.booby_trap_preview.Plane = 5000;
		this.booby_trap_preview->SetOwner(user->GetOwner());
		this.booby_trap_preview->SetGraphics(nil, Target->GetID(), 1, GFXOV_MODE_Object, nil, nil, Target);
		this.booby_trap_user = user;
		this.booby_trap_x = 0;
		this.booby_trap_y = 0;
		this.booby_trap_r = 0;
		this.booby_trap_placement = BOOBY_TRAP_PLACEMENT_Invalid;
		this.booby_trap_ok = false;
		this.booby_trap_progress = this.booby_trap_preview->CreateProgressBar(GUI_PieProgressBar, Target.BoobyTrapPlacementDelay, 0, nil, user->GetOwner(), {x = 0, y = 0}, VIS_Owner, { size = 250, color = RGBa(150, 150, 150, 50)});
	},

	Timer = func (int time)
	{
		if (this.booby_trap_preview)
		{
			if (this.booby_trap_user)
			{
				this.booby_trap_preview->SetPosition(this.booby_trap_user->GetX() + this.booby_trap_x, this.booby_trap_user->GetY() + this.booby_trap_y);
				Target->SetR(this.booby_trap_r);
			}

			if (this.booby_trap_progress && time <= Target.BoobyTrapPlacementDelay + 10)
			{
				this.booby_trap_progress->SetValue(time);
				this.booby_trap_progress->Update();
			}
			else
			{
				if (this.booby_trap_progress) this.booby_trap_progress->Close();
			}

			this.booby_trap_ok = time >= Target.BoobyTrapPlacementDelay && this.booby_trap_placement == BOOBY_TRAP_PLACEMENT_Wall;
			var color;
			if (this.booby_trap_ok)
			{
				color = RGBa(0, 255, 0, 128);
			}
			else
			{
				color = RGBa(255, 0, 0, 128);
			}
			this.booby_trap_preview->SetClrModulation(color, 1);
		}
		else
		{
			return FX_Execute_Kill;
		}
	},
	
	Update = func (int x, int y)
	{
		var angle = Angle(0, 0, x, y);
		var dist = Distance(0, 0, x, y);
		var bottom = Target->GetDefCoreVal("VertexY", "DefCore", 1);
		var pos = Target->GetWall(angle, dist, bottom);

		var p1 = Target->GetWall(angle - 5, dist + 2, bottom);
		var p2 = Target->GetWall(angle + 5, dist + 2, bottom);

		var r;
		if (p1 && p2)
		{
			r = Angle(p1.X, p1.Y, p2.X, p2.Y) + 90;
		}
		
		this.booby_trap_x = pos.X;
		this.booby_trap_y = pos.Y;
		this.booby_trap_r = r;
		this.booby_trap_placement = pos.Placement;
	},
	
	TryPlaceBoobyTrap = func (object user, int x, int y)
	{
		// Place it
		if (this.booby_trap_ok)
		{
			var trap = Target->TakeObject();
			trap->Exit();
			trap->SetPosition(this.booby_trap_user->GetX() + this.booby_trap_x, this.booby_trap_user->GetY() + this.booby_trap_y);
			trap->PlaceBoobyTrap(user);
			trap->SetR(this.booby_trap_r);
		}
		
		// Cancel / Remove preview on positive placement
		this->CancelPlaceBoobyTrap(user, x, y);
	},
	
	CancelPlaceBoobyTrap = func (object user, int x, int y)
	{
		RemoveEffect(nil, Target, this);
	},
	
	Destruction = func ()
	{
		if (this.booby_trap_preview) this.booby_trap_preview->RemoveObject();
		if (this.booby_trap_progress) this.booby_trap_progress->Close();
	},
};


func GetWall(int angle, int max_dist, int dist_bottom)
{
	// Initialize with defaults
	max_dist = Min(max_dist ?? this.BoobyTrapPlacementMaxDist, this.BoobyTrapPlacementMaxDist);
	var place_x = +Sin(angle, max_dist);
	var place_y = -Cos(angle, max_dist);

	var solid = false;

	// Hit a wall?
	for (var dist = this.BoobyTrapPlacementMinDist; dist <= max_dist; dist++)
	{
		var x = +Sin(angle, dist);
		var y = -Cos(angle, dist);
		var x_bottom = +Sin(angle, dist + dist_bottom);
		var y_bottom = -Cos(angle, dist + dist_bottom);
		if (GBackSolid(x_bottom, y_bottom))
		{
			place_x = x;
			place_y = y;
			solid = true;
			break;
		}
	}
	
	var placement = BOOBY_TRAP_PLACEMENT_Invalid;
	
	if (!solid)
	{
		if (GetMaterial(place_x, place_y) != Material("Sky") // Not a wall, but not sky?
		&& !GBackSemiSolid(place_x, place_y)) // Never place in liquids!
		{
			placement = BOOBY_TRAP_PLACEMENT_Background; // Must be a background wall
		}
	}
	else
	{
		placement = BOOBY_TRAP_PLACEMENT_Wall;
	}
	
	// Save everything as a proplist :)
	return {
		X = place_x,
		Y = place_y,
		Placement = placement,
	};
}


/* --- Laser Beam --- */

func StartLaser()
{
	// Create a laser effect that is used for drawing the laser line
	booby_trap_laser = CreateObject(LaserEffect, 0, 0, NO_OWNER);
	booby_trap_laser->SetWidth(2)->Color(RGB(255, 0, 0));
	booby_trap_laser->Activate();
	// Start a hit check
	ScheduleCall(this, this.CheckLaser, 1);
}

func CheckLaser()
{
	if (booby_trap_triggered || GetAction() != "Active") return;
	
	var self = this;

	// Create a projectile that checks whether it hits objects or the landscape
	var beam = CreateObject(CMC_Projectile_LaserBeam, 0, 0, GetController());
	beam->SetPosition(GetX() + GetVertex(0, 0), GetY() + GetVertex(0, 1));

	beam->Shooter(booby_trap_user)
		->Weapon(GetID())
		->DamageAmount(0)
 	    ->Range(200)
	    ->HitScan();

	// Override the projectile functions, so that it calls functions in the mine
	beam.OnHitObject = GetID().LaserOnHitObject;
	beam.OnLandscape = GetID().LaserOnHitLandscape;
	beam.OnHitScan = GetID().LaserOnHitScan;
	beam.booby_trap = this;

	// Launch the projectile
	beam->Launch(GetR());

	if (self && !booby_trap_triggered) // Booby trap could be removed at this point
	{
		// Start another hit check?
		ScheduleCall(self, self.CheckLaser, 1);
	}
}

// Added to the laser projectile
func LaserOnHitObject(object target)
{
	if (this.booby_trap)
	{
		this.booby_trap->LaserHit(target);
	}
	else
	{
		return RemoveObject();
	}
}

// Added to the laser projectile
func LaserOnHitLandscape()
{
	// Do nothing
}

// Added to the laser projectile
func LaserOnHitScan(int x_start, int y_start, int x_end, int y_end)
{
	if (this.booby_trap)
	{
		this.booby_trap->LaserLine(x_start, y_start, x_end, y_end);
	}
	RemoveObject();
}

func LaserLine(int x_start, int y_start, int x_end, int y_end)
{
	if (booby_trap_laser)
	{
		booby_trap_laser->Line(x_start, y_start, x_end, y_end)->Update();
	}
}

func LaserHit(object target)
{
	// Do nothing unless activated
	if (GetAction() == "Active")
	{
		if (booby_trap_triggered) return;

		// Ignore targets that do not move
		var xdir = target->GetXDir();
		var ydir = target->GetYDir();
		if (xdir == 0 && ydir == 0) return;
	
		// ... and done!
		Detonate();
	}
}

/* --- Functionality --- */

func PlaceBoobyTrap(object user)
{
	var player = user->GetOwner();
	SetOwner(player);
	EvaluateObjectLimit(player);

	this.booby_trap_user = user;

	SetAction("Activate");
	return true;
}

func Defuse()
{
	SetAction("Idle");
	this.booby_trap_user = nil;
	if (this.booby_trap_laser)
	{
		this.booby_trap_laser->RemoveObject();
	}
	SetClrModulation();
}


func Warning()
{
	CreateLEDEffect(GetPlayerColor(GetController()), GetVertex(0, 0), GetVertex(0, 1), 6);
}

func OnActive()
{
	SetClrModulation(RGBa(255, 255, 255, 55));
	StartLaser();
}


func Detonate()
{
	if (booby_trap_triggered) return;
	booby_trap_triggered = true;
	
	if (GetAction() == "Active")
	{
		var spread = 15;
		for (var amount = 12; amount > 0; --amount)
		{
			var shrapnel = CreateObject(Shrapnel, 0, 0, NO_OWNER);
			shrapnel->SetVelocity(GetR() + RandomX(-spread, +spread), RandomX(100, 180));
			shrapnel->SetRDir(RandomX(-30, +30));
			shrapnel->Launch(GetController());
			shrapnel.ProjectileDamage = this.ShrapnelDamage;
			CreateObject(BulletTrail)->Set(shrapnel, 2, 30);
		}
		ExplosionEffect(10);
	}
	else
	{
	
		ExplosionEffect(5);
	}
	RemoveObject();
}


func ShrapnelDamage(){ return 50; }


/**
 * Takes one object from the stack, the
 * stack count is reduced by 1.
 * @return the object that was taken.
 *         This object is not contained.
 */
public func TakeObject()
{
	// Create a new object always, because item limitation removes the oldest objects first
	var take = CreateObject(GetID(), 0, 0, NO_OWNER);
	take->SetStackCount(1);
	if (GetStackCount() <= 1)
	{
		RemoveObject();
	}
	else
	{
		DoStackCount(-1);
	}
	return take;
}


/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ObjectLimitPlayer = 2;

local booby_trap_triggered;
local booby_trap_user;
local booby_trap_laser;

local BoobyTrapPlacementDelay = 30;   // hold 'use' this many frames before the booby_trap can be used
local BoobyTrapPlacementMinDist = 5;  // booby trap must be placed at least this far from the clonk
local BoobyTrapPlacementMaxDist = 20; // booby trap must be placed at most this far from the clonk
local BoobyTrapExplosionDelay = 10;   // explode this many frames after triggered
local BoobyTrapExplosionRadius = 30;  // look at this radius around the trap

public func MaxStackCount() { return 3; }
public func InitialStackCount() { return 1; }

func IsProjectileTarget(object projectile, object shooter)
{
	// Do not hit self
	if (projectile && projectile.booby_trap == this) return false;

	return (projectile && projectile->~IsTracer()) // Get hit by tracers always
	    || (!Random(6));                           // other projectiles hit less often
}

/* --- Actions --- */

local ActMap = {
Activate = {
	Prototype = Action,
	Name = "Activate",
	Procedure = DFA_NONE,
	Length = 1,
	Delay = 72,
	FacetBase = 1,
	NextAction = "Active",
	EndCall = "OnActive",
},

Active = {
	Prototype = Action,
	Name = "Active",
	Procedure = DFA_NONE,
	Length = 4,
	Delay = 5,
	FacetBase = 1,
	NextAction = "Active",
	EndCall = "Warning",
},
};
