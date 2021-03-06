
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

local CursorObjects = [];
local AimOrigin;


/* --- Callbacks --- */


// Callback from the engine: this symbol has lost its parent.
func AttachTargetLost()
{
	return RemoveObject();
}

func SaveScenarioObject() { return false; }


/* --- Interface --- */

public func AddTo(object target, int player)
{
	AssertDefinitionContext();
	var cursor = CreateObject(this, 0, 0, player ?? target->GetOwner());
	cursor->Init(target);
	return cursor;
}

public func Get(object to)
{
	AssertDefinitionContext();

	// Works only the second time the player aims, which is really weird... 
	// the cursor object exists, and can be found via Find_ActionTarget()
	// but Find_ID ignores it
	//
	// return FindObject(Find_ID(this), Find_ActionTarget(to));
	for (var attached in FindObjects(Find_ActionTarget(to)))
	{
		if (attached->GetID() == this)
		{
			return attached;
		}
	}
	return nil;
}

public func SetCursorType(type, int index)
{
	if (GetType(type) == C4V_Def)
	{
		var current, spread;
		if (CursorObjects[index])
		{
			current = CursorObjects[index]->GetID();
			spread = CursorObjects[index].Cursor_Spread;

		}
		if (current != type)
		{
			if (CursorObjects[index])
			{
				CursorObjects[index]->RemoveObject();
			}
			CursorObjects[index] = CreateObject(type, 0, 0, GetOwner());
			CursorObjects[index]->Init(this);
			UpdateAimPosition(nil, GetVertex(0, VTX_X), GetVertex(0, VTX_Y));
			UpdateAimSpread([spread]);
		}
		return CursorObjects[index];
	}
	else if (GetType(type) == C4V_Array)
	{
		for (var i = 0; i < GetLength(type); ++i)
		{
			SetCursorType(type[i], i);
		}
	}
	else
	{
		FatalError("Pass array or id");
	}
}

/* --- Internals --- */

func Init(object target, string graphics_name)
{
	// Initialize
	SetAction("Be", target);

	// Attach vertex 1 of the virtual cursor to vertex 0 of the target.
	// This makes attachment of the actual cursor easy, because it will
	// simply attach to the virtual cursor vertex 0, see below.
	var attach_vertex = 1;
	var target_vertex = 0;

	// Position at center of the target
	SetVertex(attach_vertex, VTX_X, target->GetVertex(target_vertex, VTX_X));
	SetVertex(attach_vertex, VTX_Y, target->GetVertex(target_vertex, VTX_Y));
	AimOrigin = {X = 0, Y = 0};

	// Attach with the non-default vertex
	SetActionDataAttach(attach_vertex, target_vertex);

	// Set plane to be higher than the object attached to.
	this.Plane = Max(target.Plane + 1, 1000);
}


func UpdateAimPosition(object weapon, int x, int y)
{
	var min_range = 50;
	if (Distance(x, y) < min_range)
	{
		var angle = Angle(0, 0, x, y);
		x = +Sin(angle, min_range);
		y = -Cos(angle, min_range);
	}

	SetVertex(0, VTX_X, x);
	SetVertex(0, VTX_Y, y);
	
	var target = GetActionTarget();
	if (weapon)
	{
		AimOrigin = weapon->~GetWeaponPosition(target, WEAPON_POS_Muzzle, target->~GetAimPosition()) ?? AimOrigin;
	}

	for (var cursor in CursorObjects)
	{
		if (cursor) cursor->~UpdateAimPosition(x, y, AimOrigin.X, AimOrigin.Y);
	}
}


func UpdateAimSpread(array spread)
{
	for (var cursor in CursorObjects)
	{
		if (cursor) cursor->~UpdateAimSpread(spread);
	}
}


func Show()
{
	this.Visibility = VIS_Owner;
	for (var cursor in CursorObjects)
	{
		if (cursor)
		{
			cursor.Visibility = VIS_Owner;
			cursor->SetOwner(GetOwner());
		}
	}
}


func Hide()
{
	this.Visbility = VIS_None;
	for (var cursor in CursorObjects)
	{
		if (cursor) cursor.Visibility = VIS_None;
	}
}
