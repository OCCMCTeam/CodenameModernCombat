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
			BlastObjects(GetX(), GetY(), radius_stages[i], Contained(), GetController(), damage * multiplier, GetObjectLayer(), nil, true);
		}
		else
		{
			Explode(radius_stages[i], silent, damage * multiplier);
		}
	}
}

//------------------------------------------------------------------------------------------------------------
//
// Code below is from System.ocg/Explode.c
//
// Has some issues, in my opinion, should be modularized again in OC. Will mark the actual changes
// TODO (as a reminder to remove this one day.)

/*-- Blast objects & shockwaves --*/

// Damage and hurl objects away.
// documented in /docs/sdk/script/fn
global func BlastObjects(int x, int y, int level, object container, int cause_plr, int damage_level, object layer, object prev_container, bool no_shockwave)
{
	var obj;
	
	// Coordinates are always supplied globally, convert to local coordinates.
	var l_x = x - GetX(), l_y = y - GetY();
	
	// caused by: if not specified, controller of calling object
	if (cause_plr == nil)
		if (this)
			cause_plr = GetController();
	
	// damage: if not specified this is the same as the explosion radius
	if (damage_level == nil)
		damage_level = level;
	
	// In a container?
	if (container)
	{
		if (container->GetObjectLayer() == layer)
		{
			container->BlastObject(damage_level, cause_plr);
			if (!container)
				return true; // Container could be removed in the meanwhile.
			for (obj in FindObjects(Find_Container(container), Find_Layer(layer), Find_Exclude(prev_container)))
				if (obj)
					obj->BlastObject(damage_level, cause_plr);
		}
	}
	else
	{
		// Object is outside.
		var at_rect = Find_AtRect(l_x - 5, l_y - 5, 10, 10);
		// Damage objects at point of explosion.
		for (var obj in FindObjects(at_rect, Find_NoContainer(), Find_Layer(layer), Find_Exclude(prev_container)))
			if (obj) obj->BlastObject(damage_level, cause_plr);
			
		// Damage objects in radius.
		for (var obj in FindObjects(Find_Distance(level, l_x, l_y), Find_Not(at_rect), Find_NoContainer(), Find_Layer(layer), Find_Exclude(prev_container)))
			if (obj) obj->BlastObject(damage_level / 2, cause_plr);

		
		// Perform the shockwave at the location where the top level container previously was.
		// This ensures reliable flint jumps for explosives that explode inside a crew member.
		var off_x = 0, off_y = 0;
		if (prev_container)
		{	
			var max_offset = 300;
			off_x = BoundBy(-prev_container->GetXDir(100), -max_offset, max_offset);
			off_y = BoundBy(-prev_container->GetYDir(100), -max_offset, max_offset);			
		}
		// Original code: DoShockwave(x, y, level, cause_plr, layer, off_x, off_y);
		// Modified:
		if (!no_shockwave)
		{
			DoShockwave(x, y, level, cause_plr, layer, off_x, off_y);
		}
	}
	// Done.
	return true;
}
