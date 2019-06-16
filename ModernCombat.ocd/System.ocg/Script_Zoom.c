/**
	Some standardized zoom functions

	@author Marky
 */

/* --- Constants --- */

static const CMC_ViewRange_GlobalMin = 400;

static const CMC_ViewRange_Default_Player = 810;

/* --- Functions --- */

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
		SetPlayerZoomByViewRange(player, CMC_ViewRange_Default_Player, nil, PLRZOOM_Set | PLRZOOM_LimitMax);
		SetPlayerZoomByViewRange(player, CMC_ViewRange_GlobalMin, nil, PLRZOOM_LimitMin);
	}
}


global func SetPlayerZoomLandscape(int player)
{
	var range = Max(LandscapeWidth(), LandscapeHeight());

	if (player == nil)
	{
		for (var i = 0; i < GetPlayerCount(); ++i)
		{
			SetPlayerZoomLandscape(GetPlayerByIndex(i));
		}
	}
	else
	{
		SetPlayerZoomByViewRange(player, range, nil, PLRZOOM_Set | PLRZOOM_LimitMax);
		SetPlayerZoomByViewRange(player, CMC_ViewRange_GlobalMin, nil, PLRZOOM_LimitMin);
	}
}
