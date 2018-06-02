
/* --- Properties --- */

local Visibility = VIS_Owner;
local Plane = 1000;

local ActMap =
{
	Be = 
	{
		Prototype = Action,
		Name = "Be",
		Procedure = DFA_ATTACH,
		NextAction = "Hold",
		Length = 1,
		Delay = 0,
		FacetBase = 1,
		AbortCall = "AttachTargetLost",
	}
};

local Cursor_Distance = 0; // Distance from the center
local Cursor_Spread = 0;   // Opening angle of half the cone, in millidegrees

/* --- Callbacks --- */


// Callback from the engine: this symbol has lost its parent.
func AttachTargetLost()
{
	return RemoveObject();
}
	
func SaveScenarioObject() { return false; }


/* --- Internals --- */

func Init(object target)
{
	SetAction("Be", target);
	InitGraphics();
	this.Plane = target.Plane;
}

func InitGraphics()
{
	SetGraphics("Left", GetID(), CNAT_Left, GFXOV_MODE_ExtraGraphics);
	SetGraphics("Top", GetID(), CNAT_Top, GFXOV_MODE_ExtraGraphics);
	SetGraphics("Bottom", GetID(), CNAT_Bottom, GFXOV_MODE_ExtraGraphics);
	SetGraphics("Right", GetID(), CNAT_Right, GFXOV_MODE_ExtraGraphics);
}

/* --- Callbacks --- */

func UpdateAimPosition(int x, int y)
{
	Cursor_Distance = Distance(x, y);
	ScheduleUpdate();
}

func UpdateAimSpread(array spread)
{
	Cursor_Spread = 0;
	for (var value in spread)
	{
		Cursor_Spread += value;
	}
	ScheduleUpdate();
}

func ScheduleUpdate()
{
	if (!GetEffect("ScheduledUpdate", this))
	{
		CreateEffect(ScheduledUpdate, 1, 1);
	}
}

local ScheduledUpdate = new Effect
{
	Timer = func ()
	{
		this.Target->DoUpdate();
	}
};

func DoUpdate()
{
	var precision = 1000;
	var radius = Sin(Cursor_Spread, Cursor_Distance * precision, precision);
	UpdateGraphics(radius);
}

func UpdateGraphics(int distance)
{
	var full_size = 1000;
	SetObjDrawTransform(full_size, nil, -distance, nil, full_size, 0, CNAT_Left);
	SetObjDrawTransform(full_size, nil, +distance, nil, full_size, 0, CNAT_Right);
	SetObjDrawTransform(full_size, nil,         0, nil, full_size, -distance, CNAT_Top);
	SetObjDrawTransform(full_size, nil,         0, nil, full_size, +distance, CNAT_Bottom);
}
