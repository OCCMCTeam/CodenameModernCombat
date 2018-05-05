/*
	Gets all allied players.

	@author Marky
 */

global func GetAlliedPlayers(int player)
{
	var allies = [];

	var team = nil;
	if (GetTeamCount() > 0)
	{
		team = GetPlayerTeam(player);
	}
	
	for (var i = 0; i < GetPlayerCount(); ++i)
	{
		var is_ally = false;
		var other = GetPlayerByIndex(i);
		if (other == player)
		{
			is_ally = true;
		}
		else if (nil != team && GetPlayerTeam(other) == team)
		{
			is_ally = true;
		}
		else if (!Hostile(player, other))
		{
			is_ally = true;
		}
		
		if (is_ally)
		{
			PushBack(allies, other);
		}
	}
	
	SortArray(allies);
	return allies;
}
