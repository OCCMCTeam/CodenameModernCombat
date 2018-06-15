/**
	Extends the firemode template with some information.
 */

#appendto Library_Firearm_Firemode

/* --- Getters --- */

/**
 Get the aiming animation for ironsight aiming
*/
public func GetIronsightAimingAnimation()
{
	return this.ironsight_anim_aim;
}

/**
 Get the transition type for carrying to ironsight.
 Can be one of three constants (see CMC Firearm Library).
*/
public func GetIronsightType()
{
	return this.ironsight_trans;
}

/**
 Get the transition delay for carrying to ironsight.
*/
public func GetIronsightDelay()
{
	return this.ironsight_delay;
}

/**
 Get the aiming animation for hip shooting aiming
*/
public func GetHipFireAimingAnimation()
{
	return this.hipfire_anim_aim;
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
	// Cycle through the sounds with a zero-based index
	this.sound_fire_counter = (this.sound_fire_counter + 1) % this.sound_fire_max;
	return Format("%s%d", GetFireSound(), this.sound_fire_counter + 1);
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



/* --- Setters --- */

/**
 Set the aiming animation for ironsight aiming
*/
public func SetIronsightAimingAnimation(string name)
{
	this.ironsight_anim_aim = name;
	return this;
}

/**
 Set the transition type for carrying to ironsight.
 Can be one of three constants (see CMC Firearm Library).
*/
public func SetIronsightType(int type)
{
	this.ironsight_trans = type;
	return this;
}

/**
 Set the transition delay for carrying to ironsight.
*/
public func SetIronsightDelay(int delay)
{
	this.ironsight_delay = delay;
	return this;
}

/**
 Set the aiming animation for hip shooting aiming
*/
public func SetHipFireAimingAnimation(string name)
{
	this.hipfire_anim_aim = name;
	return this;
}

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
