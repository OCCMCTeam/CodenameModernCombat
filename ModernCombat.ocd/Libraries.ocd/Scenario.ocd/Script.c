/*--- CMC Default scenario script ---*/

// TODO - Library internal respawn script:
// It was created before Fulgen and Maikel reworked
// the respawn rule (which is better, but in some aspects not generic enough).
// For now our script uses this one, but I want to look into merging the two.
#include Library_ScenarioScript_PlayerRespawn

/*--- Engine callbacks ---*/

public func Initialize()
{
	_inherited(...);

	// Create the round manager - this organizes the game in rounds
	CreateObject(Environment_RoundManager);
	
	// Create the configuration - TODO: this has old style menus at the moment, will be changed
	// to proper GUI menus soon
	CreateObject(CMC_Game_Session_Configurator);
	
	// A nice log message
	Log("$InitializeScenario$");

	// Create deco, ...  - TODO: this could be made more similar to the 
	// CR CMC function/log order if we do not need multiple rounds per scenario
	CreateDeco();
}


// Player enters the game
public func InitializePlayer(int player, int x, int y, object base, int team, id extra_data)
{
	// For now, simply block the end of the round.
	// It may start immediately (later this will not be the case, once we have the main scenario entry)
	// Later, the goals should register themselves as round end blocker
	RoundManager()->RegisterRoundEndBlocker(GetHiRank(player));
	OnPlayerJoinedRound(player, team);
	
	// Do the usual stuff
	return _inherited(player, x, y, base, team, extra_data, ...);
}

// Last crew is killed
public func RelaunchPlayer(int player, int killed_by_player)
{
	return _inherited(player, killed_by_player, ...);
}


// Player leaves the game or is eliminated
public func RemovePlayer(int player, int team)
{
	return _inherited(player, team, ...);
}


// Override this function with ambience
// settings that are specific to this
// map, such as sky parallaxity, but
// do not change over the course of
// the individual rounds.
//
// Note: This is actually an engine callback :)
private func InitializeAmbience()
{
	Log("$CreateAmbience$");
	_inherited(...);
}

/*--- Custom callbacks ---*/

// Override this function to create
// static deco objects that do not
// change and do not need to
// be reset between rounds.
//
// Note: might be removed, because this
//       is essentially the same as
//       having an Objects.c file.
private func CreateDeco()
{
	Log("$CreateDecoration$");
	_inherited(...);
}


// Override this function to create
// dynamic deco objects that may be
// Destroyed during a round.
private func CreateInterior(int round_number)
{
	Log("$CreateInterior$");
	_inherited(...);
}


// Override this function to remove
// dynamic deco objects or their remains
// at the end of a round.
private func CleanUpInterior(int round_number)
{
	_inherited(...); // Does nothing for now
}


// Override this function to create
// the equipment for this round.
private func CreateEquipment(int round_number)
{
	Log("$CreateEquipment$");
	_inherited(...);
}


// Override this function to remove
// the equipment at the end of a round.
private func CleanUpEquipment(int round_number)
{
	_inherited(...); // Does nothing for now
}


/*--- Callbacks from Environment_RoundManager ---*/

// These callbacks happen in order:

// 1) Round is reset to an initial, neutral state
private func OnRoundReset(int round_number)
{
	Log("$ResetScenario$");
	
	// Interior objects and equipment should be reset at the start of the round
	CreateInterior();
	CreateEquipment();
	
	// Make map visible to everyone
	UpdateFoW();
	SetPlayerZoomLandscape();
	
	// Do the usual stuff
	_inherited(round_number, ...);
}


// 2) Round starts, players should be able to act now
private func OnRoundStart(int round_number)
{
	// Activate fog of war
	UpdateFoW();

	// Do the usual stuff
	_inherited(round_number, ...);
}


// 3) Round is over, clean up things, next call will be the reset
private func OnRoundEnd(int round_number)
{
	// Interior objects and equipment should be remove at the end of the round
	CleanUpInterior();
	CleanUpEquipment();
	
	// Disable fog of war
	UpdateFoW(nil, nil, false);
	
	_inherited(round_number, ...); // Does nothing for now
}

/* --- Player management --- */

func OnPlayerJoinedRound(int player, int team)
{
	UpdateFoW(player, team);
}

// Sets the fog of war for a player.
// If neither player nor team are given the setting will be applied to all players.
// If the 'enabled' value is not given the fog of war will be enabled only
// if the round is active.
func UpdateFoW(int player, int team, bool enabled)
{
	if (player == nil && team == nil)
	{
		for (var i = 0; i < GetPlayerCount(); ++i)
		{
			UpdateFoW(GetPlayerByIndex(i));
		}
	}
	else if (player != nil)
	{
		SetFoW(enabled ?? RoundManager()->IsRoundActive(), player);
	}
}


//	Defines which crew will be spawned for the player.
//
//	@par player The player number.
public func GetPlayerCrewID(int player)
{
	return Peacemaker;
}

/* --- Helper for spawn points --- */


/*
	Gets a proplist with the usual spawn points. 

	This is more convenient than copy & pasting the script each time.
 */
public func GetSpawnPointTemplates()
{
	return {
		ammo_bullets = CreateSpawnPoint(0, 0)->SpawnItem(CMC_Ammo_Box)->SetCallbackOnSpawn(CMC_Ammo_Box.SpawnAs, CMC_Ammo_Bullets),
		ammo_grenades = CreateSpawnPoint(0, 0)->SpawnItem(CMC_Ammo_Box)->SetCallbackOnSpawn(CMC_Ammo_Box.SpawnAs, CMC_Ammo_Grenades),
		ammo_missiles = CreateSpawnPoint(0, 0)->SpawnItem(CMC_Ammo_Box)->SetCallbackOnSpawn(CMC_Ammo_Box.SpawnAs, CMC_Ammo_Missiles),
	};
}


/*
	Removes all spawn point templates in a given proplist.
 */
public func RemoveSpawnPointTemplates(proplist templates)
{
	for (var property in GetProperties(templates))
	{
		if (templates[property] != nil)
		{
			templates[property]->RemoveObject();
		}
	}
}