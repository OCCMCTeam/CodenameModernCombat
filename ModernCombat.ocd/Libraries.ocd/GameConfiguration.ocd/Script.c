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
