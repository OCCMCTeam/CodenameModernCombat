global func PlayerControl(int plr, int ctrl, id spec_id, int x, int y, int strength, bool repeat, int status)
{
	// Map the CON_CMC_UseAlt key to the regular CON_UseAlt
	if (ctrl == CON_CMC_UseAlt)
		ctrl = CON_UseAlt;

	_inherited(plr, ctrl, spec_id, x, y, strength, repeat, status);
}