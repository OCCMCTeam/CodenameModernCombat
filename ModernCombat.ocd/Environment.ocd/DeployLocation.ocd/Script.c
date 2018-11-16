/**
	Deploy location
	
	Marks a point where the player can enter the battle field.
 */

/* --- Constants --- */

static const CMC_SPAWNSYS_Rating_Allies  = +50;
static const CMC_SPAWNSYS_Rating_Enemies = -50;
static const CMC_SPAWNSYS_Rating_Traps   = -30;

/* --- Properties --- */

local Name = "$Name$";

local Visibility = VIS_Editor;

public func IsDeployLocation() { return true; }

local conditions;
local locations;
local team = NO_OWNER;
local menu_list;
local menu_icon;


local ActMap =
{
	Symbol = 
	{
		Prototype = Action,
		Name = "Symbol",
		Procedure = DFA_NONE,
		NextAction = "Hold",
		Length = 1,
		Delay = 0,
		X = 0, Y = 0, Wdt = 100, Hgt = 100,
	}
};


/* --- Engine callbacks --- */

public func Initialize()
{
	// Editor properties
	this.EditorProps = this.EditorProps ?? {};
	this.EditorActions = this.EditorActions ?? {};
	
	this.EditorActions.AddLocation =
	{
		Name = "$EditorAddLocationName$", 
		EditorHelp = "$EditorAddLocationHelp$", 
		Command="AddEditorPropsLocation(GetX() + 10, GetY())",
	};
	this.EditorActions.ApplyLocation =
	{
		Name = "$EditorApplyLocationName$", 
		EditorHelp = "$EditorApplyLocationHelp$", 
		Command="ApplyEditorPropsLocation()",
	};
	this.EditorActions.CancelLocation =
	{
		Name = "$EditorResetLocationName$", 
		EditorHelp = "$EditorResetLocationHelp$", 
		Command="ResetEditorPropsLocation()",
	};

	// Make clickable - menus are, to my knowledge, not positioned above objects
	menu_list = [];
	menu_icon = CreateObject(Dummy);
	menu_icon.Visibility = [VIS_Select];
	menu_icon->SetCategory(C4D_StaticBack | C4D_MouseSelect | C4D_IgnoreFoW);
	menu_icon->SetShape(-50, -50, 100, 100);
	menu_icon.Plane = 10000;
	menu_icon.location = this;
	menu_icon.MouseSelection = this.MouseSelectionCallback; // Add a click callback
	
	SetMenuIcon();
}

/* --- Settings --- */

public func AddRelaunchLocation(int x, int y, bool skip_update)
{
	locations = locations ?? [];
	PushBack(locations, RelaunchLocation(x, y));
	
	var length = GetLength(locations);
	var average_x = 0;
	var average_y = 0;
	for (var i = 0; i < length; ++i)
	{
		average_x += locations[i]->GetX();
		average_y += locations[i]->GetY();
	}
	if (length >= 1)
	{
		average_x /= length;
		average_y /= length;
		SetPosition(average_x, average_y);
	}
	
	if (!skip_update)
	{
		UpdateEditorPropsLocation();
	}

	return this;
}

public func AddCondition(proplist condition)
{
	conditions = conditions ?? [];
	PushBack(conditions, condition);
	return this;
}

public func SetTeam(int team_nr)
{
	team = team_nr ?? NO_OWNER;
	UpdateMenuIcon();
	return this;
}

/* --- Getters --- */

public func GetTeam()
{
	return team;
}

public func GetPriority()
{
	// Priority, for sorting
	var prio = 0;
	if (GetTeam() < 0) prio += 1;
	if (this->~IsTemporary()) prio += 1;
	if (conditions) prio += GetLength(conditions);
	return prio;
}

public func IsDisplayed(int player)
{
	if (this->~IsTemporary())
	{
		return team == NO_OWNER || team == GetPlayerTeam(player);
	}
	else
	{
		return true;
	}
}

public func IsAvailable(int player)
{
	return IsDisplayed(player) && IsFulfilled(conditions);
}

public func IsFulfilled(array conditions)
{
	for (var condition in conditions ?? [])
	{
		var fulfilled = DoCallback(condition);
		if (!fulfilled)
			return false;
	}
	return true;
}

public func RecommendRelaunchLocation(int player)
{
	var best_locations = [RelaunchLocation(GetX(), GetY())];

	var best_value = -1000;
	if (GetLength(locations) > 0)
	{
		best_locations = [locations[0]];
		best_value = GetRelaunchLocationRating(player, locations[0]);
		for (var index = 1; index < GetLength(locations); ++index)
		{
			var value = GetRelaunchLocationRating(player, locations[index]);
			if (value > best_value)
			{
				best_value = value;
				best_locations = [locations[index]];
			}
			else if (value == best_value)
			{
				PushBack(best_locations, locations[index]);
			}
		}
	}

	var location = best_locations[Random(GetLength(best_locations))];
	location->SetTeam(GetTeam());
	return location;
}


public func GetRelaunchLocationRating(int player, proplist relaunch_location)
{
	var x = relaunch_location->GetX() - GetX();
	var y = relaunch_location->GetY() - GetY();
	var rating = 0;
	
	for(var thing in FindObjects(Find_Distance(100, x, y), Find_Or(Find_OCF(OCF_CrewMember), Find_And(Find_Hostile(player), Find_Func("IsSpawnTrap")))))
    {
		if (thing->GetOCF() & OCF_CrewMember)
		{
        	if (Hostile(player, thing->GetOwner()))
        	{
        		rating += CMC_SPAWNSYS_Rating_Enemies;
        	}
        	else
        	{
        		rating += CMC_SPAWNSYS_Rating_Allies;
        	}
		}
		else
		{
			rating += CMC_SPAWNSYS_Rating_Traps;
		}
    }
}

/* --- Clickable symbol --- */

public func CreateMenuFor(int player, proplist callback_menu)
{
	menu_list[player] = callback_menu;
	menu_icon.Visibility[player + 1] = 1;
	menu_icon->SetPosition(GetX(), GetY());
	
	UpdateMenuIcon();
}

public func CloseMenuFor(int player)
{
	menu_list[player] = nil;
	menu_icon.Visibility[player + 1] = 0;
}

// Function for the menu_icon object
public func MouseSelectionCallback(int player)
{
	this.location->OnClick(player);
}

// Actual call when clicked with mouse
public func OnClick(int player)
{
	if (menu_list[player])
	{
		menu_list[player]->GetDeployLocations()->SelectTab(ObjectNumber());
	}
}

func UpdateMenuIcon()
{
	var color;
	if (GetTeam() && GetTeam() != NO_OWNER)
	{
		color = SetRGBaValue(GetTeamColor(GetTeam()), 255, RGBA_ALPHA);
	}
	else
	{
		color = RGBa(255, 255, 255, 255);
	}
	menu_icon->SetClrModulation(color, 2);
	for (var menu in menu_list)
	{
		if (menu)
		{
			menu->GetDeployLocations()->GetTab(ObjectNumber())->UpdateLocationStatus(color);
		}
	}
}

func SetMenuIcon(id icon, string gfx_name)
{
	icon = icon ?? CMC_DeployLocation;
	gfx_name = gfx_name ?? "Neutral";

	menu_icon->SetGraphics(gfx_name, CMC_DeployLocation, 1, GFXOV_MODE_IngamePicture);
	menu_icon->SetGraphics(Format("%sOverlay", gfx_name), CMC_DeployLocation, 2, GFXOV_MODE_IngamePicture);
}

/* --- Editor properties --- */

func UpdateEditorPropsLocation()
{
	var color = 0xdf0000;
	
	this.editor_location_count = Max(this.editor_location_count, GetLength(locations));
	
	for (var i = 0; i < this.editor_location_count; ++i)
	{
		var name = GetEditorPropsLocationName(i);
		var property =
		{
			Name = Format("$EditorLocationName$", i),
			EditorHelp = "$EditorLocationHelp$",
			Type = "polyline", 
			StartFromObject = true, 
			DrawArrows = true, 
			Color = color, 
			Relative = false, 
			Set = "ApplyEditorPropsLocation", // FIXME: this does not seem to work
		};	
		this.EditorProps[name] = property;
		if (locations[i])
		{
			this[name] = [{X = locations[i].x, Y = locations[i].y}];
		}
	}
}

func AddEditorPropsLocation(int x, int y)
{
	this[GetEditorPropsLocationName(this.editor_location_count)] = [{X = x, Y = y}]; 
	this.editor_location_count += 1;
	UpdateEditorPropsLocation();
}

func ApplyEditorPropsLocation()
{
	for (var i = 0, name = GetEditorPropsLocationName(i); i < this.editor_location_count; name = GetEditorPropsLocationName(++i))
	{
		if (this[name])
		{
			if (i >= GetLength(locations))
			{
				AddRelaunchLocation(this[name][0].X, this[name][0].Y, true);
			}
			else
			{
				locations[i].x = this[name][0].X;
				locations[i].y = this[name][0].Y;
			}
		}
	}
	
	UpdateEditorPropsLocation();
}

func ResetEditorPropsLocation()
{
	for (var i = 0; i >= GetLength(locations); --i)
	{
		this[GetEditorPropsLocationName(i)] = nil;
	}
	UpdateEditorPropsLocation();
}

func GetEditorPropsLocationName(int index)
{
	return Format("editor_location%d", index);
}
