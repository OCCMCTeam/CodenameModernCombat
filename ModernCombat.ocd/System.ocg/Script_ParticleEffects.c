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