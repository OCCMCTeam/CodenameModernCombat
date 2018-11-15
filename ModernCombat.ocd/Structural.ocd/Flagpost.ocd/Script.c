/**
	Flag Post
*/

/* --- Properties --- */
static const BAR_FlagBar = 5;

local deploy_location; // Deployment location next to the flag; Might be changed, so that the flag itself is a location, instead of having a helper object?
local goal_object;     // Goal that is linked with this flag
local flag;            // The flag helper object
local bar;             // Status progress bar
local range_marker;

local capture_team;
local capture_progress;
local capture_range;
local capture_trend;
local attacking_team;
local attacking_crew;
local has_no_enemies;
local has_no_friends;

local last_owner;
local is_captured;
local icon_state;


local FlagPost_DefaultRange = 100;
local FlagPost_DefaultSpeed = 2;


/* --- Interface --- */

public func GetAttacker()		{ return attacking_team; }
public func GetTeam()			{ return capture_team; }
public func GetProgress()		{ return capture_progress; }
public func GetTrend()			{ return capture_trend; }
public func GetRange()			{ return capture_range; }
public func IsFullyCaptured()	{ return is_captured; }




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
	capture_range = value ?? FlagPost_DefaultRange;
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
	attacking_crew = [];
	last_owner = 0;

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
	if (range_marker)
	{
		range_marker->RemoveObject();
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
	var crew_in_range = GetCrewInRange();
	CheckAttackingCrew(crew_in_range);

	var friends_in_range = [];
	var enemies_in_range = [];

	//Gefundene Clonks als Feinde oder Verbündete einstufen
	for (var crew in crew_in_range)
	{
		var player = crew->GetOwner();
		if (player == NO_OWNER) continue;
		if (!GetPlayerName(player) || !GetPlayerTeam(player)) continue;
		if (!PathFree(this->GetX(), this->GetY() - (GetID()->GetDefHeight()/2), crew->GetX(), crew->GetY())) continue;
		
		if (GetPlayerTeam(player) == capture_team)
		{
			PushBack(friends_in_range, crew);
		}
		else
		{
			PushBack(enemies_in_range, crew);
		}
	}
	var friends = GetLength(friends_in_range);
	var enemies = GetLength(enemies_in_range);
	attacking_team = GetTeamMajority(enemies_in_range);
	
	var has_friends = friends > 0;
	var has_enemies = enemies > 0;

	//Zustandsänderung ermitteln
	//Nur Feinde: Flaggenneutralisierung vorrantreiben
	var max_progress = 3;
	capture_trend = 0;
	if (has_enemies && !has_friends)
	{
		DoProgress(attacking_team, Min(enemies, max_progress));
	}
	//Nur Verbündete: Flaggeneroberung vorrantreiben
	if (!has_enemies && has_friends)
	{
		DoProgress(capture_team, Min(friends, max_progress));
	}

	if (has_enemies)
	{
		if (has_no_enemies)
		{
			ShowCaptureRadius();
			has_no_enemies = false;
		}
	}
	else
	{
		has_no_enemies = true;
	}

	if (has_friends)
	{
		if (has_no_friends && capture_progress < 100)
		{
			ShowCaptureRadius();
		}
		has_no_friends = false;
	}
	else
	{
		has_no_friends = true;
	}

	if (has_enemies == has_friends)
	{
		if (has_friends)
		{
			SetIconState(0);
		}
		else
		{
			SetIconState(2, capture_team);
		}
	}
	UpdateAttackingCrew(enemies_in_range, friends_in_range);
}


func CheckAttackingCrew(array crew_in_range)
{
	for (var i = 0; i < GetLength(attacking_crew); ++i)
	{
		if (!IsValueInArray(crew_in_range, attacking_crew[i]))
		{
			attacking_crew[i] = nil;
		}
	}
	RemoveHoles(attacking_crew);
}


func UpdateAttackingCrew(array enemies_in_range, array friends_in_range)
{
	if (capture_trend < 0)
	{
		attacking_crew = enemies_in_range;
	}
	else if (capture_trend > 0)
	{
		attacking_crew = friends_in_range;
	}
}


public func /* check */ DoProgress(int team, int amount)
{
	var old_progress = capture_progress;

	//Eventuelle Gegnerflagge abnehmen
	if (capture_team)
	{
		if (team != capture_team && (capture_progress != 0))
		{
			amount = -amount;
		}
	}
	else
	{
		capture_team = team;
	}

	capture_progress = BoundBy(capture_progress + amount, 0, 100);
	capture_trend = BoundBy(capture_progress - old_progress, -1, +1);

	if ((old_progress == 100 && capture_trend < 0) || (old_progress == 0 && capture_trend > 0))
	{
		GameCallEx("FlagAttacked", this, capture_team, attacking_crew);
	}

	// Start capturing
	if (capture_progress < 100 && capture_trend != 0)
	{
		StartCapturing(team);
	}

	// Done capturing
	if ((capture_progress >= 100) && (old_progress < 100))
	{
		DoCapture(team);
	}

	// Neutral flag
	if ((capture_progress <= 0) && (old_progress > 0))
	{
		if (capture_team && last_owner != team)
		{
			GameCallEx("FlagLost", this, capture_team, team, attacking_crew);
		}
		attacking_team = 0;
		is_captured = false;
		capture_team = team;
	}

	UpdateFlag();

	if (bar)
	{
		if (capture_progress >= 100)
		{
			SetIconState(0);
		}
		else
		{
			SetIconState(1, team);
		}
	}

	return capture_progress;
}

/* --- Status --- */


public func GetCrewInRange()
{
	var crew = FindObjects(Find_Distance(capture_range), Find_OCF(OCF_Alive));
	for (var i = 0; i < GetLength(crew); ++i)
	{
		var member = crew[i];
		if (member->Contained() && !member->Contained()->~IsHelicopter())
		{
			crew[i] = nil;
		}
	}
	RemoveHoles(crew);
	return crew;
}


public func GetTeamMajority(array crew)
{
	// Determine the team with most healthy members
	var team_strength = [];
	for (var member in crew)
	{
		var team = GetPlayerTeam(member->GetOwner());
		if (team > 0)
		{
			team_strength[team] += member->GetEnergy();
		}
	}
	return GetMaxValueIndices(team_strength)[0];
}


public func /* check */ IsAttacked()
{
	for (var crew in GetCrewInRange())
	{
		if (crew->GetOwner() == NO_OWNER) continue;
		if (GetPlayerTeam(crew->GetOwner()) != capture_team)
			return true;
	}

	return false;
}


func /* check */ StartCapturing(int team)
{
	attacking_team = team;
}


func /* check */ DoCapture(int team, bool silent)
{
	capture_progress = 100;
	attacking_team = 0;
	capture_team = team;
	is_captured = true;
	var regained = false;
	if (!silent)
	{
		if (last_owner == capture_team)
		{
			regained = true;
		}
		GameCallEx("FlagCaptured", this, capture_team, attacking_crew, regained);
	}
	attacking_crew = [];
	last_owner = capture_team; // FIXME: This should be done BEFORE reassigning the team...
	UpdateFlag();
}


func /* check */ SetNeutral()
{
	capture_team = 0;
	capture_progress = 0;
	attacking_team = 0;
	is_captured = false;
	UpdateFlag();
}

/* --- Display --- */

func SetIconState(int state, int team)
{
	if (state == icon_state) return; // No change, no update

	icon_state = state;
	if (!bar) return;

	if (state == 0)
	{
		bar->SetIcon(0, SM21, 0, 0, 32);
		bar->Update(0, true, true);
	}
	else
	{
		if (team)
		{
			var color = goal_object->GetFactionColor(team);
			bar->SetBarColor(color);
		}
		bar->Update(capture_progress);
		if (state == 1)
		{
			bar->SetIcon(0, SM22, 0, 0, 32);
		}
		else if (state == 2)
		{
			bar->SetIcon(0, SM23, 0, 0, 32);
		}
	}
}

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
		icon_state = 0;
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


func ShowCaptureRadius()
{
	if (range_marker) return;
	
	// Create new one
	range_marker = CMC_Icon_SensorBall_Circle->AddTo(this);

	// Adjust to size
	var scale = capture_range * 2000 / CMC_Icon_SensorBall_Circle->GetDefWidth();
	range_marker->SetObjDrawTransform(scale, 0, 0, 0, scale, 0);
	range_marker->SetGraphics("Wide");
	range_marker->SetColor(RGB(255, 255, 255));
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
	if (iRange) capture_range = iRange;

	//Verschieben und einblenden
	SetPosition(iX, iY);
	UpdateFlag();
	FadeIn();
	flag->FadeIn();
}
