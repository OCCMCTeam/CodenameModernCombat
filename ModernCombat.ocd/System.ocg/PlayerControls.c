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

	_inherited(player, control, spec_id, x, y, strength, repeat, status);
}
