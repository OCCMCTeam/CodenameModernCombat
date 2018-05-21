#appendto Library_AimManager

// By default this restarts aiming, added a parameter to make this optional
public func SetAimPosition(int angle, bool no_aim_restart)
{
	var aim_stop_old = aim_stop;
	
	inherited(angle, ...);
	
	if (no_aim_restart)
	{
		aim_stop = aim_stop_old;
	}
}

public func StartAim(object weapon, int angle)
{
	inherited(weapon, angle);
	
	aim_stop = 0;
}
