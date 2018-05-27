/**
	Particle effects
	
	Should be backported to arena library or OC game
	
 */


global func CreateLEDEffect(int color, int x, int y, int size, int lifetime, bool attach)
{
	var colors = SplitRGBaValue(color);
	// Fallback to default magic particle starting alpha
	if (colors.Alpha = 255)
	{
		colors.Alpha = 128;
	}
	
	var attachment = nil;
	if (attach)
	{
		attachment = ATTACH_Front | ATTACH_MoveRelative;
	}

	CreateParticle("Magic", x, y, 0, 0, lifetime ?? 20,
	{
		Prototype = Particles_Magic(),
		BlitMode = GFX_BLIT_Additive,
		R = colors.R, G = colors.G, B = colors.B,
		Size = size ?? 8,
		Alpha = PV_Linear(colors.Alpha, 0),
		Attach = attachment,
	});
}