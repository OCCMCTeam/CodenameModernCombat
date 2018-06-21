/**
	Defibrillator
*/

#include Library_AmmoManager
#include Library_CMC_Pack


/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;

func SelectionTime() { return 20; }


/* --- Engine callbacks --- */

func Initialize(...)
{
	SetGraphics(nil, DynamiteBox);
	this.PictureTransformation = Trans_Scale(); // Scale the picture of the box mesh, so that it does not appear in the game
	SetGraphics(nil, CMC_Tool_Defibrillator, 1, GFXOV_MODE_Picture);
	return _inherited(...);
}

func Selection(object container)
{
	if (container && container->~IsClonk())
	{
		PlaySoundDeploy();
		if (IsCharged())
		{
			ScheduleCall(this, this.PlaySoundBeep, SelectionTime());
		}
	}
	return _inherited(container, ...);
}

/* --- Display --- */

public func GetCarryMode(object user, bool not_selected)
{
	var can_hold = user->~IsWalking() || user->~IsJumping();
	if (can_hold && !not_selected)
	{
		return CARRY_BothHands;
	}
	else
	{
		return CARRY_Back;
	}
}


public func GetCarryTransform(object user, bool not_selected, bool nohand, bool second_on_back)
{
	if (not_selected)
	{
		if (second_on_back)
		{
			return Trans_Mul(Trans_Translate(-5000, 3000, 0), Trans_Rotate(-45, 0, 1));
		}
		else
		{
			return Trans_Mul(Trans_Translate(0, 3000, 00), Trans_Rotate(-45, 0, 1));
		}
	}
	if (nohand)
	{
		return Trans_Mul(Trans_Translate(0, -3000, -2200), Trans_Rotate(-45, 0, 1));
	}
}


public func GetCarryPhase()
{
	return 450;
}


/* --- User Interface --- */

local active_menu;


// Use holding controls?
public func HoldingEnabled()
{
	return true;
}

public func RejectUse(object user)
{
	return !user->HasActionProcedure(false) // The clonk must be able to use the hands
         || user->Contained();              // and not in a building or vehicle
}

func ControlUseStart(object user, int x, int y)
{
	if (IsCharged())
	{
		return true;
	}
	else
	{
		user->PlayerMessage(user->GetOwner(), "$NotCharged$", Library_PowerConsumer);
		return false;
	}
}

func ControlUseHolding(object user, int x, int y)
{
	DoShockSparks(GetShockPoint(x, y), 0, 1);
	return true;
}

func ControlUseStop(object user, int x, int y)
{
	ReleaseShock(user, x, y);
	return true;
}

func ControlUseCancel(object user, int x, int y)
{
	return true;
}


/* --- Functionality --- */

public func MaxAmmo() { return 30; }

public func AllowAmmoRefill(object user)
{
	return true;
}

/* --- Internals --- */

func ReleaseShock(object user, int x, int y)
{
	if (!IsCharged()) return;

	// TODO: Requires at least 10 ammo points
	var shock_point = GetShockPoint(x, y);
	var find_target = Find_Target(shock_point.x, shock_point.y, shock_point.radius);
	
	if (ShockAlly(user, find_target) || ShockEnemy(user, find_target))
	{
		PlaySoundShockTarget();
		DoAmmoCount(-20);
	}
	else
	{
		PlaySoundShockFail();
		DoAmmoCount(-10);
	}
	
	DoShockSparks(shock_point, 5, 10);
	
	// AddLightFlash(40+Random(20),0,0,RGB(0,140,255));
	ScheduleCall(this, this.PlaySoundBeep, SelectionTime());
}

func ShockAlly(object user, array find_target)
{
	var allies = FindObjects(find_target, Find_Allied(user->GetOwner()), Find_Func("IsIncapacitated"), Find_Not(Find_Func("RejectReanimation")));
	for (var ally in allies)
	{
		if (!ally) continue;
		
		// Reanimate with min energy
		ally->DoReanimate();
		ally->DoEnergy(Max(0, 30 - ally->GetEnergy()));
		
		// TODO: AddEffect("ShockPaddlesHeal",obj,20,1,0,GetID(),HealAmount(),HealRate());
		// Event message?
		// EventInfo4K(0,Format("$MsgReanimation$",GetTaggedPlayerName(GetOwner(caller)), GetTaggedPlayerName(GetOwner(obj))),IC04);
		// Achievement progress (Shock Therapist)
		// DoAchievementProgress(1, AC04, GetOwner(caller));
		// DoPlayerPoints(BonusPoints("Reanimation"), RWDS_TeamPoints, GetOwner(caller), caller, IC04);
		return true;
	}
	return false;
}

func ShockEnemy(object user, array find_target)
{
	var enemies = FindObjects(find_target, Find_Hostile(user->GetOwner()), Find_Func("GetAlive"));
	for (var enemy in enemies)
	{
		if (!enemy) continue;
		
		var strength = 10;
		
		// Fling enemy
		var precision = 1000;
		var angle = Angle(user->GetX(), user->GetY(), enemy->GetX(), enemy->GetY(), precision);
		
		enemy->Fling(+Sin(angle, strength, precision) / 5
		         -Abs(Cos(angle, strength, precision))/ 5);
		
		// In case the object is flung down a cliff
		if (user->GetOwner() != NO_OWNER)
		{
			enemy->SetKiller(user->GetOwner());
		}
		
		// TODO: Full damage is 30 to 40 at random
		enemy->DoEnergy(-strength, false, FX_Call_DmgFire, user->GetOwner());
		
		// Killed him? Incapacitated enemies do count as not alive, too :)
		if (!enemy->GetAlive())
		{
			// "Don't tase me Bro"
			// TODO: DoAchievementProgress(1, AC05, GetOwner(GetUser()));
		}
		return true;
	}
	return false;
}

func Find_Target(int x, int y, int radius)
{
	return Find_And(Find_NoContainer(), Find_Distance(radius, x, y));
}

func GetShockPoint(int x, int y)
{
	var radius = 9;
	var precision = 100;
	var angle = Angle(0, 0, x, y, precision);
	var point_x = +Sin(angle, radius, precision);
	var point_y = -Cos(angle, radius, precision);
	return {x = point_x, y = point_y, radius = radius, angle = angle};
}

func DoShockSparks(proplist shock_point, int a, int b)
{
	var dir_x_min = Cos(shock_point.angle, -5, shock_point.precision) - Sin(shock_point.angle, -5, shock_point.precision);
	var dir_x_max = Cos(shock_point.angle, +5, shock_point.precision) - Sin(shock_point.angle, -1, shock_point.precision);
	var dir_y_min = Sin(shock_point.angle, -5, shock_point.precision) + Cos(shock_point.angle, -5, shock_point.precision);
	var dir_y_max = Sin(shock_point.angle, +5, shock_point.precision) + Cos(shock_point.angle, -1, shock_point.precision);
	
	if (a > 0)
	{
		CreateParticle("ElectroSpark", PV_Random(shock_point.x - 3, shock_point.x + 3), PV_Random(shock_point.y - 3, shock_point.y + 3), 
		                               PV_Random(dir_x_min, dir_x_max), PV_Random(dir_y_min, dir_y_max), 
	                                   10, Particles_Colored(Particles_ElectroSpark1(), RGB(250, 150,   0)), a);
    }
    if (b > 0)
    {                              
		CreateParticle("ElectroSpark", PV_Random(shock_point.x - 3, shock_point.x + 3), PV_Random(shock_point.y - 3, shock_point.y + 3), 
		                               PV_Random(dir_x_min, dir_x_max), PV_Random(dir_y_min, dir_y_max), 
	                                   10, Particles_Colored(Particles_ElectroSpark1(), RGB(100, 100, 250)), b);
    }
}

func IsCharged()
{
	return GetAmmoCount() >= 10;
}

func OnAmmoCountChange(int change)
{
	if (change > 0 && GetAmmoCount() == 10)
	{
		PlaySoundBeep();
	}
}

/* --- Sounds --- */


func PlaySoundDeploy()
{
	Sound("Items::Tools::Defibrillator::Deploy");
}

func PlaySoundBeep()
{
	Sound("Items::Tools::Defibrillator::Ready", {multiple = true});
}

func PlaySoundShockTarget()
{
	Sound("Items::Tools::Defibrillator::ShockTarget", {multiple = true});
}

func PlaySoundShockFail()
{
	Sound("Items::Tools::Defibrillator::Shock", {multiple = true});
}
