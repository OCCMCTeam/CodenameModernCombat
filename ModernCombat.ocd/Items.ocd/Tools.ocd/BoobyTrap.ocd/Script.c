/**
	Booby Trap
*/

#include Library_ObjectLimit

/* --- Engine callbacks --- */

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

public func RejectUse(object clonk)
{
	return !clonk->HasHandAction(false, false, true);
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
	if (preview) preview->TryActivateBoobyTrap(user, x, y);
	return true;
}

public func ControlUseCancel(object user, int x, int y)
{
	var preview = GetBoobyTrapPreview();
	if (preview) preview->CancelActivateBoobyTrap(user, x, y);
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
		this.booby_trap_progress = this.booby_trap_preview->CreateProgressBar(GUI_PieProgressBar, Target->BoobyTrapPlacementDelay(), 0, nil, user->GetOwner(), {x = 0, y = 0}, VIS_Owner, { size = 250, color = RGBa(150, 150, 150, 50)});
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

			if (this.booby_trap_progress && time <= Target->BoobyTrapPlacementDelay() + 10)
			{
				this.booby_trap_progress->SetValue(time);
				this.booby_trap_progress->Update();
			}
			else
			{
				if (this.booby_trap_progress) this.booby_trap_progress->Close();
			}

			this.booby_trap_ok = time >= Target->BoobyTrapPlacementDelay() && this.booby_trap_placement == BOOBY_TRAP_PLACEMENT_Wall;
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
	
	TryActivateBoobyTrap = func (object user, int x, int y)
	{
		if (this.booby_trap_ok)
		{
			Target->Exit();
			Target->SetPosition(this.booby_trap_user->GetX() + this.booby_trap_x, this.booby_trap_user->GetY() + this.booby_trap_y);
			Target->SetR(this.booby_trap_r);
			Target->ActivateBoobyTrap(user);
		}
		else // cancel
		{
			this->CancelActivateBoobyTrap(user, x, y);
		}
	},
	
	CancelActivateBoobyTrap = func (object user, int x, int y)
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
	// initialize with defaults
	max_dist = Min(max_dist ?? BoobyTrapPlacementMaxDist(), BoobyTrapPlacementMaxDist());
	var place_x = +Sin(angle, max_dist);
	var place_y = -Cos(angle, max_dist);

	var solid = false;

	// hit a wall?
	for (var dist = BoobyTrapPlacementMinDist(); dist <= max_dist; dist++)
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
		if (GetMaterial(place_x, place_y) != Material("Sky") // not a wall, but not sky?
		&& !GBackSemiSolid(place_x, place_y)) // never place in liquids!
		{
			placement = BOOBY_TRAP_PLACEMENT_Background; // must be a background wall
		}
	}
	else
	{
		placement = BOOBY_TRAP_PLACEMENT_Wall;
	}
	
	// save everything as a proplist :)
	return {
		X = place_x,
		Y = place_y,
		Placement = placement,
	};
}


/* --- Functionality --- */

func ActivateBoobyTrap(object user)
{
	this.Collectible = false;
	SetOwner(user->GetOwner());
	SetAction("Activate");
	return true;
}


func Warning()
{	
	var a = 190;
	if (GetAction() == "Active")
		a = 95 + 120 * GetDamage() / MaxDamage();
	else
		Sound("Weapon::Bip");

	CreateLEDEffect(RGBa(255, 64, 0, a), Sin(GetR(), WarningDist()), -Cos(GetR(), WarningDist()), 6); 
}

func OnActive()
{
	SetClrModulation(RGBa(255, 255, 255, 55));
}

func Check()
{
	/*var obj;
	for (obj in FindProjectileTargets(BoobyTrapExplosionRadius(), this, this)) 
	
	{
		// only moving objects
		if (obj->GetXDir() || obj->GetYDir() || obj->GetAction() == "ScaleLadder")
		{
			// except booby_traps
			if (!(obj->~IsBoobyTrap()))
			{
				DoDamage(MaxDamage()); // trigger!
				return;
			}
		}
	}*/
}


func Damage(int change)
{
	if (GetAction() != "Idle")
		SetClrModulation(RGBa(255, 255, 255, 55 + 200 * GetDamage() / MaxDamage()));
	if (GetDamage() >= MaxDamage())
		Trigger();
}

func Trigger()
{
	if (triggered) return;
	triggered = true;
	Sound("Weapon::BipBipBip");
	ScheduleCall(this, this.Triggered, BoobyTrapExplosionDelay());
}

func Triggered()
{
	Explode(45);
}

func RejectEntrance()
{
	if (GetAction() != "Idle") return true;
	return false;
}


/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ObjectLimitPlayer = 2;

local triggered;

func MaxDamage(){	return 30;}
func BoobyTrapExplosionRadius(){	return 30;}
func BoobyTrapExplosionDelay(){	return 10;}
func BoobyTrapPlacementDelay(){ return 30;}   // hold 'use' this many frames before the booby_trap can be used
func BoobyTrapPlacementMinDist(){ return 5;}  // booby trap must be placed at least this far from the clonk
func BoobyTrapPlacementMaxDist(){ return 20;} // booby trap must be placed at most this far from the clonk
func WarningDist(){ return 3;}
func IsProjectileTarget(){	return true;}

func IsEquipment(){	return true;}
func IsBoobyTrap(){	return true;}

/* --- Actions --- */

local ActMap = {
Activate = {
	Prototype = Action,
	Name = "Activate",
	Procedure = DFA_NONE,
	Length = 5,
	Delay = 20,
	FacetBase = 1,
	NextAction = "Active",
	EndCall = "OnActive",
	PhaseCall = "Warning",
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
	PhaseCall = "Check",
},

};
