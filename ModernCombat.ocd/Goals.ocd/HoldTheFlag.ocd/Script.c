/**
	Hold the Flag
 */


#include Library_Goal_ForTeam
#include Plugin_Goal_TimeLimit
#include Plugin_Goal_EliminateLosingPlayers

/* --- Callbacks from other round system --- */

func OnRoundStart(int round)
{
	inherited(...);
	
	var clock = CreateObject(GUI_Clock);
	SetClock(clock);
}


func DoSetup(int round)
{
	ScheduleCall(this, this.UpdateFlag, 1);

	//Spielzielmechanik-Effekt
	var interval = Max(14 - 2 * GetLength(GetActiveTeams()), 5);
	AddEffect("IntAddProgress", this, 1, interval, this);

	UpdateHUDs();

	_inherited(round);
}


func DoCleanup(int round)
{
	if (goal_flagpost)
	{
		goal_flagpost->RemoveObject();
	}
}

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local goal_flagpost; // The captured flag
local goal_progress; // Progress of the owning team, towards gaining a point

func GetDefaultWinScore()
{
	return 10;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


func /* check */ UpdateFlag()
{
	//Flaggenposten ermitteln
	goal_flagpost = FindFlag();
	if (!goal_flagpost) return;

	//Einnahmegeschwindigkeit ermitteln
	var capture_speed = Max(18 - 4 * GetLength(GetActiveTeams()), 4);
	goal_flagpost->Set(GetName(goal_flagpost), goal_flagpost->GetRange(), capture_speed);

	return true;
}


func FindFlag()
{
	return FindObject(Find_Func("IsFlagpole"));
}



/* Spielzielmechanik-Effekt */

func /* check */ FxIntAddProgressTimer()
{
	if (!goal_flagpost && !SetFlag(FindFlag())) return;

	UpdateScoreboard();

	//Besitzer des Flaggenpostens ermitteln
	var team = goal_flagpost->~GetTeam();

	//Kein Besitzer: Nichts weiter unternehmen
	if (team == -1) return;

	//Nicht vollständig eingenommen: Fortschritt zurücksetzen
	if (!goal_flagpost->IsFullyCaptured())
	{
		goal_progress = 0;

		//Update um Prozente nicht falschen Teams zuzuschreiben
		return UpdateScoreboard();
	}
	
	

	var warning = BoundBy(GetWinScore()* 3 / 4, Max(0, GetWinScore() - 5), Max(0, GetWinScore() - 1));
	//Punkt erspielt: Entsprechendem Team zuschreiben
	if ((++goal_progress) >= 100)
	{
		//Punkt vergeben
		DoFactionScore(team, 1);
		//Fortschritt zurücksetzen
		goal_progress = 0;

		UpdateHUDs();

		//Punkte bei Belohnungssystem
		for (var i; i < GetPlayerCount(); i++)
		{
			/* TODO
			if (GetPlayerTeam(GetPlayerByIndex(i)) == team)
			{
				//Punkte bei Belohnungssystem (Team führt)
				DoPlayerPoints(BonusPoints("Control"), RWDS_TeamPoints, GetPlayerByIndex(i), GetCrew(GetPlayerByIndex(i)), IC28);
				Sound("Info_Event.ogg", true, 0, 0, GetPlayerByIndex(i)+1);
			}
			else*/ if (GetFactionScore(GetFactionByIndex(team)) == warning)
			{
				//Eventnachricht: Hinweis auf Team, das dem Ziel nahe ist
				EventInfo4K(GetPlayerByIndex(i)+1, Format("$TeamReachingGoal$", GetTaggedTeamName(team), GetWinScore()-warning), IC28, 0, 0, 0, "Info_Alarm.ogg");
			}
		}
	}

	//Nach Gewinner prüfen
	// TODO
	//if (!EffectVar(0, Par(), Par(1)))
	//	EffectVar(0, Par(), Par(1)) = IsFulfilled();
}

/* Flaggenverhalten */

public func /* check */ SetFlag(object flagpost)
{
	if (!flagpost || !flagpost->~IsFlagpole()) return;
	goal_flagpost = flagpost;
	return !!goal_flagpost;
}

public func /* check */ GetFlag()
{
	return goal_flagpost;
}

public func /* check */ FlagLost(object flagpost, int iOldTeam, int iNewTeam, array aAttackers)
{
	//Ist es die Flagge?
	if (flagpost != goal_flagpost)
		return;
	//Punkte für die Angreifer
	/* TODO
	for (var clonk in aAttackers)
	{
		//Punkte bei Belohnungssystem (Flaggenposten neutralisiert)
		if (clonk)
			DoPlayerPoints(BonusPoints("OPNeutralization"), RWDS_TeamPoints, clonk->GetOwner(), clonk, IC13);
	}
	*/
	for (var i; i < GetPlayerCount(); i++)
		if (GetPlayerTeam(GetPlayerByIndex(i)) == iOldTeam)
			//Eventnachricht: Flaggenposten verloren
			EventInfo4K(GetPlayerByIndex(i)+1, Format("$MsgFlagLost$", GetName(goal_flagpost), GetTeamFlagColor(iNewTeam), GetTeamName(iNewTeam)), IC13, 0, GetTeamFlagColor(iNewTeam), 0, "Info_Event.ogg");
}

public func /* check */ FlagCaptured(object flagpost, int iTeam, array aAttackers, bool fRegained)
{
	//Ist es die Flagge?
	if (flagpost != goal_flagpost)
		return;

	var first = true; //Der erste bekommt mehr Punkte, der Rest bekommt Assistpunkte
	for (var clonk in aAttackers)
		if (clonk)
		{
			/* TODO
			if (fRegained)
				//Punkte bei Belohnungssystem (Flaggenposten verteidigt)
				DoPlayerPoints(BonusPoints("OPDefense"), RWDS_TeamPoints, clonk->GetOwner(), clonk, IC12);
			else
			{
				if (first)
					//Punkte bei Belohnungssystem (Flaggenposten erobert)
					DoPlayerPoints(BonusPoints("OPConquest"), RWDS_TeamPoints, clonk->GetOwner(), clonk, IC10);
				else
					//Punkte bei Belohnungssystem (Hilfe bei Flaggenposteneroberung)
					DoPlayerPoints(BonusPoints("OPConquestAssist"), RWDS_TeamPoints, clonk->GetOwner(), clonk, IC11);
			}
			*/
			first = false;
		}

	//Eventnachricht: Flaggenposten erobert
	EventInfo4K(0, Format("$MsgCaptured$", GetTeamFlagColor(iTeam), GetTeamName(iTeam), GetName(goal_flagpost)), IC10, 0, GetTeamFlagColor(iTeam), 0, "Info_Objective.ogg");
	UpdateScoreboard();
}

/* Scoreboard */

static const GHTF_FlagColumn		= 1;
static const GHTF_ProgressColumn	= 2;
static const GHTF_PointsColumn		= 3;
static const GHTF_FlagRow		= 1024;

public func /* check */ InitScoreboard()
{
	//Wird noch eingestellt
//	if (FindObject(CHOS) || IsFulfilled()) return;
/*
	UpdateHUDs();

	//Titelzeile
	SetScoreboardData(SBRD_Caption, SBRD_Caption, Format("%s",GetName()), SBRD_Caption);

	//Spaltentitel
	SetScoreboardData(SBRD_Caption, GHTF_FlagColumn, "{{IC12}}", SBRD_Caption);
	SetScoreboardData(SBRD_Caption, GHTF_ProgressColumn, " ", SBRD_Caption);
	SetScoreboardData(SBRD_Caption, GHTF_PointsColumn, "{{SM02}}", SBRD_Caption);

	UpdateScoreboard();
*/
}

public func /* check */ UpdateScoreboard()
{
/*
	//Wird noch eingestellt
	if (FindObject(CHOS) || !goal_flagpost || fulfilled) return;

	//Zeileniterator
	var i = 0;

	//Teamfarbe und Flaggenzustand ermitteln
	var teamclr = GetTeamFlagColor(goal_flagpost->GetTeam()),
	prog = goal_flagpost->GetProcess();
	var percentclr = InterpolateRGBa3(RGBa(255, 255, 255), teamclr, prog, 100);

	//Flaggennamenfarbe ermitteln
	if (!goal_flagpost->~IsFullyCaptured())
		var nameclr = RGB(255,255,255);
	else
		var nameclr = teamclr;
		
	//Flaggenicon ermitteln
	var icon, trend = goal_flagpost->GetTrend();
	if (!trend)		icon = SM21;	//Keine Aktivität
	if (trend == -1)	icon = SM23;	//Angriff
	if (trend == 1)	icon = SM22;	//Verteidigung

	SetScoreboardData(i, GHTF_FlagColumn, Format("<c %x>%s</c>", nameclr, GetName(goal_flagpost)));
	SetScoreboardData(i, GHTF_ProgressColumn, Format("{{%i}}", icon), GHTF_FlagRow-i);
	SetScoreboardData(i, GHTF_PointsColumn, Format("<c %x>%d%</c>", percentclr, prog), GHTF_FlagRow-i);

	i++;

	//Leere Zeile
	SetScoreboardData(i, GHTF_FlagColumn, " ");
	SetScoreboardData(i, GHTF_ProgressColumn, " ", GHTF_FlagRow-i);
	SetScoreboardData(i, GHTF_PointsColumn, " ", GHTF_FlagRow-i);

	i++;

	//Benötigte Punktzahl
	SetScoreboardData(i, GHTF_FlagColumn, "$SbrdGoalPoints$");
	SetScoreboardData(i, GHTF_ProgressColumn, " ", GHTF_FlagRow-i);
	SetScoreboardData(i, GHTF_PointsColumn, Format("%d", iGoal), GHTF_FlagRow-i);

	i++;

	//Leere Zeile
	SetScoreboardData(i, GHTF_FlagColumn, " ");
	SetScoreboardData(i, GHTF_ProgressColumn, " ", GHTF_FlagRow-i);
	SetScoreboardData(i, GHTF_PointsColumn, " ", GHTF_FlagRow-i);

	i++;

	//Icons
	SetScoreboardData(i, GHTF_FlagColumn, "{{SM26}}");
	SetScoreboardData(i, GHTF_ProgressColumn, "{{SM27}}", GHTF_FlagRow-i);
	SetScoreboardData(i, GHTF_PointsColumn, "{{IC28}}", GHTF_FlagRow-i);

	i++;

	for (var j = 0; j < GetTeamCount(); j++)
	{
		var iTeam = GetTeamByIndex(j);

		if (GetTeamName(iTeam) && GetTeamPlayerCount(iTeam))
		{
			i++;

			SetScoreboardData(i, GHTF_FlagColumn, Format("<c %x>%s</c>", GetTeamFlagColor(iTeam), GetTeamName(iTeam)));

			if (iTeam == goal_flagpost->~GetTeam())
				SetScoreboardData(i, GHTF_ProgressColumn, Format("<c %x>%d</c>", RGB(128, 128, 128), goal_progress), goal_progress);
			else
				SetScoreboardData(i, GHTF_ProgressColumn, Format("<c %x>%d</c>", RGB(128, 128, 128), 0), 0);

			SetScoreboardData(i, GHTF_PointsColumn, Format("<c ffbb00>%d</c>", aTeamPoints[iTeam]), aTeamPoints[iTeam]);
		}
	}

	//Sortieren
	SortScoreboard(GHTF_ProgressColumn, true);
	SortScoreboard(GHTF_PointsColumn, true);
*/
}

public func /* check */ GetTeamFlagColor(int iTeam)
{
	//Team hat mehrere Spieler: Teamfarbe
	if (GetTeamPlayerCount(iTeam) != 1)
		return GetTeamColor(iTeam);
	//Ein Spieler: Dessen Spielerfarbe
	for (var i; i < GetPlayerCount(); i++)
		if (GetPlayerTeam(GetPlayerByIndex(i)) == iTeam)
			return GetPlayerColor(GetPlayerByIndex(i));
	return GetTeamColor(iTeam);
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

func check TeamGetScore(int iTeam)
{
	return aTeamPoints[iTeam];
}
*/

func EventInfo4K()
{
	// TODO
}

func UpdateHUDs()
{
	// TODO
}

// TODO
static const IC28 = Rock;
static const IC10 = Rock;
static const IC12 = Rock;
static const IC13 = Rock;
