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
		Sting(user);
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

func Sting(object user)
{
	// Not implemented yet
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
		PlayerMessage(user->GetOwner(), reject);
	}
	else if (target->Heal(this.HealAmount, this.HealInterval, true, true, GetID()->DefineCallback(this.HealEffect)))
	{
		PlaySoundInject();
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

func PlaySoundInject()
{
	Sound("Items::Tools::Syringe::Inject");
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
