#include CMC_Library_GrenadeItem

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local ContactIncinerate = 1;
local NoBurnDecay = 1;
local Collectible = true;

local Grenade_ContainedDamage = 80;
local Grenade_SmokeColor = 0xffdddd00;

/* --- Callbacks --- */

// What happens when the grenade explodes
public func OnDetonation()
{
	// Fragments
	var level = 30;
	DoShockwave(GetX(), GetY(), level, GetController(), GetObjectLayer());
	ExplosionEffect(level, 0, 0, 0, true, level);

	// Determine free angles
	var angles = GetPathFreeAngles(level);
	var min_angle = Min(angles) ?? -90;
	var max_angle = Max(angles) ?? +90;
	var avg_angle = GetAverage(angles);

	// Determine cones
	var neg_angle = (min_angle + avg_angle) / 2;
	var pos_angle = (max_angle + avg_angle) / 2;

	// Cast shrapnel in 3 cones, preferrably sideways
	var shrapnel_count = 40;
	var spread = 5;
	LaunchShrapnel(min_angle, neg_angle, spread, 2 * shrapnel_count / 5);
	LaunchShrapnel(neg_angle, pos_angle, spread, 1 * shrapnel_count / 5);
	LaunchShrapnel(pos_angle, max_angle, spread, 2 * shrapnel_count / 5);

	RemoveObject();
}

func LaunchShrapnel(int min_angle, int max_angle, int spread, int amount)
{
	var min = Min(min_angle, max_angle);
	var max = Max(min_angle, max_angle);

	var steps = Max(1, (max - min) / Max(1, amount));

	for (var angle = min; amount > 0; --amount)
	{
		var shrapnel = CreateObject(Shrapnel, 0, 0, NO_OWNER);
		shrapnel->SetVelocity(angle + RandomX(-spread, +spread), RandomX(70, 100));
		shrapnel->SetRDir(RandomX(-30, +30));
		shrapnel->Launch(GetController());
		shrapnel.ProjectileDamage = this.ShrapnelDamage;
		CreateObject(BulletTrail)->Set(shrapnel, 2, 30);

		/*
		var frag = CreateObject(CMC_Projectile_Bullet); // FIXME: Lazy again :) But seriously, the fragment code is loooong
		frag->Velocity(RandomX(70, 100))
		    ->Range(RandomX(200, 300))
		    // size = 5
		    // trail = 15
		    ->DamageAmount(20)
		    ->Shooter(this);
		frag->Launch(angle);
		*/

		angle += steps;
	}
}


func ShrapnelDamage(){ return 20; }
