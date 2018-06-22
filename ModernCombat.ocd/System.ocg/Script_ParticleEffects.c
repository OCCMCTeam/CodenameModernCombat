/**
	Particle effects
 */

global func CastSmokeParticles(string name, int amount, int level, x, y, lifetime, properties)
{
	if(GBackLiquid(x, y))
	{
	}
	else
	{
		var range = level / 2;
		CreateParticle(name, x, y, PV_Random(-range, +range), PV_Random(-range, range), 
		lifetime, properties, amount);
	}
}

global func Particles_ThrustColored(int r, int g, int b)
{
	return {
		Prototype = Particles_Thrust(),
		R = PV_KeyFrames(0, 0, r, 500, 2 * r / 5, 1000, r / 5),
		G = PV_KeyFrames(0, 0, g, 500, 2 * g / 5, 1000, g / 5),
		B = PV_KeyFrames(0, 0, b, 500, 2 * b / 5, 1000, b / 5),		
	};
}
