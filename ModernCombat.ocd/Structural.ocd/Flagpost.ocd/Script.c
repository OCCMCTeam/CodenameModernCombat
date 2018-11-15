/**
	Flag Post
*/

/* --- Properties --- */
static const BAR_FlagBar = 5;

local deploy_location; // Deployment location next to the flag; Might be changed, so that the flag itself is a location, instead of having a helper object?
local goal_object;     // Goal that is linked with this flag

local capture_team;
local capture_progress;
local range;
local flag;
local bar;
local attacker;
local trend;
local capt;
local pAttackers;
local lastowner;
local iconState;
local captureradiusmarker;
local noenemys;
local nofriends;


local FlagPost_DefaultRange = 100;
local FlagPost_DefaultSpeed = 2;


/* --- Interface --- */

public func GetAttacker()		{ return attacker; }
public func GetTeam()			{ return capture_team; }
public func GetProgress()		{ return capture_progress; }
public func GetTrend()			{ return trend; }
public func GetRange()			{ return range; }
public func IsFullyCaptured()	{ return capt; }




public func GetDeployLocation()
{
	if (!deploy_location)
	{
		deploy_location = CreateObject(CMC_DeployLocation, 0, -50, NO_OWNER);
	}
	return deploy_location;
}

public func IsFlagpole()		{ return true; }		//Ist ein Flaggenposten

public func SetCaptureRange(int value)
{
	range = value ?? FlagPost_DefaultRange;
	return this;
}

public func SetCaptureSpeed(int value)
{
	RemoveTimer(this.CaptureTimer);
	AddTimer(this.CaptureTimer, value ?? FlagPost_DefaultSpeed);
	return this;
}


public func SetHoldTheFlag() // Fixed setting for HTF: one deploy location
{
	var name = GetName();
	if (name == "$Name$")
	{
		name = "Alpha";
	}
	GetDeployLocation()->AddRelaunchLocation(GetX(), GetY() - 30)->SetName(name);
	return this;
}

public func RegisterGoal(object goal)
{
	goal_object = goal;
}


/* --- Engine Callbacks --- */

func Initialize()
{
	// Set defaults
	pAttackers = [];
	lastowner = 0;

	SetCaptureRange();
	SetCaptureSpeed();

	// Create flag and update HUD
	flag = flag ?? CreateObject(CMC_FlagPost_Flag);
	UpdateFlag();
}


func Destruction()
{
	if (deploy_location)
	{
		deploy_location->RemoveObject();
	}
	_inherited(...);
}


public func SetPosition(int x, int y, bool check_bounds)
{
	if (deploy_location)
	{
		deploy_location->SetPosition(x, y - 50, check_bounds);
	}
	return inherited(x, y, check_bounds, ...);
}

/* --- Capturing logic --- */

func CaptureTimer()
{
	var enemys, friends, opposition;

	//Momentanen Zustand speichern
	var iOld = trend;
	trend = 0;

	//Zuvor gespeicherte Clonks in Reichweite auf Aktualität prüfen
	var del;
	var clonks = FindObjects(Find_Distance(range), Find_OCF(OCF_Alive));
	for (var pClonk in pAttackers)
	{
		del = true;
		for (var clonk in clonks)
		{
			if (clonk == pClonk)
			{
				if (pClonk->Contained() && !pClonk->Contained()->~IsHelicopter()) continue;

				//Clonk vorhanden: Eintrag beibehalten
				del = false;
				break;
			}
		}
		//Clonk nicht vorhanden: Eintrag entfernen
		if (del)
			pAttackers[GetIndexOf(pAttackers, pClonk)] = 0;
	}

	//Leere Einträge entfernen
	RemoveHoles(pAttackers);

	var aFriends = CreateArray();
	var aEnemies = CreateArray();

	//Passende Clonks in Reichweite ermitteln
	var clonks = FindObjects(Find_Distance(range),Find_OCF(OCF_Alive));

	//Gefundene Clonks als Feinde oder Verbündete einstufen
	for (clonk in clonks)
	{
		if (clonk->Contained() && !clonk->Contained()->~IsHelicopter()) continue;
		if (clonk->GetOwner() == NO_OWNER) continue;
		if (!GetPlayerName(clonk->GetOwner()) || !GetPlayerTeam(clonk->GetOwner())) continue;
		if (!PathFree(this->GetX(),this->GetY()-GetID()->GetDefHeight()/2,clonk->GetX(),clonk->GetY())) continue;
		if (GetPlayerTeam(clonk->GetOwner()) == capture_team)
		{
			friends++;
			aFriends[GetLength(aFriends)] = clonk;
		}
		else
		{
			enemys++;
			opposition = GetPlayerTeam(clonk->GetOwner());
			aEnemies[GetLength(aEnemies)] = clonk;
		}
	}
	attacker = opposition;

	//Zustandsänderung ermitteln
	//Nur Feinde: Flaggenneutralisierung vorrantreiben
	if (enemys && !friends)
		DoProgress(opposition,Min(enemys,3));
	//Nur Verbündete: Flaggeneroberung vorrantreiben
	if (!enemys && friends)
		DoProgress(capture_team,Min(friends,3));

	if (enemys)
	{
		if (!captureradiusmarker && noenemys)
		{
			captureradiusmarker = ShowCaptureRadius(this);
			noenemys = false;
		}
	}
	else
		noenemys = true;

	if (friends)
	{
		if (!captureradiusmarker && nofriends && capture_progress < 100)
			captureradiusmarker = ShowCaptureRadius(this);

		nofriends = false;
	}
	else
		nofriends = true;

	if ((!enemys) == (!friends))
	{
		if (!friends)
		{
			if (iconState != 0 && bar) // TODO: Added && bar must be checked
			{
				bar->SetIcon(0, SM21, 0, 0, 32);
				bar->Update(0, true, true);
				iconState = 0;
			}
		}
		else
		{
			if (iconState != 2 && bar) // TODO: Added && bar must be checked
			{
				var clr = GetTeamColor(capture_team), plr;
				if ((GetTeamConfig(TEAM_AutoGenerateTeams) && GetTeamPlayerCount(capture_team) <= 1 && (plr = GetTeamPlayer(capture_team, 0)) > -1) || !GetTeamConfig(TEAM_TeamColors))
					clr = GetPlayerColor(plr);

				bar->SetIcon(0, SM23, 0, 0, 32);
				bar->SetBarColor(clr);
				bar->Update(capture_progress);
				iconState = 2;
			}
		}
	}

	if (trend != iOld)
		ResetAttackers();

	var pClonks = CreateArray();
	if (trend < 0)
		pClonks = aEnemies;
	if (trend > 0)
		pClonks = aFriends;

	for (var clonk in pClonks)
	{
		if (!clonk) continue;
		var new = true;
		//Clonk auffindbar?
		for (var pClonk in pAttackers)
		{
			if (pClonk == clonk) new = false;
			if (!new) break;
		}
		//Neu: Einstellen
		if (new) pAttackers[GetLength(pAttackers)] = clonk;
	}
}

public func /* check */ DoProgress(int iTeam, int iAmount)
{
	var old = capture_progress;

	//Eventuelle Gegnerflagge abnehmen
	if (capture_team)
	{
		if (iTeam != capture_team && (capture_progress != 0))
			iAmount = -iAmount;
	}
	else
		capture_team = iTeam;

	capture_progress = BoundBy(capture_progress+iAmount,0,100);

	if (old < capture_progress)
		trend = +1;

	if (old > capture_progress)
		trend = -1;

	if ((old == 100 && trend < 0) || (old == 0 && trend > 0))
	{
		GameCallEx("FlagAttacked", this, capture_team, pAttackers);
	}

	//Flagge wird übernommen
	if (capture_progress < 100 && trend != 0)
	{
		StartCapturing(iTeam);
	}

	//Flagge ist fertig übernommen
	if ((capture_progress >= 100) && (old < 100))
	{
		DoCapture(iTeam);
	}

	//Neutrale Flagge
	if ((capture_progress <= 0) && (old > 0))
	{
		if (capture_team && lastowner != iTeam) GameCallEx("FlagLost", this, capture_team, iTeam, pAttackers);
		//lastowner = capture_team;
		attacker = 0;
		capt = false;
		capture_team = iTeam;
	}

	UpdateFlag();

	var clr = GetTeamColor(iTeam);
	var plr = GetTeamPlayer(iTeam, 0);
	if ((GetTeamConfig(TEAM_AutoGenerateTeams) && GetTeamPlayerCount(iTeam) <= 1 && plr > -1) || !GetTeamConfig(TEAM_TeamColors))
		clr = GetPlayerColor(plr);

	if (bar)
	{
		bar->SetBarColor(clr);
		if (capture_progress >= 100)
		{
			if (iconState != 0)
			{
				bar->SetIcon(0, SM21, 0, 0, 32);
				bar->Update(0, true, true);
				iconState = 0;
			}
		}
		else if (iconState != 1)
		{
			bar->SetIcon(0, SM22, 0, 0, 32);
			iconState = 1;
		}
		if (iconState != 0)
			bar->Update(capture_progress);
	}

	return capture_progress;
}

/* --- Status --- */

public func /* check */ IsAttacked()
{
	for (var crew in FindObjects(Find_Distance(range), Find_OCF(OCF_Alive)))
	{
		if (crew->Contained() && !crew->Contained()->~IsHelicopter()) continue;
		if (crew->GetOwner() == NO_OWNER) continue;
		if (GetPlayerTeam(crew->GetOwner()) != capture_team)
			return true;
	}

	return false;
}

func ResetAttackers()
{
	pAttackers = [];
}


func /* check */ StartCapturing(int iTeam)
{
	attacker = iTeam;
}


func /* check */ DoCapture(int iTeam, bool bSilent)
{
	capture_progress = 100;
	attacker = 0;
	capture_team = iTeam;
	capt = true;
	var fRegained = false;
	if (!bSilent)
	{
		if (lastowner == capture_team) fRegained = true;
		GameCallEx("FlagCaptured", this, capture_team, pAttackers, fRegained);
	}
	ResetAttackers();
	lastowner = capture_team;
	UpdateFlag();
}


func /* check */ SetNeutral()
{
	capture_team = 0;
	capture_progress = 0;
	attacker = 0;
	capt = false;
	UpdateFlag();
}

/* --- Display --- */

func UpdateFlag()
{
	if (!flag) return;
/* TODO
	// No status bar?
	if (!bar)
	{
		bar = CreateObject(SBAR, 0, 0, -1);
		bar->Set(this, RGB(255, 255, 255), BAR_FlagBar, 100, 0, SM21, 0, 0, true, true);
		bar->ChangeDefOffset(GetID()->GetDefOffset(1)+5);
		bar->SetIcon(0, SM21, 0, 0, 32);
		bar->Update(0, true, true);
		iconState = 0;
	}
	*/

	// Set color according to owner
	if (capture_team)
	{
		flag->SetColor(goal_object->GetFactionColor(capture_team));
	}
	else
	{
		flag->SetColor(RGB(255, 255, 255));
	}

	// Update the position
	SetFlagPosition(capture_progress);
}


// Lifts the flag up from the bottom, position in percent
func SetFlagPosition(int percent)
{
	if (!flag) return;

	var flag_height = flag->GetID()->GetDefHeight();
	var maximum = GetDefHeight() - flag_height;
	var height = (BoundBy(percent, 0, 100) * maximum / 100) + flag_height / 2;
	flag->SetPosition(GetX(), GetY() - height);
}


func /* check */ ShowCaptureRadius(object pTarget)
{
	//Kreis-Symbol erstellen
	/* TODO
	var obj = CreateObject(SM09, 0, 0, -1);
	obj->Set(pTarget);

	//Symbolgröße anpassen
	var wdt = range * 2000 / SM09->GetDefWidth();

	//Symbol konfigurieren
	obj->SetObjDrawTransform(wdt, 0, 0, 0, wdt, 0);
	obj->SetGraphics("Big");

	return obj;*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Temporary stuff below


/* Flaggenposten verschieben */

public func /* check */ MoveFlagpost(int iX, int iY, string szName, int iRange, bool fNeutral)
{
	//Effekte
	/* TODO
	for (var i = -80; i < -20; i += 10)
		CastParticles("MetalSplinter",1,20,0,i,50,80);
		
	if (GetEffectData(EFSM_ExplosionEffects) > 0) CastSmoke("Smoke3",8,15,0,-5,250,200,RGBa(255,255,255,100),RGBa(255,255,255,130));
	
	CastParticles("Sandbag", 10, 70, 0,-10, 35, 45, RGBa(228,228,228,0), RGBa(250,250,250,50));
	*/
	Sound("FenceDestruct.ogg");
	Sound("StructureHit*.ogg");
	Sound("StructureDebris*.ogg");

	//Namen ändern
	if (szName)
		SetName(szName);

	//Besitzer neutralisieren
	if (fNeutral)
		SetNeutral();

	//Reichweite setzen
	if (iRange) range = iRange;

	//Verschieben und einblenden
	SetPosition(iX, iY);
	UpdateFlag();
	FadeIn();
	flag->FadeIn();
}
