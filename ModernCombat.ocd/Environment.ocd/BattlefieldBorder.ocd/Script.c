
/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local Visibility = VIS_Editor; // Only visible in Editor Mode

local area = [-20, -20, 40, 40]; // Area to check
local zonetype = 0; // Type of zone. 0 = Outside of the battlefield, 1 = Spawn area for specific team, 2 = Instant death zone
local team = 0; // Team for Spawn area

static const ZONETYPE_NORMAL = 0;
static const ZONETYPE_TEAMSPAWN = 1;
static const ZONETYPE_INSTANTDEATH = 2;


/* --- Functionality --- */

// Effect for zone checks
local fxzonecheck = new Effect
{
	Timer = func()
	{
		var objs;

		if (Target.zonetype == ZONETYPE_NORMAL)
		{
			// Outside Battlefield. Find everything that is in this area.
			objs = Target->FindObjects(Target->Find_InZone(), Target->Find_Target());

			for (var obj in objs)
			{
				if (!GetEffect("DeathZoneTimer", obj)) obj->CreateEffect(Target.fxinsidedeathzone,1,1, { zoneobj = Target } );
			}
		}
		else if (Target.zonetype == ZONETYPE_TEAMSPAWN)
		{
			// Team Spawn. Find everything but clonks from the specified team.
			objs = Target->FindObjects(Target->Find_InZone(), Target->Find_Target(), Find_Not(Find_Team(Target.team)));

			for (var obj in objs)
			{
				if (!GetEffect("DeathZoneTimer", obj)) obj->CreateEffect(Target.fxinsidedeathzone,1,1, { zoneobj = Target } );
			}
		}
		else if (Target.zonetype == ZONETYPE_INSTANTDEATH)
		{
			// Instant Death Zone. Kill everything instantaneously in this area.
			objs = Target->FindObjects(Target->Find_InZone(), Target->Find_Target());

			// TODO: Maybe adjust killtracing if needed
			for (var obj in objs)
			{
				obj->Kill(); // Kill clonk
			}
		}
	}
};


// Effect applied to clonks that entered a death zone
local fxinsidedeathzone = new Effect
{
	Name = "DeathZoneTimer",

	MaxAreaTime = 35 * 10, // 35 Frames per second * 10 Seconds

	zoneobject = nil,

	Start = func(temp, params)
	{
		zoneobject = params.zoneobj;
	},

	Timer = func()
	{
		// Remove effect from dead or respawning targets
		if (!Target->GetAlive() || Target->~IsRespawning())
		{
			return FX_Execute_Kill;
		}


		// Remove Effect if clonk leaves the area
		// The combination of Find_Not and Find_Exclude reduces the possible objects to find to just the clonk. We only want to check for this clonk if it's still inside the area.
		// If we can't find it here, it has moved out of the area.
		if (!zoneobject->FindObject(Find_Not(Find_Exclude(Target)), zoneobject->Find_InZone()))
		{
			return FX_Execute_Kill;
		}

		// Show remaining time
		Target->PlayerMessage(Target->GetOwner(), "$GoBack$", (MaxAreaTime - Time) / 35);

		if (Time > MaxAreaTime)
		{
			// TODO: Maybe adjust killtracing if needed
			Target->Kill();
		}
	},

	Stop = func (int temp)
	{
		if (!temp && Target)
		{
			Target->PlayerMessage(Target->GetOwner(), "");
		}
	},
};


func Find_InZone()
{
	return Find_InRect(area[0], area[1], area[2], area[3]);
}


func Find_Target()
{
	return Find_And(Find_Func("IsClonk"), Find_Func("GetAlive"), Find_Not(Find_Func("IsRespawning")));
}

/* --- Engine callbacks --- */


func Initialize()
{
	CreateEffect(fxzonecheck,1,1);
}


func Definition(def)
{
	// EditorProps
	if (!def.EditorProps) def.EditorProps = {};

	// Area to check for clonks
	def.EditorProps.area = 
	{ 
		Name="$Area$",
		Type="rect",
		Value=
		{
			Rect = [-20, -20, 40, 40]
		},
		ValueKey = "Rect",
		Color = RGB(255,0,0),
		Relative = true,
		Set = "SetAreaRect",
		SetRoot = false
	};

	// Type of area
	def.EditorProps.zonetype =
	{
		Name="$ZoneType$",
		Type="enum",
		Options =
		[
			{ Name = "$OutsideBattlefield$", Value = ZONETYPE_NORMAL },
			{ Name = "$TeamSpawn$", Value = ZONETYPE_TEAMSPAWN },
			{ Name = "$InstantDeath$", Value = ZONETYPE_INSTANTDEATH }
		],
		Set = "SetZoneType"
	};

	// Team for Spawn Areas. This team will be excluded from the area check.
	def.EditorProps.team =
	{
		Name = "$Team$",
		Type = "int",
		Set = "SetTeam"
	};
}


/* --- Setter functions for EditorProps --- */


func SetAreaRect(array new_area_rect)
{
	area = new_area_rect;
	return true;
}


func SetZoneType(int new_type)
{
	zonetype = new_type;

	// Update Graphics for editor view
	var gfxnames = ["", "TeamSpawn", "Death"];
	SetGraphics(gfxnames[new_type]);

	return true;
}


func SetTeam(int new_team)
{
	team = new_team;
	return true;
}

/* --- Scenario Saving --- */


func SaveScenarioObject(props)
{
	if (!inherited(props, ...)) return false;

	props->AddCall("Area", this, "SetAreaRect", area);
	props->AddCall("ZoneType", this, "SetZoneType", zonetype);
	props->AddCall("Team", this, "SetTeam", team);

	return true;
}