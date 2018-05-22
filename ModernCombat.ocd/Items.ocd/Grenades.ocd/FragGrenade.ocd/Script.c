#include CMC_Library_Grenade

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local ContactIncinerate = 1;
local NoBurnDecay = 1;
local Collectible = true;

local Grenade_ContainedDamage = 80;
local Grenade_SmokeColor = 0xffffff00;

/* --- Callbacks --- */

// What happens when the grenade explodes
public func OnDetonation()
{
	// Fragments
	var level = 30;
	DoShockwave(GetX(), GetY(), level, GetController(), GetObjectLayer());
	ExplosionEffect(level, 0, 0, 0, true, level);
	
	var r = Angle(GetXDir(), GetYDir());
	var speed = BoundBy(Distance(GetXDir(), GetYDir()), 0, 80);
	for (var i = 40; i > 0; --i)
	{
		var angle = InterpolateLinear(speed, 0, RandomX(-180, 180), 80, r);
		
		var frag = CreateObject(CMC_Projectile_Bullet); // FIXME: Lazy again :) But seriously, the fragment code is loooong
		frag->Velocity(RandomX(70, 100))
		    ->Range(RandomX(200, 300))
		    // size = 5
		    // trail = 15
		    ->DamageAmount(20)
		    ->Shooter(this);
		frag->Launch(angle);
	}

	RemoveObject();
}
