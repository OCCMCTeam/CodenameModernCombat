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


// When crew is being recruited - non recruited crew may die normally :D
public func OnClonkRecruitment(object crew, int player)
{
	var check = GetEffect("RuleMortalWoundsCheck", crew);
	if (!check)
	{
		crew->CreateEffect(RuleMortalWoundsCheck, 1, 1); // Does not need to be top priority, let other effects modify damage first, and so on
	}
}

/* --- Functionality --- */

local RuleMortalWoundsCheck = new Effect
{
	Name = "RuleMortalWoundsCheck",
	
	Start = func (int temp)
	{
		if (!temp)
		{
			Log("Modded functions!");
			this.Target.IsIncapacitated = CMC_Rule_MortalWounds.IsIncapacitated;
			this.Target.GetAlive = CMC_Rule_MortalWounds.GetAlive;
		}
		return FX_OK;
	},
	
	Damage = func (int health_change, int cause, int by_player)
	{
		// Already wounded?
		if (IsIncapacitated())
		{
			// Do not die if the change would kill you
			if (health_change < 0)
			{
				return 0;
			}
			else // Healing is allowed
			{
				return health_change;
			}
		}
		// In case the victim would die
		else if (health_change < 0 && (health_change + 1000 * this.Target->GetEnergy() <= 0))
		{
			Log("Target got incapacitated");
			var stay_at_one_health = (1 - this.Target->GetEnergy()) * 1000;
			this.is_incapacitated = true;
			return stay_at_one_health;
		}
		Log("Health change: %d", health_change);
		return health_change;
	},
	
	// Status
	
	IsIncapacitated = func ()
	{
		return this.is_incapacitated;
	},
	
};

/* --- Functions, are being added to clonks if the rule is active --- */

func IsIncapacitated()
{
	var fx = GetEffect("RuleMortalWoundsCheck", this);
	return fx && fx->IsIncapacitated();
}

func GetAlive()
{
	Log("Mortal wounds get alive");
	if (this->~IsIncapacitated())
	{
		return false;
	}
	else
	{
		return Call(this->GetID().GetAlive);
	}
}
