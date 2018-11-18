/**
    Heal.c
    Function to heal livings over time.

    @author Armin, modified for CMC by Marky
*/

/**
	Heals the object over time for /amount/ HP.
	Calling the function multiple times results in faster healing (as opposed to longer healing).
	
	If necessary, a custom interval can be set. The healing effect restores 1 energy per interval.
	
	@par amount This much will be healed in total.
	@par interval Heals 1 energy every <interval> frames
	@par no_stacking No healing effect is added, if the target already has a *Heal* effect.
	@par cancel_on_damage Cancel the effect if damage is received?
	@par timer_callback Defines a custom callback that is called every <interval> frames,
	                    can be used for particle effects and the like.
	                    First parameter is the object that is healed, the second is the effect time.
*/
global func Heal(int amount, int interval, bool no_stacking, bool cancel_on_damage, array timer_callback)
{
	AssertObjectContext();

	if (no_stacking && GetEffect("*Heal*", this))
	{
		return nil;
	}
	else
	{
		// Add effect.
		var fx = CreateEffect(FxHealingOverTimeCmc, 1, interval ?? 36);
		fx.healing_amount = amount;
		fx.done = 0;
		fx.cancel_on_damage = cancel_on_damage;
		fx.timer_callback = timer_callback;
		return fx;
	}
}


static const FxHealingOverTimeCmc = new Effect
{
	Timer = func (int time)
	{
		// Stop healing the Clonk if he reached full health. Additionally, if the object is dead (fake death).
		if (Target->GetEnergy() >= Target->GetMaxEnergy() || this.done >= this.healing_amount || !(Target->GetAlive()))
		{
			return FX_Execute_Kill;
		}
		Target->DoEnergy(1);
		++this.done;
		
		if (this.timer_callback)
		{
			DoCallback(this.timer_callback, Target, time);
		}
		return FX_OK;
	},

	Damage = func (int damage, int cause, int by_player)
	{
		// Stop healing, if the Clonk receives damage?
		if (this.cancel_on_damage && damage < 0)
		{
			RemoveEffect(nil, Target, this);
		}
		return damage;
	},
};
