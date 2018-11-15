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
	         ->Set(GetFlag()->GetRange(), capture_speed);
	         
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
			UpdateScoreboard();
	
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

static const GHTF_FlagColumn		= "Scoreboard_Flag";
static const GHTF_ProgressColumn	= "Scoreboard_Progress";
static const GHTF_PointsColumn		= "Scoreboard_Points";
static const GHTF_FlagRow		= 1024;

func InitScoreboard() // TODO
{
	Scoreboard->SetTitle(GetName());
	Scoreboard->Init([
		//{key = "title", title = "Teams"},
		{key = GHTF_FlagColumn,     title = IC12, sorted = true, desc = true, default = 0, priority = 85}, //, conditional = Scoreboard_Death.ScoreboardCondition}
		{key = GHTF_ProgressColumn, title = " ",  sorted = true, desc = true, default = 0, priority = 80}, //, conditional = Scoreboard_Death.ScoreboardCondition}
		{key = GHTF_PointsColumn,   title = SM02, sorted = true, desc = true, default = 0, priority = 75}  //, conditional = Scoreboard_Death.ScoreboardCondition}
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
	//Teamfarbe und Flaggenzustand ermitteln
	var capturing_team = GetFlag()->GetTeam();
	var teamclr = GetTeamFlagColor(capturing_team);
	var nameclr = RGB(255, 255, 255);
	var prog = GetFlag()->GetProcess();
	var percentclr = InterpolateRGBa(prog, 0, RGBa(255, 255, 255), 100, teamclr);

	//Flaggennamenfarbe ermitteln
	if (GetFlag()->~IsFullyCaptured())
	{
		nameclr = teamclr;
	}
		
	//Flaggenicon ermitteln
	var trend = GetFlag()->GetTrend();
	var icon = SM21; // No activity
	if (trend == -1) icon = SM23; // Attack
	if (trend == +1) icon = SM22; // Defense
	
	
	var row = 1000;
	var flag_name = GetFlag()->GetName();
	var extra_data = GHTF_FlagRow - row;
	
	// First row with data
	Scoreboard->SetData(row, GHTF_FlagColumn,     Format("<c %x>%s</c>", nameclr, flag_name));
	Scoreboard->SetData(row, GHTF_ProgressColumn, Format("{{%i}}", icon), extra_data);
	Scoreboard->SetData(row, GHTF_PointsColumn,   Format("<c %x>%d%</c>", percentclr, prog), extra_data);

	//Leere Zeile
	++row;
	Scoreboard->SetData(row, GHTF_FlagColumn, " ");
	Scoreboard->SetData(row, GHTF_ProgressColumn, " ", extra_data);
	Scoreboard->SetData(row, GHTF_PointsColumn, " ", extra_data);

	//Benötigte Punktzahl
	++row;
	Scoreboard->SetData(row, GHTF_FlagColumn, "$SbrdGoalPoints$");
	Scoreboard->SetData(row, GHTF_ProgressColumn, " ", extra_data);
	Scoreboard->SetData(row, GHTF_PointsColumn, Format("%d", GetWinScore()), extra_data);

	//Leere Zeile
	++row;
	Scoreboard->SetData(row, GHTF_FlagColumn, " ");
	Scoreboard->SetData(row, GHTF_ProgressColumn, " ", extra_data);
	Scoreboard->SetData(row, GHTF_PointsColumn, " ", extra_data);

	//Icons
	++row;
	Scoreboard->SetData(row, GHTF_FlagColumn, "{{SM26}}");
	Scoreboard->SetData(row, GHTF_ProgressColumn, "{{SM27}}", extra_data);
	Scoreboard->SetData(row, GHTF_PointsColumn, "{{IC28}}", extra_data);

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
		Scoreboard->SetData(row, GHTF_FlagColumn, Format("<c %x>%s</c>", GetTeamFlagColor(team), GetTeamName(team)));
		Scoreboard->SetData(row, GHTF_ProgressColumn, Format("<c %x>%d</c>", RGB(128, 128, 128), progress), progress);
		Scoreboard->SetData(row, GHTF_PointsColumn, Format("<c ffbb00>%d</c>", GetFactionScore(team)), GetFactionScore(team));
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
