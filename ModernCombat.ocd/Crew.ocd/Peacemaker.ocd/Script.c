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
	AddCmcVirtualCursor(player);
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

func Grab(object target, bool grab)
{
	if (grab)
		Sound("Clonk::Action::Grab?");
	else
		Sound("Clonk::Action::UnGrab");
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


func AddCmcVirtualCursor(int player)
{
	if (GetPlayerType(player) == C4PT_User && CMC_Virtual_Cursor->Get(this) == nil)
	{
		CMC_Virtual_Cursor->AddTo(this);
	}
}


func UpdateCmcVirtualCursor(int x, int y)
{
	var cursor = CMC_Virtual_Cursor->Get(this);
	if (cursor)
	{
		cursor->UpdateAimPosition(x, y);
	}
}

/* --- Callback from aim manager --- */

public func StartAim(object weapon, int angle, string type)
{
	AddCmcVirtualCursor(GetOwner());
	return inherited(weapon, angle, type, ...);
}

/* --- Going prone --- */

public func GoProne()
{
	var prone_speed = 25;

	SetXDir(0);
	SetAction("GoProne");
	PlayAnimation("GoProne", CLONK_ANIM_SLOT_Movement, Anim_Linear(0, 0, GetAnimationLength("GoProne"), prone_speed, ANIM_Remove), Anim_Linear(0, 0, 1000, 5, ANIM_Remove));

	ScheduleCall(this, "EndGoProne", prone_speed);
}

func EndGoProne()
{
	if(GetAction() == "GoProne") SetAction("Crawl");
}

public func IsProne()
{
	return GetAction() == "Crawl";
}

/* --- Crawl --- */

func StartCrawl()
{
	if(!GetEffect("IntCrawl", this))
		CreateEffect(IntCrawl, 1, 1);
}

func StopCrawl()
{
	if(GetAction() != "Crawl") RemoveEffect("IntCrawl", this);
}

local IntCrawl = new Effect {
	Construction = func() {
		var position = Anim_X(0, 0, this.Target->GetAnimationLength("Crawl"), 11);
		var weight = Anim_Linear(0, 0, 1000, 5, ANIM_Remove);
		this.animation_id = this.Target->PlayAnimation("Crawl", CLONK_ANIM_SLOT_Movement, position, weight);
	},
	Timer = func() {
		// Test Waterlevel -- taken from IntWalk
		if(this.Target->InLiquid() && this.Target->GBackLiquid(0, -5) && !this.Target->Contained())
		{
			this.Target->SetAction("Swim");
			if(this.Target->GetComDir() == COMD_Left)
				this.Target->SetComDir(COMD_UpLeft);
			else if(this.Target->GetComDir() == COMD_Right)
				this.Target->SetComDir(COMD_UpRight);
			else if(this.Target->GetComDir() != COMD_Down && this.Target->GetComDir() != COMD_DownLeft && this.Target->GetComDir() != COMD_DownRight)
				this.Target->SetComDir(COMD_Up);
			return;
		}
	}
};

/* --- ActMap --- */

local ActMap = {
	Prototype = Clonk.ActMap,

	Crawl = {
		Prototype = Action,
		Name = "Crawl",
		Procedure = DFA_WALK,
		Accel = 2,
		Decel = 6,
		Speed = 25,
		Directions = 2,
		FlipDir = 0,
		Length = 1,
		Delay = 0,
		X = 0,
		Y = 0,
		Wdt = 8,
		Hgt = 20,
		StartCall = "StartCrawl",
		AbortCall = "StopCrawl",
	},
	GoProne = {
		Prototype = Action,
		Name = "GoProne",
		Procedure = DFA_KNEEL,
		Directions = 2,
		FlipDir = 0,
		Length = 1,
		Delay = 0,
		X = 0,
		Y = 0,
		Wdt = 8,
		Hgt = 20,
		InLiquidAction = "Swim",
		Sound = "Clonk::Movement::ProneFromWalking*",
	}
};

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local MaxEnergy = 140000;
