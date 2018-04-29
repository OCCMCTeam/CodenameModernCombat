/**
	Refillable use item
	
	When including this, be sure to call the inherited function in:
	- Initialize
*/

public func Initialize()
{
	SetStackRefillInterval(this->~GetStackRefillInterval());
	return _inherited(...);
}


public func SetStackRefillInterval(int interval)
{
	interval = interval ?? 35;
	var refill = GetEffect("RefillStackEffect", this) ?? CreateEffect(RefillStackEffect, 1, interval);
	if (refill)
	{
		refill.Interval = interval;
		return true;
	}
	return false;
}


// Refill stackable items!
local RefillStackEffect = new Effect
{
	Timer = func ()
	{
		// Already filled?
		if (Target->IsFullStack())
		{
			return FX_OK;
		}
		
		// Do it!
		var user = Target->Contained();
		var can_refill = Target->~AllowStackRefill(user);
		if (user && can_refill)
		{
			Target->DoStackCount(1);
		}
		return FX_OK;
	},
};
