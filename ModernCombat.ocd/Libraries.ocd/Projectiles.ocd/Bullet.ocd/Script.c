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
	SoundAt("Projectiles::Cartridge::HitObject?");
}


/**
 Callback if the projectile collides with the landscape.
 */
public func OnHitLandscape()
{
	// Sound
	if (!MaterialProperties.hit_water)
	{
		var sound_name = "HitHard";
		if (MaterialProperties.sound_category)
		{
			sound_name = Format("Hit%s", MaterialProperties.sound_category);
		}
		SoundAt(Format("Projectiles::Cartridge::%s?", sound_name));
	}
	
	var precision = 10;
	var hit_angle = Angle(0, 0, GetXDir(), GetYDir(), precision);

	if (MaterialProperties.dust_factor)
	{
		// Dust
		var dust_size = 4 * MaterialProperties.dust_factor;
		var dust_xdir = +Sin(hit_angle, -1, precision);
		var dust_ydir = -Cos(hit_angle, -1, precision);
		CreateParticle("Dust", PV_Random(-1, 1), PV_Random(-1, 1), PV_Random(dust_xdir - 1, dust_xdir + 1), PV_Random(dust_ydir - 1, dust_ydir + 1), PV_Random(20, 40),
		{
			Prototype = Particles_Dust(),
			Alpha = PV_KeyFrames(0, 0, 0, 250, 50, 1000, 0),
			R = MaterialProperties.color.R, B = MaterialProperties.color.B, G = MaterialProperties.color.G,
			Size = PV_KeyFrames(0, 0, 5 * dust_size / 12, 100, dust_size, 1000, 7 * dust_size / 12),
		}, 3);
	
		// Debris
		var particle_name = MaterialProperties.particle_type ?? "Air";
		var particles =
		{
			CollisionVertex = 500,
			OnCollision = PC_Bounce(100),
			ForceY = PV_Gravity(900),
			Rotation = PV_Direction(),
			Size = PV_Random(1, Max(1, MaterialProperties.shape)),
		    R = MaterialProperties.color.R,
		    G = MaterialProperties.color.G,
			B = MaterialProperties.color.B,
		};
		if (MaterialProperties.in_liquid)
		{
			particles.DampingX = 650;
			particles.DampingY = 650;
		}
		if (MaterialProperties.particle_phase)
		{
			particles.Phase = MaterialProperties.particle_phase;
		}
		var amount = RandomX(1, 2 + MaterialProperties.dust_factor);
		for (var i = 0; i < amount; ++i)
		{
			var angle = RandomX(hit_angle - 20, hit_angle + 20);
			var speed = -RandomX(velocity / 50, velocity / 20);
			var xdir = +Sin(angle, speed, precision);
			var ydir = -Cos(angle, speed, precision) + Cos(hit_angle, 5, precision);
			CreateParticle(particle_name, -Sign(GetXDir()), -Sign(GetYDir()), xdir, ydir, PV_Random(20, 60), particles, 1);
		}
		CreateParticle(particle_name, PV_Random(-1, 1), PV_Random(-1, 1), PV_Random(-3, 3), PV_Random(-3, 3), PV_Random(20, 40), particles, 2);
	}

	// Sparks
	if (MaterialProperties.spark_factor && !MaterialProperties.in_liquid)
	{
		var xdir = +Sin(hit_angle, -6, precision);
		var ydir = -Cos(hit_angle, -6, precision);
		CreateParticle("Frazzle", 0, 0, PV_Random(xdir - 1, xdir + 1), PV_Random(ydir - 1, ydir + 1), PV_Random(20, 40), { Prototype = Particles_Glimmer(), Phase = PV_Random(0, 4)}, 2 * MaterialProperties.spark_factor);
		CreateParticle("Frazzle", PV_Random(-2, 2), PV_Random(-2, 2), PV_Random(-3, +3), PV_Random(-3, +3), PV_Random(20, 40), { Prototype = Particles_Glimmer(), Phase = PV_Random(0, 4)}, 3);
	}
}

/**
	Callback that happens after a hitscan projectile determines the
	projectile end position. This is typically used to create a
	line of hit effects.

	@par x_start The starting X coordinate of the projectile, in global
                 coordinates. 

	@par y_start The starting Y coordinate of the projectile, in global
                 coordinates. 

	@par x_end   The final X coordinate of the projectile, in global
                 coordinates. 

	@par y_end   The final Y coordinate of the projectile, in global
                 coordinates.
 */
public func OnHitScan(int x_start, int y_start, int x_end, int y_end)
{
	var x = Sign(GetXDir());
	var y = Sign(GetYDir());
	MaterialProperties = GetMaterialProperties(x + x_end - GetX(), y + y_end - GetY());
	DrawBubbles(x_start, y_start, x_end, y_end);
	DrawTrace(x_start, y_start, x_end, y_end);
	DrawSpark();
}

/* --- Display --- */


func ProjectileColor(int time)
{
	var progress = 100 * time / Max(1, lifetime);
	var value = Max(0, 255 - progress * 2);
	
	return RGBa(255, value, value, value);
}

func TrailColor(int time)
{
	var progress = 100 * time / Max(1, lifetime);
	var value = Max(0, 255 - progress * 2);
	
	return RGBa(255, value, value, value);
}

/* --- Effects --- */

func DrawBubbles(int x_start, int y_start, int x_end, int y_end)
{
	var length = Distance(x_start, y_start, x_end, y_end);
	for (var i = 0; i < length; i += 10)
	{

	    var x = ((length - i) * x_start + (i - length) * x_end) / length;
	    var y = ((length - i) * y_start + (i - length) * y_end) / length;
		if (GBackLiquid(x, y))
		{
			if (MaterialProperties && !MaterialProperties.hit_water)
			{
				MaterialProperties.hit_water = true;
				SoundAt("Projectiles::Shared::HitWater?", x, y);
			}
			if (Random(5)) continue;
			CreateObject(Fx_Bubble, x, y, NO_OWNER)->SetSpeed(GetXDir() / 30 + RandomX(-5, 5), GetYDir() / 30 + RandomX(-5, 5));
		}
	}
}

func DrawTrace(int x_start, int y_start, int x_end, int y_end)
{
	var particle_size = 64;
	var position_margin = 20;

	// Determine distance, and cancel if it is too near
	var distance = Distance(x_start, y_start, x_end, y_end);
	if (distance <= 2 * position_margin) return;

	// Determine starting position of the effect
	var position_end = distance - position_margin;
	var position_start = Max(position_margin, RandomX(position_margin, distance * 2 / 3));
	
	// Determine position for full particle size
	var speed = Max(60, RandomX(-20, 20) + velocity);
	var trace_length = RandomX(40, 80);
	var position_growth = Min(trace_length, position_end);
	var position_shrink = Max(position_start, position_end - trace_length);
	if (position_shrink < position_growth)
	{
		position_shrink = (position_shrink + position_growth) / 2;
		position_growth = position_shrink;
		trace_length = position_shrink - position_start;
	}

	// Determine times
	var time_start = CalcLifetime(position_start, speed);
	var time_growth = CalcLifetime(position_growth, speed);
	var time_shrink = CalcLifetime(position_shrink, speed);
	var time_end = CalcLifetime(position_end, speed);

	var time_interval = Max(1, time_end - time_start);
	var keyframe_end = 1000;
	var keyframe_growth = Max(0, (time_growth - time_start) * keyframe_end / time_interval);
	var keyframe_shrink = Max(0, (time_shrink - time_start) * keyframe_end / time_interval);
	var stretch_min = position_margin * 1000 / particle_size;
	var stretch_max = trace_length * 1000 / particle_size;

	// Determine creation offset
	var x = (distance - position_start) * (x_start - x_end) / distance;
	var y = (distance - position_start) * (y_start - y_end) / distance;
	var angle = Angle(x_start, y_start, x_end, y_end);
	var color_start = SplitRGBaValue(this->TrailColor(time_start));
	var color_end = SplitRGBaValue(this->TrailColor(time_end));
	
	var pv_stretch;
	if (position_growth <= position_start)
	{
		pv_stretch = PV_KeyFrames(0, 0, stretch_max, keyframe_shrink, stretch_max, keyframe_end, stretch_min);
	}
	else
	{
		pv_stretch = PV_KeyFrames(0, 0, stretch_min, keyframe_growth, stretch_max, keyframe_shrink, stretch_max, keyframe_end, stretch_min);
	}

	CreateParticle("BulletTrace", x, y, +Sin(angle, speed), -Cos(angle, speed), time_interval,
	{
		R = PV_Linear(color_start.R, color_end.R), G = PV_Linear(color_start.G, color_end.G), B = PV_Linear(color_start.B, color_end.B),
		Size = particle_size,
		Alpha = PV_KeyFrames(0, 0, 110, keyframe_growth, 220, keyframe_shrink, 220, keyframe_end, 0),
		Stretch = pv_stretch,
		Rotation = angle,
		BlitMode = GFX_BLIT_Additive,
		CollisionDensity = 25, // Collide with liquids
		OnCollision = PC_Stop, // Original particle died, but stopping is better
	},
	1);
}

func DrawSpark()
{
	CreateParticle("StarFlash", 0, 0, 0, 0, PV_Random(7, 11), {
	    R = 255,
	    G = PV_Linear(196, 64),
	    B = PV_Random(0, 128, 2),
		Alpha = PV_Linear(160, 0),
		Rotation = PV_Random(0, 360),
		BlitMode = GFX_BLIT_Additive,
		Size = PV_Linear(BoundBy(GetDamageAmount() / 2, 2, 5), 1),
	});
}

func CalcLifetime(int distance, int speed)
{
	var precision_velocity = 10;
	return (precision_velocity * distance) / Max(1, speed ?? velocity); 
}

/* --- Properties --- */

local Name = "$Name$";

local MaterialProperties; // Info for bullet impact effects
