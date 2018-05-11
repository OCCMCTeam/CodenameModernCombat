/**
	Some standardized zoom functions

	@author Marky
 */

global func SetPlayerZoomDefault(int player)
{
	if (player == nil)
	{
		for (var i = 0; i < GetPlayerCount(); ++i)
		{
			SetPlayerZoomDefault(GetPlayerByIndex(i));
		}
	}
	else
	{
		SetPlayerZoomByViewRange(player, nil, nil, PLRZOOM_Set | PLRZOOM_LimitMin | PLRZOOM_LimitMax);
	}
}


global func SetPlayerZoomLandscape(int player)
{
	var range = Max(LandscapeWidth(), LandscapeHeight());
	
	if (player == nil)
	{
		for (var i = 0; i < GetPlayerCount(); ++i)
		{
			SetPlayerZoomDefault(GetPlayerByIndex(i));
		}
	}
	else
	{
		SetPlayerZoomByViewRange(GetOwner(), range, nil, PLRZOOM_Set | PLRZOOM_LimitMax);
	}
}
