#include Library_Projectile
#include CMC_Library_Grenade

/* --- Properties --- */

local Grenade_StartX = 0;
local Grenade_StartY = 0;
local Grenade_SecureDistance = 75;

local Grenade_StartFrame = 0;
local Grenade_SecureDelay = 70;

func IsProjectileTarget()
{
	return !HasDetonated();
}


/* --- Callbacks from projectile --- */


func OnLaunch()
{
	SetAction("TravelBallistic");

	Grenade_StartX = GetX();
	Grenade_StartY = GetY();
	Grenade_StartFrame = FrameCounter();

	Fuse();
	StayOnHit();
}

public func DoDamageObject(object target)
{
	if (IsSecure())
	{
		target->DoEnergy(-20);
	}
	else
	{
		Detonate();
	}
}

func OnHitLandscape()
{
	if (IsSecure())
	{
		CreateImpactEffect(3);
	}
	else
	{
		Detonate();
	}
}

/* --- Functionality --- */

func IsSecure()
{
	if (IsActive())
	{
		return (Distance(Grenade_StartX, Grenade_StartY, GetX(), GetY()) <= Grenade_SecureDistance)
		    && (FrameCounter() - Grenade_StartFrame < Grenade_SecureDelay);
	}
	return true;
}
