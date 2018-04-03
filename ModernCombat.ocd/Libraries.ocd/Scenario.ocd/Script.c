/*--- CMC Default scenario script ---*/

/*--- Engine callbacks ---*/

public func Initialize()
{
	_inherited(...);

	// Create the round manager - this organizes the game in rounds
	CreateObject(Environment_RoundManager);

	// Create deco, ...
	InitializeDeco();
}


// Player enters the game
public func InitializePlayer(int player, int x, int y, object base, int team, id extra_data)
{
	// For now, simply block the end of the round.
	// It may start immediately (later this will not be the case, once we have the main scenario entry)
	RoundManager()->RegisterRoundEndBlocker(GetHiRank(player));
	
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
	_inherited(...); // Does nothing for now
}

/*--- Custom callbacks ---*/

// Override this function to create
// static deco objects that do not
// change over and do not need to
// be reset between rounds.
//
// Note: might be removed, because this
//       is essentially the same as
//       having an Objects.c file.
private func InitializeDeco()
{
	_inherited(...); // Does nothing for now
}

/*--- Callbacks from Environment_RoundManager ---*/

// These callbacks happen in order:

// 1) Round is reset to an initial, neutral state
private func OnRoundReset(int round_number)
{
	_inherited(round_number, ...); // Does nothing for now
}


// 2) Round starts, players should be able to act now
private func OnRoundStart(int round_number)
{
	_inherited(round_number, ...); // Does nothing for now
}


// 3) Round is over, clean up things, next call will be the reset
private func OnRoundEnd(int round_number)
{
	_inherited(round_number, ...); // Does nothing for now
}
