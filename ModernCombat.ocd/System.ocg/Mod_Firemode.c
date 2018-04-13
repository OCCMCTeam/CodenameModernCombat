/*
	Mod_Firemode.c

	Extends the firemode template with some information.
 */

#appendto Library_Firearm_Firemode


/*-- Getters --*/

/**
 Get the sound for firing.
 
 @return The sound string, without sound number.
*/
public func GetFireSound()
{
	return this.sound_fire;
}

/**
 Gets the current fire sound. This composes the fire sound with a cycling number.
 Calling this function also cycles the number, so call it only once per function,
 preferrably.
 
 @return The composed sound string.
*/
public func GetCurrentFireSound()
{
	// Cycle through the sounds with a zero-based index
	this.sound_fire_counter = (this.sound_fire_counter + 1) % this.sound_fire_max;
	return Format("%s%d", GetFireSound(), this.sound_fire_counter + 1);
}

/*-- Setters --*/

/**
 Set the fire sound for this fire mode.
 
 @par name The sound name. Must not contain numbers.
 @par count The amount of sounds of this type. Defaults to 1;
*/
public func SetFireSound(string name, int count)
{
	AssertNotNil(name);
	
	this.sound_fire_counter = 0;
	this.sound_fire_max = count ?? 1;
	this.sound_fire = name;
}
