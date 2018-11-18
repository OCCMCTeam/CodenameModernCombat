/**
	Syringe
*/

#include Library_ObjectLimit


/* --- Engine callbacks --- */

func RejectEntrance(object into)
{
	return into->ContentsCount(GetID()) >= 1;
}


func Selection(object container)
{
	if (container && container->~IsClonk())
	{
		PlaySoundDeploy();
	}
	return _inherited(container, ...);
}


/* --- Display --- */

// Callback from the item status HUD element: What should be displayed?
public func GetGuiItemStatusProperties(object user)
{
	var status = new GUI_Item_Status_Properties {};
	
	var configuration = Format("<c %x>%s</c>", GUI_CMC_Text_Color_Highlight, this->GetName());
	status->SetObjectConfiguration(configuration);

	return status;
}


/* --- User Interface --- */

func ControlUseItemConfig(object user, int x, int y, int status)
{
	if (status == CONS_Down)
	{
		return ControlUse(user, x, y);
	}
	else
	{
		return false;
	}
}

func ControlUse(object user, int x, int y)
{
	// User has to be ready to act
	if (user->~HasActionProcedure())
	{
		Sting(user, x, y);
	}
	return true;
}

func ControlUseAlt(object user, int x, int y)
{
	// User has to be ready to act
	if (user->~HasActionProcedure())
	{
		UseOn(user, user);
	}
	return true;
}

/* --- Functionality --- */

func Sting(object user, int x, int y)
{
	// Find targets, heal most hurt preferrably
	var sting_point = GetStingPoint(x, y);
	var targets = FindObjects(Find_Distance(sting_point.radius, sting_point.x, sting_point.y), Find_Exclude(user), Find_Allied(user->GetOwner()), Find_NoContainer(), Find_Func("GetAlive"), Sort_Func("GetEnergy"));
	
	// Heal the target?
	var rejected;
	for (var target in targets)
	{
		rejected = RejectHealing(target);
		if (rejected)
		{
			continue;
		}

		return UseOn(user, target);
	}
	
	// No effect? The message only shows the reason for the last clonk that was rejected
	PlaySoundIneffective();
	if (rejected)
	{
		user->PlayerMessage(user->GetOwner(), rejected);
	}
}

func GetStingPoint(int x, int y)
{
	var radius = 9;
	var precision = 100;
	var angle = Angle(0, 0, x, y, precision);
	var point_x = +Sin(angle, radius, precision);
	var point_y = -Cos(angle, radius, precision);
	return {x = point_x, y = point_y, radius = radius, angle = angle};
}

func RejectHealing(object target)
{
	if (target->GetEnergy() >= target->GetMaxEnergy())
	{
		return "$NotWounded$";
	}
	if (GetEffect("*Heal*", target))
	{
		return "$AlreadyHealing$";
	}
	return nil;
}

func UseOn(object user, object target)
{
	var reject = RejectHealing(target);
	if (reject)
	{
		user->PlayerMessage(user->GetOwner(), reject);
	}
	else if (target->Heal(this.HealAmount, this.HealInterval, true, true, GetID()->DefineCallback(this.HealEffect)))
	{
		PlaySoundInject(target);
		target->CreateEffect(FxFlashScreenRGBa, 200, 1, this.HealEffectLayer, this.HealEffectColor, 180, 35);
  
	    if ((user != target)
	     && (!Hostile(user->GetOwner(), target->GetOwner())))
	    {
			// Points for achievement system
			// TODO DoPlayerPoints(BonusPoints("Healing", this.HealAmount), RWDS_TeamPoints, user->GetOwner(), user, IC05);
			// Achievement progress (I'll fix you up!)
			// TODO DoAchievementProgress(BonusPoints("Dragnin"), AC02, user->GetOwner());
	    }
		RemoveObject();
	}
}

func HealEffect(object target, int time)
{
	var size = target->GetObjHeight() + 5 + Sin(time * 5, 7);
	var color = SplitRGBaValue(this.HealEffectColor);

	target->CreateParticle("Shockwave", 0, 0, PV_Random(-5, 5), PV_Random(-5, -10), 30,
	{
        Size = PV_Linear(size, 0),
        R = color.R,
        G = color.G,
        B = color.B,
		Rotation = PV_Linear(PV_Random(-180, 180), PV_Random(-180, 180)),
		BlitMode = GFX_BLIT_Additive,
        Alpha = PV_Linear(50, 0),
	}, 1);
	
	if(0 == (time % 18))
	{
		target->CreateEffect(FxFlashScreenRGBa, 200, 1, this.HealEffectLayer, this.HealEffectColor, 80, 30);
	}
}

/* --- Sounds --- */

func PlaySoundDeploy()
{
	Sound("Items::Tools::Syringe::Deploy");
}

func PlaySoundInject(object target)
{
	target->Sound("Items::Tools::Syringe::Inject");
}

func PlaySoundIneffective()
{
	Sound("Items::Grenades::Shared::Throw?");
}

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;

local ObjectLimitPlayer = 3;

// Healing effect
local HealAmount = 40;
local HealInterval = 2;
local HealEffectColor = 0xff00e6ff; // 0, 230, 255
local HealEffectLayer = "Medicament";
