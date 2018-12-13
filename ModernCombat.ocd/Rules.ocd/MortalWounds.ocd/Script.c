/**
	Prevent instant death.

	@author Marky
*/

#include Library_Singleton

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Visibility = VIS_Editor;
local EditorPlacementLimit = 1; // Rules are to be placed only once

/* --- Engine Callbacks --- */

// Message window if rule is selected in the player menu
func Activate(int for_player)
{
	MessageWindow(this.Description, for_player);
	return true;
}

func Construction(object creator)
{
	_inherited(creator, ...);

	SetDelayedDeath(false); // Creating the rule disables delayed death
}

func Destruction()
{
	SetDelayedDeath(true); // Destroying it resotres delayed death for everyone
}

/* --- Interface --- */


// When crew is being recruited - non recruited crew may die normally :D
public func SetDelayedDeath(bool enable, object crew)
{
	if (nil == crew) // Set for all?
	{
		for (var crew in FindObjects(Find_OCF(OCF_CrewMember), Find_Func("IsClonk")))
		{
			if (crew) SetDelayedDeath(enable, crew);
		}
	}
	else // specific
	{
		var check = GetEffect("RuleMortalWoundsCheck", crew);
		if (enable && !check)
		{
			crew->CreateEffect(RuleMortalWoundsCheck, 1, 1); // Does not need to be top priority, let other effects modify damage first, and so on
		}
		if (!enable && check)
		{
			RemoveEffect(nil, crew, check);
		}
	}
}

public func GetDelayedDeathEffect(object target)
{
	return GetEffect("RuleMortalWoundsCheck", target);
}

/* --- Functionality --- */

local RuleMortalWoundsCheck = new Effect
{
	Name = "RuleMortalWoundsCheck",
	FramesPerSecond = 35,
	TimerMax = 15 * 35,
	TimerMin = 5 * 35,

	Start = func (int temp)
	{
		if (!temp)
		{
			this.Target.IsIncapacitated = CMC_Rule_MortalWounds.IsIncapacitated;
			this.Target.DoReanimate = CMC_Rule_MortalWounds.DoReanimate;
			this.Target.GetAlive = CMC_Rule_MortalWounds.GetAlive;
			this.Target.GetEnergy = CMC_Rule_MortalWounds.GetEnergy;
			this.Target.GetOCF = CMC_Rule_MortalWounds.GetOCF;
			this.is_incapacitated = false;
			this.death_timer = this.TimerMax;
		}
		return FX_OK;
	},

	Timer = func ()
	{
		var change = +1;
		var overlay = GetScreenOverlay();
		// Already wounded?
		if (IsIncapacitated())
		{
			if (this.death_timer <= 1) // Would reduce to 0 at the end of the function, but skip another timer call
			{
				this.Target->Kill(); // TODO: This will very likely cause problems with kill tracing at the moment
				return FX_Execute_Kill;
			}
			change = -1;

			// Some effects
			if (this.death_timer % this.FramesPerSecond == 0)
			{
				this.Target->~PlaySoundHeartbeat();
			}
			if (overlay)
			{
				var alpha = InterpolateLinear(this.death_timer, 0, 230, this.TimerMax, 0);
				overlay->Update({BackgroundColor = RGBa(1, 1, 1, alpha)});
			}

			// Update the menu
			if (this.Target->~GetIncapacitatedMenu())
			{
				this.Target->GetIncapacitatedMenu()->OnTimeRemaining(this.death_timer);
			}
		}
		else
		{
			if (overlay)
			{
				overlay->Update({BackgroundColor = nil});
			}
		}

		this.death_timer = BoundBy(this.death_timer + change, 0, this.TimerMax);
	},

	Damage = func (int health_change, int cause, int by_player)
	{
		// Already wounded?
		if (IsIncapacitated())
		{
			// Do not die, but do not heal either
			// This is important, because healing effects stay active even if the target is not "alive"!
			// Healing effects should, however, not boost the victim while they are incapacitated.
			// Normally I'd overload the healing effect, but preventing the healing here is easier.  
			return 0;
		}
		// In case the victim would die
		else if (health_change < 0 && (health_change + 1000 * this.Target->GetEnergy() <= 0))
		{
			var stay_at_one_health = (1 - this.Target->GetEnergy()) * 1000;
			this.is_incapacitated = true;
			this.death_timer = Max(this.TimerMin, this.death_timer);
			this.Target->~OnIncapacitated(health_change, cause, by_player);
			if (this.Target->~GetHUDController()) // Update the HUD; Used OnCrewDisabled callback, because defining a new one is not worth the effort
			{
				this.Target->GetHUDController()->OnCrewDisabled(this.Target);
			}
			return stay_at_one_health;
		}
		return health_change;
	},

	// Status

	IsIncapacitated = func ()
	{
		return this.is_incapacitated;
	},

	DoReanimate = func (int by_player)
	{
		if (IsIncapacitated())
		{	
			this.is_incapacitated = false;
			this.Target->~OnReanimated(by_player);
			if (this.Target->~GetHUDController()) // Update the HUD; Used OnCrewEnabled callback, because defining a new one is not worth the effort
			{
				this.Target->GetHUDController()->OnCrewEnabled(this.Target);
			}
			return true;
		}
		return false;
	},

	// Effects
	GetScreenOverlay = func ()
	{
		if (this.Target->~GetHUDController())
		{
			return this.Target->GetHUDController()->~GetColorLayer(this.Target, Format("%v", CMC_Rule_MortalWounds));
		}
	},

};

/* --- Functions, are being added to clonks if the rule is active --- */

func IsIncapacitated()
{
	var fx = GetEffect("RuleMortalWoundsCheck", this);
	return fx && fx->IsIncapacitated();
}

func DoReanimate(int by_player)
{
	var fx = GetEffect("RuleMortalWoundsCheck", this);
	return fx && fx->DoReanimate(by_player);
}

func GetAlive()
{
	if (this->~IsIncapacitated())
	{
		return false;
	}
	else
	{
		// Calling inherited calls the inherited function of the rule, instead of the object.
		// Using the ID is a reference seems fair enough, most objects do not overload
		// these functions anyway.
		return Call(this->GetID().GetAlive); 
	}
}

func GetOCF()
{
	// Calling inherited calls the inherited function of the rule, instead of the object.
	var ocf = Call(this->GetID().GetOCF);

	// Remove alive flag if incapacitated	
	if (this->~IsIncapacitated() && (ocf & OCF_Alive))
	{
		return ocf - OCF_Alive;
	}
	else
	{
		return ocf;
	}
}

func GetEnergy()
{	
	if (this->~IsIncapacitated())
	{
		return 0;
	}
	else
	{
		// Calling inherited calls the inherited function of the rule, instead of the object.
		return Call(this->GetID().GetEnergy);
	}
}
