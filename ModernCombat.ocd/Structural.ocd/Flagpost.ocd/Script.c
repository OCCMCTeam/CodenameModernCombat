/**
	Flag Post
*/

/* --- Properties --- */

local deploy_location; // Deployment location next to the flag; Might be changed, so that the flag itself is a location, instead of having a helper object?

local team;
local process;
local range;
local flag;
local bar;
local attacker;
local spawnpoints;
local trend;
local capt;
local pAttackers;
local lastowner;
local iconState;
local captureradiusmarker;
local noenemys;
local nofriends;

/* --- Interface --- */

public func GetAttacker()		{ return attacker; }
public func GetTeam()			{ return team; }
public func GetProcess()		{ return process; }
public func GetTrend()			{ return trend; }
public func GetRange()			{ return range; }
public func IsFullyCaptured()	{ return capt; }

public func StandardRange()		{ return 100; }		//Standardreichweite
public func StandardSpeed()		{ return 2; }		//Standardeinnahmegeschwindigkeit

public func IsFlagpole()		{ return true; }		//Ist ein Flaggenposten
public func IsSpawnable()		{ return true; }		//Einstiegspunkt

static const BAR_FlagBar = 5;


/* --- Engine Callbacks --- */

func Initialize()
{
	spawnpoints = [];
	pAttackers = [];

	// Start neutral
	lastowner = 0;

	// Set defaults
	Set();

	// Create Flag
	flag = flag ?? CreateObject(CMC_FlagPost_Flag);

	// Update HUD
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


public func GetDeployLocation()
{
	if (!deploy_location)
	{
		deploy_location = CreateObject(CMC_DeployLocation, 0, -50, NO_OWNER);
	}
	return deploy_location;
}


public func SetPosition(int x, int y, bool check_bounds)
{
	if (deploy_location)
	{
		deploy_location->SetPosition(x, y - 50, check_bounds);
	}
	return inherited(x, y, check_bounds, ...);
}


public func SetHoldTheFlag(string name)
{
	GetDeployLocation()->AddRelaunchLocation(GetX(), GetY() - 30)->SetName(name ?? "Alpha");
}


/* Einstellungen */

public func /* check */ Set(string szName, int iRange, int iSpeed, int iValue)
{
	//Name setzen
	GetDeployLocation()->SetName(szName ?? "Alpha");

	//Reichweite setzen
	if (!iRange) iRange = StandardRange();
	range = iRange;

	//Einnahmegeschwindigkeit setzen
	if (!iSpeed) iSpeed = StandardSpeed();

	//Prüfungseffekt einrichten
	RemoveTimer(this.Timer);
	AddTimer(this.Timer, iSpeed);
}


/* Flaggenzustände */

public func /* check */ IsAttacked()
{
	for (clonk in FindObjects(Find_Distance(range),Find_OCF(OCF_Alive)))
	{
		if (clonk->Contained() && !clonk->Contained()->~IsHelicopter()) continue;
		if (clonk->GetOwner() == NO_OWNER) continue;
		if (GetPlayerTeam(clonk->GetOwner()) != team)
			return true;
	}

	return false;
}

public func /* check */ IsCaptured(bool pBool)
{
	capt = pBool;
}

func /* check */ ResetAttackers()
{
	pAttackers = CreateArray();
}

/* Prüfungseffekt und -timer */


/* Umkreis-Effekt */

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

func /* check */ Timer()
{
	var enemys,friends,opposition;

	//Momentanen Zustand speichern
	var iOld = trend;
	trend = 0;

	//Zuvor gespeicherte Clonks in Reichweite auf Aktualität prüfen
	var del;
	var clonks = FindObjects(Find_Distance(range),Find_OCF(OCF_Alive));
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
		if (GetPlayerTeam(clonk->GetOwner()) == team)
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
		DoProcess(opposition,Min(enemys,3));
	//Nur Verbündete: Flaggeneroberung vorrantreiben
	if (!enemys && friends)
		DoProcess(team,Min(friends,3));

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
		if (!captureradiusmarker && nofriends && process < 100)
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
				var clr = GetTeamColor(team), plr;
				if ((GetTeamConfig(TEAM_AutoGenerateTeams) && GetTeamPlayerCount(team) <= 1 && (plr = GetTeamMemberByIndex(team, 0)) > -1) || !GetTeamConfig(TEAM_TeamColors))
					clr = GetPlayerColor(plr);

				bar->SetIcon(0, SM23, 0, 0, 32);
				bar->SetBarColor(clr);
				bar->Update(process);
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

public func /* check */ Capture(int iTeam, bool bSilent)
{
	process = 100;
	attacker = 0;
	team = iTeam;
	capt = true;
	var fRegained = false;
	if (!bSilent)
	{
		if (lastowner == team) fRegained = true;
		GameCallEx("FlagCaptured", this, team, pAttackers, fRegained);
	}
	ResetAttackers();
	lastowner = team;
	UpdateFlag();
}

func /* check */ Capturing(int iTeam)
{
	attacker = iTeam;
}

public func /* check */ NoTeam()
{
	team = 0;
	process = 0;
	attacker = 0;
	capt = false;
	UpdateFlag();
}

/* Flaggenkonfiguration */

public func /* check */ UpdateFlag()
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

	//Entsprechend dem Besitzer färben
	if (team)
	{
		flag->SetColor(RGB(0,0,0));
		for (var i = 0; i < GetPlayerCount(); i++)
		{
			if (GetPlayerTeam(GetPlayerByIndex(i)) != team) continue;
			flag->SetOwner(GetPlayerByIndex(i));
			break;
		}
	}
	else
	{
		flag->SetOwner(NO_OWNER);
		flag->SetColor(RGB(255, 255, 255));
	}

	//Flaggenposition aktualisieren
	SetFlagPos(process);
}


// Lifts the flag up from the bottom, position in percent
func SetFlagPos(int percent)
{
	if (!flag) return;

	var flag_height = flag->GetID()->GetDefHeight();
	var maximum = GetDefHeight() - flag_height;
	var height = (percent * maximum / 100) + flag_height / 2;
	flag->SetPosition(GetX(), GetY() - height);
}

/* Einnahme/Neutralisierung umsetzen */

public func /* check */ DoProcess(int iTeam, int iAmount)
{
	var old = process;

	//Eventuelle Gegnerflagge abnehmen
	if (team)
	{
		if (iTeam != team && (process != 0))
			iAmount = -iAmount;
	}
	else
		team = iTeam;

	process = BoundBy(process+iAmount,0,100);

	if (old < process)
		trend = +1;

	if (old > process)
		trend = -1;

	if ((old == 100 && trend < 0) || (old == 0 && trend > 0))
	{
		GameCallEx("FlagAttacked", this, team, pAttackers);
	}

	//Flagge wird übernommen
	if (process < 100 && trend != 0)
	{
		Capturing(iTeam);
	}

	//Flagge ist fertig übernommen
	if ((process >= 100) && (old < 100))
	{
		Capture(iTeam);
	}

	//Neutrale Flagge
	if ((process <= 0) && (old > 0))
	{
		if (team && lastowner != iTeam) GameCallEx("FlagLost", this, team, iTeam, pAttackers);
		//lastowner = team;
		attacker = 0;
		capt = false;
		team = iTeam;
	}

	UpdateFlag();

	var clr = GetTeamColor(iTeam);
	var plr = GetTeamMemberByIndex(iTeam, 0);
	if ((GetTeamConfig(TEAM_AutoGenerateTeams) && GetTeamPlayerCount(iTeam) <= 1 && plr > -1) || !GetTeamConfig(TEAM_TeamColors))
		clr = GetPlayerColor(plr);

	if (bar)
	{
		bar->SetBarColor(clr);
		if (process >= 100)
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
			bar->Update(process);
	}

	return process;
}

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
		NoTeam();

	//Reichweite setzen
	if (iRange) range = iRange;

	//Spawnpunkte anpassen
	var curX = GetX(), curY = GetY();
	for (var i = 0; i < GetLength(spawnpoints); i++)
	{
		spawnpoints[i][0] -= iX - curX;
		spawnpoints[i][1] -= iY - curY;
	}

	//Verschieben und einblenden
	SetPosition(iX, iY);
	UpdateFlag();
	FadeIn();
	flag->FadeIn();
}




global func GetTeamMemberByIndex(int team, int index)
{
	var member_index;
	for (var i = 0; i < GetPlayerCount(); ++i)
	{
		if (GetPlayerTeam(GetPlayerByIndex(i)) == team)
		{
			if (member_index == index)
				return GetPlayerByIndex(i);
			
			++member_index;
		}
	}
	
	return -1;
}

static const SM21 = Rock;
static const SM22 = Rock;
static const SM23 = Rock;
