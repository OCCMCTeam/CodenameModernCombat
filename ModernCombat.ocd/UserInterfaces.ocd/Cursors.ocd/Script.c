
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

local CursorObject = nil;


/* --- Callbacks --- */


// Callback from the engine: this symbol has lost its parent.
func AttachTargetLost()
{
	return RemoveObject();
}
	
func SaveScenarioObject() { return false; }


/* --- Interface --- */

public func AddTo(object target)
{
	AssertDefinitionContext();
	var cursor = CreateObject(this, 0, 0, target->GetOwner());
	cursor->Init(target);
	return cursor;
}

public func Get(object to)
{
	AssertDefinitionContext();

	return FindObject(Find_ID(this), Find_ActionTarget(to));
}

public func SetCursorType(id type)
{
	if (CursorObject)
	{
		CursorObject->Remove();
	}
	CursorObject = CreateObject(type, 0, 0, GetOwner());
	CursorObject->Init(this);
	return CursorObject;
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
	
	// Attach with the non-default vertex
	SetActionDataAttach(attach_vertex, target_vertex);

	// Set plane to be higher than the object attached to.
	this.Plane = Max(target.Plane + 1, 1000);
}


func UpdateAimPosition(int x, int y)
{
	SetVertex(0, VTX_X, x);
	SetVertex(0, VTX_Y, y);
	if (CursorObject)
	{
		CursorObject->~UpdateAimPosition(x, y);
	}
}


func UpdateAimSpread(array spread)
{
	if (CursorObject)
	{
		CursorObject->~UpdateAimSpread(spread);
	}
}


func Show()
{
	this.Visibility = VIS_Owner;
	if (CursorObject)
	{
		CursorObject.Visibility = VIS_Owner;
	}
}


func Hide()
{
	this.Visbility = VIS_None;
	if (CursorObject)
	{
		CursorObject.Visibility = VIS_None;
	}
}
