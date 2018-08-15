/**
	Extends the firemode template with some information.
 */

#appendto Library_Firearm_Firemode

/* --- CMC Defaults --- */

public func SetCMCDefaults()
{
	this->SetHipFireTransition(WEAPON_AIM_TRANS_INST)
	    ->SetHipFireAimingAnimation("AimArmsGeneric.R")
	    ->SetIronsightTransition(WEAPON_AIM_TRANS_BLND)
	    ->SetIronsightDelay(15)
	    ->SetIronsightAimingAnimation("AimPistol")
	    ->SetProneTransition(WEAPON_AIM_TRANS_BLND)
	    ->SetProneDelay(20)
	    ->SetProneAimingAnimation("ProneAim");
	return this;
}

/* --- Getters --- */

// --- Hip Fire Aiming

/**
 Get the aiming transition for hip fire aiming
 "Instant" is recommended because hip fire aiming should be the quick way
*/
public func GetHipFireTransition()
{
	return this.hipfire_transition;
}

/**
 Get the transition animation into hip fire aiming
 Usually not used because hip fire is mostly instant
*/
public func GetHipFireAnimation()
{
	return this.hipfire_animation;
}

/**
 Get the delay for hip fire aiming
 Usually not used because hip fire is mostly instant
*/
public func GetHipFireDelay()
{
	return this.hipfire_delay;
}

/**
 Get the aiming animation for hip fire aiming
*/
public func GetHipFireAimingAnimation()
{
	return this.hipfire_anim_aim;
}

// --- Ironsight Aiming

/**
 Get the aiming transition for ironsight aiming
*/
public func GetIronsightTransition()
{
	return this.ironsight_transition;
}

/**
 Get the transition animation into ironsight aiming
*/
public func GetIronsightAnimation()
{
	return this.ironsight_animation;
}

/**
 Get the delay for ironsight aiming
*/
public func GetIronsightDelay()
{
	return this.ironsight_delay;
}

/**
 Get the aiming animation for ironsight aiming
*/
public func GetIronsightAimingAnimation()
{
	return this.ironsight_anim_aim;
}

// --- Prone Aiming

/**
 Get the aiming transition for prone aiming
*/
public func GetProneTransition()
{
	return this.prone_transition;
}

/**
 Get the transition animation into prone aiming
*/
public func GetProneAnimation()
{
	return this.prone_animation;
}

/**
 Get the delay for prone aiming
*/
public func GetProneDelay()
{
	return this.prone_delay;
}

/**
 Get the aiming animation for prone aiming
*/
public func GetProneAimingAnimation()
{
	return this.prone_anim_aim;
}

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
	if (this.sound_fire_max)
	{
		// Cycle through the sounds with a zero-based index
		this.sound_fire_counter = (this.sound_fire_counter + 1) % this.sound_fire_max;
		return Format("%s%d", GetFireSound(), this.sound_fire_counter + 1);
	}
	else
	{
		return GetFireSound();
	}
}

/**
	Get the spread that the user gets
	initially after selecting the weapon
	in this fire mode.
 */
public func GetSpreadBySelection()
{
	return this.spread_by_selection;
}

/**
	Set the maximum spread that the user can have
	in this fire mode.
 */
public func GetSpreadLimit()
{
	return this.spread_limit;
}

/**
	Get the type of cursor that is shown
	while aiming. Nil means no cursor.
 */
public func GetAimCursor()
{
	return this.aim_cursor;
}

/**
	Get an override for the <ammo name> in the status GUI
	info "<ammo name> - <fire technique>".

	@return string An override for the <ammo name>. If not
	               defined this defaults to the name of
	               the ammo ID.
 */
public func GetAmmoName()
{
	return this.ammo_name;
}

/**
	Get whether hip shooting mode is disabled.

	@return bool Should be {@code true} if hip
	             shooting is disabled.
 */
public func GetHipShootingDisabled()
{
	return this.hip_shooting_disabled;
}

/* --- Setters --- */

// --- Hip Fire Aiming

/**
 Set the aiming transition for hip fire aiming
 "Instant" is recommended because hip fire aiming should be the quick way
*/
public func SetHipFireTransition(string transition)
{
	this.hipfire_transition = transition;
	return this;
}

/**
 Set the transition animation into hip fire aiming
 Usually not used because hip fire is mostly instant
*/
public func SetHipFireAnimation(string animation)
{
	this.hipfire_animation = animation;
	return this;
}

/**
 Set the delay for hip fire aiming
 Usually not used because hip fire is mostly instant
*/
public func SetHipFireDelay(int delay)
{
	this.hipfire_delay = delay;
	return this;
}

/**
 Set the aiming animation for hip fire aiming
*/
public func SetHipFireAimingAnimation(string name)
{
	this.hipfire_anim_aim = name;
	return this;
}

// --- Ironsight Aiming

/**
 Set the aiming transition for ironsight aiming
*/
public func SetIronsightTransition(string transition)
{
	this.ironsight_transition = transition;
	return this;
}

/**
 Set the transition animation into ironsight aiming
*/
public func SetIronsightAnimation(string animation)
{
	this.ironsight_animation = animation;
	return this;
}

/**
 Set the delay for ironsight aiming
*/
public func SetIronsightDelay(int delay)
{
	this.ironsight_delay = delay;
	return this;
}

/**
 Set the aiming animation for hip fire aiming
*/
public func SetIronsightAimingAnimation(string name)
{
	this.ironsight_anim_aim = name;
	return this;
}

// --- Prone Aiming

/**
 Set the aiming transition for prone aiming
*/
public func SetProneTransition(string transition)
{
	this.prone_transition = transition;
	return this;
}

/**
 Set the transition animation into prone aiming
*/
public func SetProneAnimation(string animation)
{
	this.prone_animation = animation;
	return this;
}

/**
 Set the delay for prone aiming
*/
public func SetProneDelay(int delay)
{
	this.prone_delay = delay;
	return this;
}

/**
 Set the aiming animation for hip fire aiming
*/
public func SetProneAimingAnimation(string name)
{
	this.prone_anim_aim = name;
	return this;
}

/**
 Set the fire sound for this fire mode.
 
 @par name The sound name. Must not contain numbers.
 @par count The amount of sounds of this type.
            A value of 'nil' means that there is no sound playlist,
            but a random sound;
*/
public func SetFireSound(string name, int count)
{
	AssertNotNil(name);

	this.sound_fire_counter = 0;
	this.sound_fire_max = count;
	this.sound_fire = name;
	return this;
}

/**
	Set the spread that the user gets
	initially after selecting the weapon
	in this fire mode.
 */
public func SetSpreadBySelection(int value)
{
	this.spread_by_selection = value;
	return this;
}

/**
	Set the maximum spread that the user can have
	in this fire mode.
 */
public func SetSpreadLimit(int value)
{
	this.spread_limit = value;
	return this;
}

/**
	Set the type of cursor that is shown
	while aiming. Nil means no cursor.
	
	@par cursor Can be an ID in case of one cursor,
	            or an array of IDs if you desire
	            multiple cursors at the same time.
 */
public func SetAimCursor(cursor)
{
	this.aim_cursor = cursor;
	return this;
}

/**
	Set an override for the <ammo name> in the status GUI
	info "<ammo name> - <fire technique>".

	@par name An override for the <ammo name>. If not
	          defined this defaults to the name of
	          the ammo ID.
 */
public func SetAmmoName(string name)
{
	this.ammo_name = name;
	return this;
}

/**
	Set the hip shooting mode disabled. This makes
	sense for weapons that allow aimed firing only.

	@par disabled Should be {@code true} if hip
	              shooting is disabled.
 */
public func SetHipShootingDisabled(bool disabled)
{
	this.hip_shooting_disabled = disabled;
	return this;
}
