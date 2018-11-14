/**
	Unit test for the mortal wounds mechanic
 */

static team_a_p1, team_a_p2, team_b_p1, team_b_p2;


func Initialize()
{
	// Create script players for these tests.
	CreateScriptPlayer("Team A P1", RGB(0, 0, 255), 1, CSPF_NoEliminationCheck);
	CreateScriptPlayer("Team A P2", RGB(0, 0, 255), 1, CSPF_NoEliminationCheck);
	CreateScriptPlayer("Team B P1", RGB(255, 0, 0), 2, CSPF_NoEliminationCheck);
	CreateScriptPlayer("Team B P2", RGB(255, 0, 0), 2, CSPF_NoEliminationCheck);
}


func InitializePlayer(int player)
{
	// Initialize script player.
	if (GetPlayerType(player) == C4PT_Script)
	{
		// Store the player numbers.
		if (GetPlayerName(player) == "Team A P1")
		{
			team_a_p1 = player;
		}
		else if (GetPlayerName(player) == "Team A P2")
		{
			team_a_p2 = player;
		}
		else if (GetPlayerName(player) == "Team B P1")
		{
			team_b_p1 = player;
		}
		else if (GetPlayerName(player) == "Team B P2")
		{
			team_b_p2 = player;
		}
		return;
	}
	
	// Set zoom to full map size.
	SetPlayerZoomByViewRange(player, LandscapeWidth(), nil, PLRZOOM_Direct);
	
	// No FoW to see everything happening.
	SetFoW(false, player);
	
	// Move normal players into a relaunch container.
	var relaunch = CreateObject(RelaunchContainer, LandscapeWidth() / 2, LandscapeHeight() / 2);
	GetCrew(player)->Enter(relaunch);

	LaunchTest(1);
	return true;
}

/* --- Some helper things --- */

global func GetPlayerName(int player)
{
	if (player == NO_OWNER)
		return "NO_OWNER";
	return _inherited(player, ...);
}



global func InitTest()
{
	// Remove all objects except the player crew members and relaunch container they are in.
	for (var obj in FindObjects(Find_Not(Find_Or(Find_ID(RelaunchContainer), Find_Category(C4D_Rule)))))
		if (!((obj->GetOCF() & OCF_CrewMember) && (GetPlayerType(obj->GetOwner()) == C4PT_User || obj->GetOwner() == player_victim)))
			obj->RemoveObject();

	// Initialize crew members
	for (var player in [team_a_p1, team_a_p2, team_b_p1, team_b_p2])
	{
		var clonk = GetCrew(player);
		if (!clonk)
		{
			clonk = CreateObjectAbove(Peacemaker, 100, 150, player);
			clonk->MakeCrewMember(player);
			clonk->SetDir(DIR_Right);
			SetCursor(player, clonk);
		}
		clonk->SetPosition(100, 150);
	}
	
	// Initialize flagpole
	CreateObject(CMC_FlagPost, 380, 200, NO_OWNER)->SetHoldTheFlag("Name")->Set();
	CreateObject(CMC_Goal_HoldTheFlag);
	return true;
}

/* --- Tests --- */

//--------------------------------------------------------

global func Test1_OnStart(int player){ return InitTest();}
global func Test1_OnFinished(){ return; }
global func Test1_Execute()
{
	return Wait(30);
}
