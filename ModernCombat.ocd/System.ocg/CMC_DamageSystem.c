/**
	CMC damage system.
	
	Added as an effect, so that other Clonk types can potentially use this, too.
	Also, this seems better than messing with inheritance.
 */

static const FxCmcDamageSystem = new Effect
{
	// Name, for identification
	Name = "FxCmcDamageSystem",
	
	// Settings, so that e.g. other living beings can bleed, but they do not need the sound system or screen flash
	SettingBlood = true,
	SettingSound = true,
	SettingScreen = true,

	Damage = func (int health_change_exact, int cause, int by_player)
	{
		// Color the screen red
		if (Target->GetAlive() && health_change_exact < 0)
		{
			this.last_cause = cause;
			AddScreenEffect(Abs(health_change_exact));
			AddSoundEffect(Abs(health_change_exact), cause, by_player);
			AddBloodEffect(Abs(health_change_exact), cause, by_player);
		}
		
		return health_change_exact;
	},
	
	CatchBlow = func (int health_change, object from)
	{
		var blood = GetEffect(FxCmcBloodBurst.Name, Target);
		if (blood && from)
		{
			blood.X = from->GetX() - Target->GetX();
			blood.Y = from->GetY() - Target->GetY();
			blood.XDir = from->GetXDir();
			blood.YDir = from->GetYDir();
		}
	},
	
	AddScreenEffect = func (int damage)
	{
		if (!this.SettingScreen) return;

		var intensity_max = 160;
		var intensity = BoundBy(damage / 200, 0, intensity_max);
		var duration = BoundBy(damage / 1000, 15, 45);
		var flash = Target->CreateEffect(FxFlashScreenRGBa, 200, 1, "PlayerDamaged", RGB(255, 0, 0), intensity, duration);
		if (flash)
		{
			flash.AlphaMax = intensity_max;
		}
	},
	
	AddSoundEffect = func (int damage, int cause, int by_player)
	{
		if (!this.SettingSound) return;
		
		// Round down to normal values
		damage /= 1000;
		
		// Impact sound
		var hit = nil;
		if (cause == FX_Call_EngScript)
		{
			hit = "Bullet";
		}
		if (damage > 40)
		{
			hit = "Critical";
		}
		if (hit) // Play only if there is a sound
		{
			Target->~PlaySoundDamageImpact(hit);
		}
		
		// Hurt sound
		var hurt = "";
		if (FX_Call_EngCorrosion == cause
		||  FX_Call_EngAsphyxiation == cause)
		{
			hurt = "Poison";
		}
		else if (FX_Call_EngFire == cause)
		{
			hurt = "Fire";
		}
		// Play always
		if (Random(damage))
		{
			Target->~PlaySoundDamageHurt(hurt);
		}
	},
	
	AddBloodEffect = func (int damage, int cause, int by_player)
	{
		if (!this.SettingBlood) return;
		if (Target->Contained() == nil
		&& (cause == FX_Call_EngObjHit || cause == FX_Call_EngBlast)) // Bleed only from explosions or things that directly hurt you
		{
			var blood = Target->CreateEffect(FxCmcBloodBurst, 200, 1);
			blood.Damage = Min(damage, 200000); // Cap to 200 damage
			blood.Cause = cause;
		}
	},
};


static const FxCmcBloodBurst = new Effect
{
	Name = "FxCmcBloodBurst",
	
	Timer = func (int time)
	{
		Spray(this.Damage / 1000, this.X, this.Y);
		return FX_Execute_Kill;
	},
	
	GetBloodColor = func ()
	{
		return Target.BloodColor ?? {R = RandomX(70, 195), G = 0, B = 0, Alpha = 255};
	},
	
	GetBloodColorLight = func ()
	{
		return Target.BloodColor ?? {R = RandomX(130, 225), G = 0, B = 0, Alpha = 255};
	},
	
	GetRandomX = func ()
	{
		var range_x = Target->GetID()->GetDefWidth()/3; 
		return RandomX(-range_x, +range_x);
	},
	
	GetRandomY = func ()
	{
		var range_y = Target->GetID()->GetDefHeight()/3; 
		return RandomX(-range_y, +range_y);
	},
	
	IsSky = func(int x, int y)
	{
		var material = Target->GetMaterial(x, y);
		return (material == -1) || (GetMaterialVal("Density", "Material", material) != 0);
	},

	Spray = func (int size, int x, int y)
	{
		// Determine some data
		var angle = 0;
		if (this.XDir || this.YDir)
		{
			angle = Angle(0, 0, this.XDir, this.YDir);
		}
		else if (this.Cause == FX_Call_EngBlast)
		{
			angle = Angle(0, 0, Target->GetXDir(), Target->GetYDir());
		}
		
		// Burst / spray
		var burst_angle = Normalize(angle - 90 + RandomX(-5, 5), 0);
		var	burst_color = GetBloodColorLight();
		Target->CreateParticle("BloodBurst", x ?? GetRandomX(), y ?? GetRandomY(), PV_Random(0, this.XDir / 5), PV_Random(0, this.YDir / 5), RandomX(20, 30), 
		{
		    Size = size,
			Phase = PV_Linear(0, 15),
			Rotation = burst_angle,
			Alpha = 255,
			R = burst_color.R, G = burst_color.G, B = burst_color.B,
			DampingX = 500,
			DampingY = 500,
			Attach = ATTACH_Front, 
		});
		
		// Splatter on the background
		if (!IsSky(x, y))
		{
			var lifetime = RandomX(240, 360);
			var splat_color = GetBloodColor();
			var splat = CreateObject(Dummy, Target->GetX(), Target->GetY(), NO_OWNER);
			splat.Visibility = VIS_All;
			splat.Plane = RandomX(1, 6);
			splat->SetObjectLayer(splat); // This mainly excludes it from searches, hit checks, etc. so some performance is saved
			ScheduleCall(splat, splat.RemoveObject, lifetime + 1);
			if (this.Cause == FX_Call_EngBlast || size > 20)
			{
				splat->
				CreateParticle("BloodSplatter", x, y, 0, 0, lifetime,
				{
					Size = size,
					Phase = PV_Random(0, 2),
					Rotation = PV_Random(0, 360),
					Alpha = PV_KeyFrames(0, 0, 0, 50, 200, 800, 200, 1000, 0),
					R = splat_color.R, G = splat_color.G, B = splat_color.B,
					Attach = ATTACH_Back,
				});
			}
			else
			{
				var splat_angle = Normalize(angle + RandomX(-10, 10), 0);
				var radius = RandomX(2, 5) + size / 2; // Factor in the particle rotation
				
				splat->
				CreateParticle("BloodSplatter2", x + Sin(splat_angle, radius), y - Cos(splat_angle, radius), 0, 0, lifetime,
				{
					Size = size,
					Phase = PV_Random(0, 3),
					Rotation = splat_angle,
					Alpha = PV_KeyFrames(0, 0, 0, 50, 200, 800, 200, 1000, 0),
					R = splat_color.R, G = splat_color.G, B = splat_color.B,
					Attach = ATTACH_Back,
				});
			}
		}
	}
};
