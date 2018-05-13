/**
	Overloads/implements callbacks from the game configuration object.
 */

#include Environment_Configuration

/* --- Settings --- */

// Prohibit menu for spawn point contents
func CanConfigureSpawnPoints()
{
	return false;
}

// Crews are contained at this point by default. Scenarios may overload this.
func ContainCrewAt()
{
	return RelaunchLocation(LandscapeWidth() / 2, LandscapeHeight() / 2);
}
