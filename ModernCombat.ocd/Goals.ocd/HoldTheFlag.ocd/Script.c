/**
	Hold the Flag
	
	Integration into a scenario works as described in {@link CMC_Goal_HoldTheFlag#DoSetup}.
*/


#include Library_Goal_ForTeam
#include Plugin_Goal_TimeLimit
#include Plugin_Goal_EliminateLosingPlayers

/* --- Callbacks from round system --- */

func OnRoundStart(int round)
{
	inherited(...);
	
	var clock = CreateObject(GUI_Clock);
	SetClock(clock);
}


/**
	Sets up the goal for a round.
	
	@note
	The flag post should be created by this function.
	Overload for your scenario, complying with:
	{@code
		func DoSetup(int round)
		{
			// Create flag post, etc.
			...
			// Call inherited function
			inherited(round, ...);
		}
	}
	The order is vital here, because the function
	call expects the flag to exist already.
	
	@par round The round number.
 */
func DoSetup(int round)
{
	// Compatibility mode for scenarios that do not have a round manager.
	if (RoundManager() == nil && GetFlag() == nil)
	{
		SetFlag(FindFlag());
	}
	AssertNotNil(GetFlag(), "Goal cannot work properly if there is no flag. Make sure to overload this function and call SetFlag()!");

	// Determine how fast the flag is captured
	var capture_speed = Max(18 - 4 * GetLength(GetActiveTeams()), 4);
	GetFlag()->SetHoldTheFlag()
	         ->SetCaptureSpeed(capture_speed);
	         
	// Set defaults:
	score_progress = 0;
	faction_score_warning = BoundBy(GetWinScore()* 3 / 4, Max(0, GetWinScore() - 5), Max(0, GetWinScore() - 1));

	// Setup the goal timer
	var interval = Max(14 - 2 * GetLength(GetActiveTeams()), 5);
	AddTimer(this.EvaluateProgress, interval);
	
	InitScoreboard();

	inherited(round);
}


func DoCleanup(int round)
{
	RemoveTimer(this.EvaluateProgress);
	if (GetFlag())
	{
		GetFlag()->RemoveObject();
	}
	inherited(round, ...);
}

/* --- Interface --- */

public func SetFlag(object flagpost)
{
	AssertNotNil(flagpost);
	if (!flagpost->~IsFlagpole())
	{
		FatalError(Format("Object %v is not a flag post!", flagpost));
	}
	goal_flagpost = flagpost;
}


public func GetFlag()
{
	return goal_flagpost;
}


func FindFlag()
{
	return FindObject(Find_Func("IsFlagpole"));
}

/* --- Goal Timer --- */

func EvaluateProgress()
{
	if (!GetFlag())
	{
		RemoveObject();
		FatalError("Hold the Flag goal has no flag, will remove the object");
	}

	var team = GetFlag()->GetTeam();
	if (team != NO_OWNER && GetFlag()->IsFullyCaptured())
	{
		score_progress += 1;
		if (score_progress >= 100)
		{
			// Score and reset progress
			score_progress = 0;
			DoFactionScore(team, 1);
	
			// Add points for achievement system
			for (var i; i < GetPlayerCount(); ++i)
			{
				var player = GetPlayerByIndex(i);
				// Leading team?
				if (GetPlayerTeam(GetPlayerByIndex(i)) == team)
				{
					// TODO
					// DoPlayerPoints(BonusPoints("Control"), RWDS_TeamPoints, player, GetCrew(player), IC28);
					Sound("Info_Event", {global = true, player = player});
				}
				// Event message for other teams: The team is close to winning
				else if (GetFactionScore(team) == faction_score_warning)
				{
					EventInfo4K(GetPlayerByIndex(i)+1, Format("$TeamReachingGoal$", GetTaggedTeamName(team), GetWinScore() - faction_score_warning), IC28, 0, 0, 0, "Info_Alarm.ogg");
				}
			}
		}
		UpdateScoreboard();
	}
	else
	{
		score_progress = 0;
		UpdateScoreboard();
	}
}


/* --- Events --- */

// This is currently a game call, might be changed...
public func FlagLost(object flagpost, int old_team, int new_team, array attackers)
{
	if (flagpost != GetFlag()) return;

	// Add points for achievement system (neutralized flag post)
	for (var attacker in attackers)
	{
		if (attacker)
		{
			// TODO
			// DoPlayerPoints(BonusPoints("OPNeutralization"), RWDS_TeamPoints, attacker->GetOwner(), attacker, IC13);
		}
	}

	// Event message: Flag post lost
	for (var i; i < GetPlayerCount(); i++)
	{
		if (GetPlayerTeam(GetPlayerByIndex(i)) == old_team)
		{
			// TODO
			EventInfo4K(GetPlayerByIndex(i)+1, Format("$MsgFlagLost$", GetName(GetFlag()), GetTeamFlagColor(new_team), GetTeamName(new_team)), IC13, 0, GetTeamFlagColor(new_team), 0, "Info_Event.ogg");
		}
	}
}


// This is currently a game call, might be changed...
public func FlagCaptured(object flagpost, int by_team, array attackers, bool regained)
{
	if (flagpost != GetFlag()) return;

	// First attacker gains most points, the others gain assist points
	var first = true; //Der erste bekommt mehr Punkte, der Rest bekommt Assistpunkte
	for (var attacker in attackers)
	{
		if (!attacker) continue;
			
		// Add points for achievement system (defended flag post)
		if (regained)
		{
			// TODO
			// DoPlayerPoints(BonusPoints("OPDefense"), RWDS_TeamPoints, attacker->GetOwner(), attacker, IC12);
		}
		else
		{
			if (first) // Add points for achievement system (captured flag post)
			{
				// TODO
				// DoPlayerPoints(BonusPoints("OPConquest"), RWDS_TeamPoints, attacker->GetOwner(), attacker, IC10);
			}
			else // Add points for achievement system (assisted capturing flag post)
			{
				// TODO
				// DoPlayerPoints(BonusPoints("OPConquestAssist"), RWDS_TeamPoints, attacker->GetOwner(), attacker, IC11);
			}
		}

		first = false;
	}


	// Event message: Flag post captured
	EventInfo4K(0, Format("$MsgCaptured$", GetTeamFlagColor(by_team), GetTeamName(by_team), GetFlag()->GetName()), IC10, 0, GetTeamFlagColor(by_team), 0, "Info_Objective.ogg");
	UpdateScoreboard();
}

/* --- Scoreboard --- */

static const GHTF_Column_Name   = "Scoreboard_Flag";
static const GHTF_Column_Status	= "Scoreboard_Progress";
static const GHTF_Column_Score  = "Scoreboard_Points";
static const GHTF_FlagRow		= 1024;

func InitScoreboard() // TODO
{
	Scoreboard->SetTitle(GetName());
	Scoreboard->Init([
		{key = GHTF_Column_Name,   title = IC12, sorted = true, desc = true, default = 0, priority = 85},
		{key = GHTF_Column_Status, title = " ",  sorted = true, desc = true, default = 0, priority = 80},
		{key = GHTF_Column_Score,  title = SM02, sorted = true, desc = true, default = 0, priority = 75}
	]);
	Scoreboard->NewEntry(1000, "");
	Scoreboard->NewEntry(1001, ""); // Emptry row
	Scoreboard->NewEntry(1002, ""); // Required points
	Scoreboard->NewEntry(1003, ""); // Empty row
	Scoreboard->NewEntry(1004, ""); // Icons
	UpdateScoreboard();
}


func UpdateScoreboard() // TODO
{
	var large_space = "          "; // Should be wide enough, so that the "100%" message does not change the width of the scoreboard

	//Teamfarbe und Flaggenzustand ermitteln
	var capturing_team = GetFlag()->GetTeam();
	var team_color = GetTeamFlagColor(capturing_team);
	var flag_name_color = RGB(255, 255, 255);
	var flag_name = GetFlag()->GetName();
	var capture_progress = GetFlag()->GetProcess();
	var capture_color = InterpolateRGBa(capture_progress, 0, RGB(255, 255, 255), 100, team_color);

	//Flaggennamenfarbe ermitteln
	if (GetFlag()->~IsFullyCaptured())
	{
		flag_name_color = team_color;
	}
		
	//Flaggenicon ermitteln
	var trend = GetFlag()->GetTrend();
	var flag_status_icon = SM21; // No activity
	if (trend == -1) flag_status_icon = SM23; // Attack
	if (trend == +1) flag_status_icon = SM22; // Defense
	
	// First row with data
	var row = 1000;
	var sort_top = 1000;
	Scoreboard->SetData(row, GHTF_Column_Name,   Format("<c %x>%s</c>", flag_name_color, flag_name));
	Scoreboard->SetData(row, GHTF_Column_Status, Format("{{%i}}", flag_status_icon), sort_top);
	Scoreboard->SetData(row, GHTF_Column_Score,  Format("<c %x>%d%</c>", capture_color, capture_progress), sort_top);

	//Leere Zeile
	++row;
	Scoreboard->SetData(row, GHTF_Column_Name, large_space);
	Scoreboard->SetData(row, GHTF_Column_Status, large_space, sort_top);
	Scoreboard->SetData(row, GHTF_Column_Score, large_space, sort_top);

	//Benötigte Punktzahl
	++row;
	Scoreboard->SetData(row, GHTF_Column_Name, "$WinScore$");
	Scoreboard->SetData(row, GHTF_Column_Status, large_space, sort_top);
	Scoreboard->SetData(row, GHTF_Column_Score, Format("%d", GetWinScore()), sort_top);

	//Leere Zeile
	++row;
	Scoreboard->SetData(row, GHTF_Column_Name, large_space);
	Scoreboard->SetData(row, GHTF_Column_Status, large_space, sort_top);
	Scoreboard->SetData(row, GHTF_Column_Score, large_space, sort_top);

	//Icons
	++row;
	Scoreboard->SetData(row, GHTF_Column_Name, "{{SM26}}");
	Scoreboard->SetData(row, GHTF_Column_Status, "{{SM27}}", sort_top);
	Scoreboard->SetData(row, GHTF_Column_Score, "{{IC28}}", sort_top);

	for (var j = 0; j < GetFactionCount(); ++j)
	{
		var team = GetFactionByIndex(j);
		var progress = 0;

		if (team == capturing_team)
		{
			progress = score_progress;
		}

		++row;
		Scoreboard->NewEntry(row, "");
		Scoreboard->SetData(row, GHTF_Column_Name, Format("<c %x>%s</c>", GetTeamFlagColor(team), GetTeamName(team)));
		Scoreboard->SetData(row, GHTF_Column_Status, Format("<c %x>%d%</c>", RGB(128, 128, 128), progress), progress);
		Scoreboard->SetData(row, GHTF_Column_Score, Format("<c ffbb00>%d</c>", GetFactionScore(team)), GetFactionScore(team));
	}
}

func ScoreboardTeamID(int team)
{
	return team + 1000;
}


/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local goal_flagpost; // The captured flag
local score_progress; // Progress of the owning team, towards gaining a point
local faction_score_warning;

func GetDefaultWinScore()
{
	return 10;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Temporary stuff below

func GetTeamFlagColor(int team)
{
	if (GetTeamPlayerCount(team) == 1)
	{
		var player = GetTeamPlayer(team);
		return GetPlayerColor(player);
	}
	else
	{
		return GetTeamColor(team);
	}
}

/* Rundenauswertung */
/*
local fulfilled;

public func check  IsFulfilled()
{
	if (FindObject(CHOS))	return;
	if (fulfilled)		return true;

	UpdateScoreboard();

	//Punktestände prüfen
	for (var i; i < GetLength(aTeamPoints); i++)
		//Ein Team hat die Punktezahl erreicht: Sieg
		if (aTeamPoints[i] >= iGoal)
		{
			//Verlierer eliminieren
			for (var team, j = GetTeamCount(); j; team++)
			if (GetTeamName(team))
			{
				j--;
				if (team != i)
					EliminateTeam(team);
			}
			//Spielende planen
			Schedule("GameOver()", 150);

			//Auswertung
			RewardEvaluation();

			//Nachricht über Gewinner
			Message("@$TeamHasWon$", 0, GetTeamFlagColor(i), GetTeamName(i));

			//Sound
			Sound("Cheer.ogg", true);

			return fulfilled = true;
		}

	//Nur ein Team vorhanden: Gewinnen lassen
	if (GetLength(GetActiveTeams()) <= 1)
	{
		var i = GetPlayerTeam(GetPlayerByIndex());

		//Spielende planen
		Schedule("GameOver()", 150);

		//Auswertung
		RewardEvaluation();

		//Nachricht über Gewinner
		Message("@$TeamHasWon$", 0, GetTeamFlagColor(i), GetTeamName(i));

		//Sound
		Sound("Cheer.ogg", true);

		return fulfilled = true;	
	}
}
*/

func EventInfo4K()
{
	// TODO
}

// TODO
static const SM02 = Rock;
static const SM21 = Rock;
static const SM22 = Shield;
static const SM23 = Firestone;
static const SM26 = Rock;
static const SM27 = Rock;
static const IC28 = Rock;
static const IC10 = Rock;
static const IC12 = Rock;
static const IC13 = Rock;
