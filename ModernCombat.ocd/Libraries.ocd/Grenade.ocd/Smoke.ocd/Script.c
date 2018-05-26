/**
	Include this in objects that are affected by smoke grenades.
	
	The default implementation is suited for Clonks, but can be expanded or overriden
	for vehicles, structures, etc.

	@author Marky
 */

func IsAffectedBySmokeGrenade()
{
	return !Contained()
	    && GetAlive();
}

func BlindedBySmokeGrenade(object grenade)
{
	var HUD = this->~GetHUDController();
	if (HUD && GetType(HUD.GetColorLayer) == C4V_Function && !GetEffect("BlindedBySmoke", this))
	{
		CreateEffect(BlindedBySmoke, 1, 1, grenade->GetController());
	}
}

local BlindedBySmoke = new Effect 
{
	Start = func (int temp, int by_player)
	{
		if (!temp)
		{
			this.ColorLayer = Format("%v", CMC_Grenade_Smoke);
			this.AlphaMax = 250;
			this.Alpha = 0;
		}
	},

	Timer = func (int time)
	{
		var overlay = this.Target->GetHUDController()->GetColorLayer(this.Target, this.ColorLayer);

		var change_alpha = -10;
		var blinded = ObjectCount(Find_ID(CMC_Grenade_SmokeHelper), Find_Func("CanAffect", this.Target)) > 0;
		if (blinded)
		{
			change_alpha = +10;
		}
		this.Alpha = BoundBy(this.Alpha + change_alpha, 0, this.AlphaMax);

		overlay->Update({BackgroundColor = RGBa(150, 150, 150, this.Alpha)});
		
		if (this.Alpha == 0)
		{
			return FX_Execute_Kill;
		}
	},

	Stop = func (int temp)
	{
		if (!temp && this.Target)
		{
			this.Target->GetHUDController()->GetColorLayer(this.Target, this.ColorLayer)->Update({BackgroundColor = nil});
		}
	},
};

