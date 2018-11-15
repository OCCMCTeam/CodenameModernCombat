/**
	Unit test for the mortal wounds mechanic
 */

static const team_a = 1;
static const team_b = 2;
static team_a_p1, team_a_p2, team_b_p1, team_b_p2, flag, goal, crew;


func Initialize()
{
	// Create script players for these tests.
	CreateScriptPlayer("Team A P1", RGB(0, 0, 255), team_a, CSPF_NoEliminationCheck);
	CreateScriptPlayer("Team A P2", RGB(0, 0, 255), team_a, CSPF_NoEliminationCheck);
	CreateScriptPlayer("Team B P1", RGB(255, 0, 0), team_b, CSPF_NoEliminationCheck);
	CreateScriptPlayer("Team B P2", RGB(255, 0, 0), team_b, CSPF_NoEliminationCheck);
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



global func InitTest(int win_score, int crew_count)
{
	// Remove all objects except the player crew members and relaunch container they are in.
	for (var obj in FindObjects(Find_Not(Find_Or(Find_ID(RelaunchContainer), Find_Category(C4D_Rule)))))
		if (obj && !((obj->GetOCF() & OCF_CrewMember) && (GetPlayerType(obj->GetOwner()) == C4PT_User)))
			obj->RemoveObject();

	crew = [];
	// Initialize crew members
	for (var player in [team_a_p1, team_a_p2, team_b_p1, team_b_p2])
	{
		crew[player] = [];
		for (var i = 0; i < crew_count; ++i)
		{
			var clonk = GetCrew(player, i);
			if (!clonk)
			{
				clonk = CreateObjectAbove(Peacemaker, 100, 150, player);
				clonk->MakeCrewMember(player);
				clonk->SetDir(DIR_Right);
				SetCursor(player, clonk);
			}
			clonk->SetPosition(100, 150);
			crew[player][i] = clonk; 
		}
	}
	
	// Initialize flagpole
	flag = CreateObject(CMC_FlagPost, 380, 200, NO_OWNER);
	flag->SetHoldTheFlag("Name");
	goal = CreateObject(CMC_Goal_HoldTheFlag);
	goal->SetWinScore(win_score);
	return true;
}

/* --- Tests --- */

//--------------------------------------------------------

global func Test1_OnStart(int player)
{
	Log("Team A captures the flag");
	InitTest(1, 10);
	for (var clonk in crew[team_a_p1])
	{
		clonk->SetPosition(flag->GetX(), flag->GetY() - 10);
	}
	return true;
}
global func Test1_OnFinished(){ return; }
global func Test1_Execute()
{
	if (goal->IsFulfilled())
	{
		doTest("Score for team A is %d, expected %d", goal->GetFactionScore(team_a), 1);
		doTest("Score for team B is %d, expected %d", goal->GetFactionScore(team_b), 0);
		return Evaluate();
	}
	else
	{
		return Wait(30);
	}
}

//--------------------------------------------------------

global func Test2_OnStart(int player)
{
	Log("Team B captures the flag");
	InitTest(1, 10);
	for (var clonk in crew[team_b_p2])
	{
		clonk->SetPosition(flag->GetX(), flag->GetY() - 10);
	}
	return true;
}
global func Test2_OnFinished(){ return; }
global func Test2_Execute()
{
	if (goal->IsFulfilled())
	{
		doTest("Score for team A is %d, expected %d", goal->GetFactionScore(team_a), 0);
		doTest("Score for team B is %d, expected %d", goal->GetFactionScore(team_b), 1);
		return Evaluate();
	}
	else
	{
		return Wait(30);
	}
}
