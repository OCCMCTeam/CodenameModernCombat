/**
	CMC damage system.
	
	Added as an effect, so that other Clonk types can potentially use this, too.
	Also, this seems better than messing with inheritance.
 */

static const FxCmcDamageSystem = new Effect
{
	Damage = func (int health_change, int cause, int by_player)
	{
		// Color the screen red
		if (Target->GetAlive() && health_change < 0)
		{
			AddScreenEffect(Abs(health_change));
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
};
