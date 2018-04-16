#include Library_Projectile

func Initialize()
{
	SetObjectBlitMode(GFX_BLIT_Additive);
}

/* --- Launching --- */

func OnLaunch()
{
	SetAction("Travel");
	// FIXME: No idea if we actually want bullets to glow :)
	// I just took this from the CR script
	SetLightColor(RGB(255, 190, 0));
	SetLightRange(50, 30);
}


func OnLaunched()
{
	CreateTrail(0, 0);
	if (trail) trail->SetGraphics("Red");
}

/* --- Effects --- */

/**
 Callback if the projectile hits another object.
 @par target This is the object that was hit.
 @par hitcheck_effect // FIXME: is currently not passed to this callback
 */
public func OnHitObject(object target, proplist hitcheck_effect)
{
	// No sound yet :'(
	
	// No blood yet, but we have sparks!
	// Blood should be created by the victim anyway
	CreateImpactEffect(3);
}


/**
 Callback if the projectile collides with the landscape.
 */
public func OnHitLandscape()
{
	// No sound yet :'(
	
	// Just some sparks
	CreateImpactEffect(3);
}

/* --- Display --- */


func ProjectileColor(int time)
{
	var progress = 100 * time / lifetime;
	var value = Max(0, 255 - progress * 2);
	
	return RGBa(255, value, value, value);
}

func TrailColor(int time)
{
	var progress = 100 * time / lifetime;
	var value = Max(0, 255 - progress * 2);
	
	return RGBa(255, value, value, value);
}

/* --- Properties --- */

local Name = "$Name$";

