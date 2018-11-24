/**
	CMC damage system.
	
	Added as an effect, so that other Clonk types can potentially use this, too.
	Also, this seems better than messing with inheritance.
 */

static const FxCmcDamageSystem = new Effect
{
	Name = "FxCmcDamageSystem",

	Damage = func (int health_change, int cause, int by_player)
	{
		// Color the screen red
		if (Target->GetAlive() && health_change < 0)
		{
			this.last_cause = cause;
			AddScreenEffect(Abs(health_change));
			AddSoundEffect(Abs(health_change), cause, by_player);
		}
		
		return health_change;
	},
	
	AddScreenEffect = func (int damage)
	{
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
};
