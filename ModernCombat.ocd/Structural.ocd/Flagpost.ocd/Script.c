/**
	Flag Post
*/

/* --- Properties --- */

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
local has_deployment;

local last_owner;
local is_captured;


local FlagPost_DefaultRange = 100;
local FlagPost_DefaultSpeed = 2;

local FlagPost_Flag_Color_Neutral = 0xffffffff;
local FlagPost_Bar_Color_Back = 0x80ffffff;


/* --- Interface --- */

public func GetAttacker()		{ return attacking_team; }
public func GetTeam()			{ return capture_team; }
public func GetProgress()		{ return capture_progress; }
public func GetTrend()			{ return capture_trend; }
public func GetRange()			{ return capture_range; }
public func IsFullyCaptured()	{ return is_captured; }

public func IsFlagpole()		{ return true; }


public func GetDeployLocation()
{
	if (has_deployment && !deploy_location)
	{
		deploy_location = CreateObject(CMC_DeployLocation, 0, -50, NO_OWNER);
		deploy_location->AddCondition(DefineCallback(this.IsAvailableForDeployment));
	}
	return deploy_location;
}


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
/*
	var name = GetName();
	if (name == "$Name$")
	{
		name = "Alpha";
	}
	GetDeployLocation()->AddRelaunchLocation(GetX(), GetY() - 30)->SetName(name);
*/
	has_deployment = false;
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
	has_deployment = true;

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
	// Update attackers that are not in range; The system is a little strange though
	// the attackers should not need concatenation at all, see UpdateAttackingCrew
	var crew_in_range = GetCrewInRange();
	CheckAttackingCrew(crew_in_range);

	var friends_in_range = [];
	var enemies_in_range = [];

	// Sort by hostility
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
	
	var has_friends = friends > 0;
	var has_enemies = enemies > 0;
	
	attacking_team = GetTeamMajority(enemies_in_range);

	// Only enemies in range? Neutralize the flag
	var max_progress = 3;
	capture_trend = 0;
	if (has_enemies && !has_friends)
	{
		DoProgress(attacking_team, Min(enemies, max_progress));
	}
	// Only allies in range? Continue capturing
	if (!has_enemies && has_friends)
	{
		DoProgress(capture_team, Min(friends, max_progress));
	}

	UpdateStatusDisplay(has_enemies, has_friends);
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


func UpdateStatusDisplay(bool has_enemies, bool has_friends)
{
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
			SetIconState(CMC_Icon_FlagPost_Neutral);
		}
		else
		{
			SetIconState(CMC_Icon_FlagPost_Embattled, capture_team);
		}
	}
}


public func /* check */ DoProgress(int team, int amount)
{
	var old_progress = capture_progress;

	// Neutralize a hostile flag?
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
	if ((capture_progress == 100) && (old_progress < 100))
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

	if (capture_progress >= 100)
	{
		SetIconState(CMC_Icon_FlagPost_Neutral);
	}
	else
	{
		SetIconState(CMC_Icon_FlagPost_Capturing, team);
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


func StartCapturing(int team)
{
	attacking_team = team;
}


func DoCapture(int team, bool silent)
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


func SetNeutral() // Used only by MoveFlagpost, so might be removed
{
	capture_team = 0;
	capture_progress = 0;
	attacking_team = 0;
	is_captured = false;
	UpdateFlag();
}

/* --- Display --- */

func SetIconState(id state, int team)
{
	SetGraphics(nil, state, 1, GFXOV_MODE_IngamePicture);
	SetObjDrawTransform(500, 0, 0, 0, 500, 1000 * (GetID()->GetDefOffset(1) - 30), 1);
}

func UpdateFlag()
{
	if (!flag) return;

	// Set color according to owner
	var color;
	if (capture_team)
	{
		color = SetRGBaValue(goal_object->GetFactionColor(capture_team), 255, RGBA_ALPHA);
	}
	else
	{
		color = FlagPost_Flag_Color_Neutral;
	}

	// Update the position
	flag->SetColor(color);
	SetFlagPosition(capture_progress);
	
	// No status bar?
	if (!bar)
	{
		bar = CreateProgressBar(GUI_ShadedSimpleProgressBar, 100, 0, 0, NO_OWNER, {x = 0, y = (GetID()->GetDefOffset(1) - 10)}, VIS_All);
	}

	bar->SetBarColor(InterpolateRGBa(capture_progress, 0, FlagPost_Flag_Color_Neutral, 100, color, FlagPost_Bar_Color_Back));
	bar->SetValue(capture_progress);
	
	if (GetDeployLocation())
	{
		if (capture_team)
		{
			GetDeployLocation()->SetTeam(capture_team); // Also calls UpdateMenuIcon
		}
		else
		{
			GetDeployLocation()->UpdateMenuIcon();
		}
	}
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

func GetScoreboardInfo()
{
	var team_color = goal_object->GetFactionColor(capture_team);
	var flag_name_color = RGB(255, 255, 255);
	var flag_name = GetName();
	var capture_color = InterpolateRGBa(capture_progress, 0, RGBa(255, 255, 255, 255), 100, team_color);
	if (is_captured)
	{
		flag_name_color = team_color;
	}
	
	var flag_status_icon = CMC_Icon_FlagPost_Neutral;
	if (capture_trend == -1) flag_status_icon = CMC_Icon_FlagPost_Embattled; // Attack
	if (capture_trend == +1) flag_status_icon = CMC_Icon_FlagPost_Capturing; // Defense

	return {
		name = Format("<c %x>%s</c>", flag_name_color, flag_name),
		status = Format("{{%i}}", flag_status_icon),
		score = Format("<c %x>%d%</c>", capture_color, capture_progress),
	};
}


func IsAvailableForDeployment(int player)
{
	return capture_team == GetPlayerTeam(player);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Temporary stuff below


/* Flaggenposten verschieben */

public func /* check */ MoveFlagpost(int x, int y, string name, int range, bool set_neutral)
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

	if (name)
	{
		SetName(name);
	}

	if (set_neutral)
	{
		SetNeutral();
	}

	//Reichweite setzen
	if (range) capture_range = range;

	//Verschieben und einblenden
	SetPosition(x, y);
	UpdateFlag();
	FadeIn();
	flag->FadeIn();
}
