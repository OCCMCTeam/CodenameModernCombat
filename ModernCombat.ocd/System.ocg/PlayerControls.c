/**
	Use this for redefining certain button behaviour.
 */

global func PlayerControl(int player, int control, id spec_id, int x, int y, int strength, bool repeat, int status)
{
	// Map the CON_CMC_UseAlt key to the regular CON_UseAlt
	if (control == CON_CMC_UseAlt)
	{
		control = CON_UseAlt;
	}
	
	// Map the CON_QuickSwitch key to CON_CMC_ItemConfig
	if (control == CON_QuickSwitch)
	{
		control = CON_CMC_ItemConfig;
	}

	return _inherited(player, control, spec_id, x, y, strength, repeat, status);
}

// Called when CON_Left/Right/Up/Down controls are issued/released
// Return whether handled
global func ObjectControlMovement(int plr, int ctrl, int strength, int status, bool repeat)
{
	if (!this) return false;

	// Movement is only possible when not contained
	if (Contained()) return false;

	var proc = GetProcedure();

	if (status == CONS_Down)
	{
		if (ctrl == CON_Down && proc == "WALK" && Abs(GetXDir()) < 5)
		{
			this->GoProne();
			return true;
		}
	}

	return _inherited(plr, ctrl, strength, status, repeat);
}