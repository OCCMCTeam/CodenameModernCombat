#appendto Library_AimManager

// Modify the aim manager to take in an 'aim type' variable.
// Because in CMC one weapon has different styles of aiming (ironsight, hip shooting, lying down), the aim manager
// should preserve a value to distinguish these.

local aim_type;

// If not set, type will be "default" if not set
public func StartAim(object weapon, int angle, string type)
{
	_inherited(weapon, angle);

	if (!type)
		type = "Default";
	aim_type = type;
}

public func GetAimType()
{
	return aim_type;
}

// Modify the aiming effect to shift the camera if wanted.
// With this, aiming can shift according to the viewing angle.

public func SetAimViewOffset(int distance)
{
	var aim_effect = GetEffect("IntAim", this);
	if (!aim_effect) // Need to call StartAim first!
		return;

	aim_effect.view_offset = distance;
}

func FxIntAimTimer(target, effect, time)
{
	// Call the main function first, to set everything up
	var die = _inherited(target, effect, time);
	if (die == -1) // X_X
		return FX_Execute_Kill;
	// Check if a view offset is set
	if (effect.view_offset)
	{
		var x_offset = +Sin(this.aim_angle, effect.view_offset);
		var y_offset = -Cos(this.aim_angle, effect.view_offset);
		SetViewOffset(this->GetOwner(), x_offset, y_offset);
	}
}

func FxIntAimStop(object target, effect, int reason, bool temp)
{
	_inherited(target, effect, reason, temp);

	if (temp)
		return;

	// Reset viewing offset!
	if (effect.view_offset)
		SetViewOffset(this->GetOwner(), 0,0);
}