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

// Charge counters - these count the progress when charging the item before releasing a shock
local PowerUpUse = 0;
local PowerUpMax = 30;

// Healing effect
local HealAmount = 70;
local HealInterval = 3;

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
	// Play sounds
	if (container && container->~IsClonk())
	{
		PlaySoundDeploy();
		if (IsCharged())
		{
			ScheduleCall(this, this.PlaySoundBeep, SelectionTime());
		}
	}

	// Reset the charge, just to be sure
	PowerUpUse = 0;
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
         || user->Contained()               // and not in a building or vehicle
         || GetEffect("IntCooldown", this);
}

func ControlUseStart(object user, int x, int y)
{
	// Reset powerup charge
	PowerUpUse = 0;

	// Handle the other things
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
	// Charge
	PowerUpUse = BoundBy(PowerUpUse + 1, 0, PowerUpMax);

	// Effects
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
	PowerUpUse = 0;
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

	var shock_point = GetShockPoint(x, y);
	var find_target = Find_Target(shock_point.x, shock_point.y, shock_point.radius);

	var ammo_cost = 10;
	if (ShockAlly(user, find_target) || ShockEnemy(user, find_target))
	{
		// Use 10 ammo if not charged, so that a short click costs more ammo
		// while a fully charged shock uses ammo most effectively;
		// added 5 points, so that charging for a few frames still costs 20 ammo total
		ammo_cost += ScalePowerUpInverse(10);
	}
	else
	{
		PlaySoundShockFail();
	}

	DoAmmoCount(-ammo_cost);	
	DoShockSparks(shock_point, 5, 10);

	// AddLightFlash(40+Random(20),0,0,RGB(0,140,255));

	// Cooldown and reset use; cooldown is inverted, so that a short powerup causes a long cooldown period
	var cooldown = Max(5, 35 - PowerUpUse);
	AddEffect("IntCooldown", this, 1, cooldown);
	PowerUpUse = 0;
	ScheduleCall(this, this.PlaySoundBeep, cooldown);
}

func ShockAlly(object user, array find_target)
{
	var allies = FindObjects(find_target, Find_Allied(user->GetOwner()), Find_Func("IsIncapacitated"), Find_Not(Find_Func("RejectReanimation")));
	for (var ally in allies)
	{
		if (!ally) continue;
		PlaySoundShockAlly();

		// Reanimate with min energy, add healing effect
		ally->DoReanimate();
		ally->DoEnergy(Max(0, 30 - ally->GetEnergy()));
		ally->Heal(this.HealAmount, this.HealInterval + ScalePowerUpInverse(Max(5, 25 - this.HealInterval)));

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
		PlaySoundShockEnemy();

		var strength = 10 + PowerUpUse;

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

		enemy->DoEnergy(-30, false, FX_Call_DmgFire, user->GetOwner());

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

func ScalePowerUpInverse(int value)
{
	return Max(5 + PowerUpMax - PowerUpUse, 0) * value / PowerUpMax;
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

func PlaySoundShockAlly()
{
	Sound("Items::Tools::Defibrillator::ShockPatient", {multiple = true});
}

func PlaySoundShockEnemy()
{
	Sound("Items::Tools::Defibrillator::ShockEnemy", {multiple = true});
}

func PlaySoundShockFail()
{
	Sound("Items::Tools::Defibrillator::Shock", {multiple = true});
}
