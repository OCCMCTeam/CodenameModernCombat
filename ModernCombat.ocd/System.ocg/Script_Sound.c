/**
	Script_Sound.c

	Modification to the Sound() function.
*/


/*
 Modification to Sound():
 - You may pass two proplists as parameters: Sound(name, options)
 - The options proplist can be used to set the parameters of the Sound function except "name", see the official docu
 - The options proplist must contain the .player setting; This defines for which player the sound should be exclusive
 - The name proplist must have the following format: {self = C4V_String, others = C4V_String}
 - The property "self" determines, which sound is played for options.player; No sound will be played for this player if the value is nil
 - The property "others" determines, which sound is played for all other players of type C4PT_User; No sound will be played if the value is nil
 - Note that "others" = nil is redundant, you could simply call Sound("foo", {player = some_index})
 */
global func Sound(name, opts, ...)
{
	if (GetType(name) == C4V_PropList)
	{
		// The opts parameter can be a proplist, see OC System.ocg/NamedParams.c
		// The parameter signature could be made in such a way that it supports both the proplist
		// and non-proplist version, but this is too tiresome to debug.
		if (GetType(opts) != C4V_PropList)
		{
			FatalError("A player specific sound %v has to be used with options in proplist format", name);
		}
		// You have to specify a player then!
		if (nil == opts.player)
		{
			FatalError("Trying to play a player specific sound %v, without a player given.", name);
		}
		
		// Play a sound for this player? You can set name.self to nil, so that a sound is player for the others only
		if (name.self)
		{
			inherited(name.self, opts, ...);
		}
		
		// Play a sound for other players?
		if (name.others)
		{
			// Play for actual players only
			for (var i = 0; i < GetPlayerCount(C4PT_User); ++i)
			{
				var other_player = GetPlayerByIndex(i, C4PT_User);
				if (opts.player != other_player)
				{
					inherited(name.others, {Prototype = opts, player = other_player}, ...);
				}
			}
		}
	}
	else // Play the sound normally
	{
		return inherited(name, opts, ...);
	}
}
