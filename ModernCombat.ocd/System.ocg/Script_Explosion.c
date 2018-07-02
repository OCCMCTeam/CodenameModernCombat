/**
	Helper functions for explosions
	
	@author Marky
 */

/**
	Creates an explosion with multiple damage and radius stages.
	
	@par radius_stages The explosion ranges, e.g. [10, 20, 30] pixels. Must be ascending;
	@par damage_stages    The damage stages, e.g. [50, 40, 30] energy. Must be descending;
	@par silent If set to {@code true} there will be no sound.
	
	@par full_damage By default, the damage is 100% at the explosion center (10x10 pixel box),
	                 and 50% of the damage level inside the radius range.
	                 Setting this to {@code true} causes 200% damage at the center and 100% in the radius range.
 */
global func Explosion(array radius_stages, array damage_stages, bool silent, bool full_damage)
{
	AssertObjectContext();
	
	var multiplier = 1;
	if (full_damage) multiplier = 2;

	// Default to inverted values
	var length = GetLength(radius_stages) - 1;
	if (nil == damage_stages)
	{
		damage_stages = [];
		for (var i = 0; i <= length; ++i)
		{
			damage_stages[i] = radius_stages[length - i];
		}
	}
	if (GetLength(radius_stages) != GetLength(damage_stages))
	{
		FatalError("Damage and radius arrays must be of equal length");
	}
	var accumulated_damage = 0;
	for (var i = length; i >= 0; --i)
	{
		var damage = damage_stages[i] - accumulated_damage;
		accumulated_damage += damage;

		if (i > 0)
		{
			BlastObjects(GetX(), GetY(), radius_stages[i], Contained(), GetController(), damage * multiplier, GetObjectLayer());
		}
		else
		{
			Explode(radius_stages[i], silent, damage * multiplier);
		}
	}
}
