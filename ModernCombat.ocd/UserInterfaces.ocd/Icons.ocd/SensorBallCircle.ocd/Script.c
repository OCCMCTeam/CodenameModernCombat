
/* --- Properties --- */

local Visibility = VIS_Allies | VIS_Owner | VIS_Editor;
local Plane = 1000;


local ActMap =
{
	Be = 
	{
		Prototype = Action,
		Name = "Be",
		Procedure = DFA_ATTACH,
		NextAction = "Be",
		Length = 1,
		Delay = 0,
		FacetBase = 1,
		AbortCall = "AttachTargetLost"
	}
};


/* --- Callbacks --- */


// Callback from the engine: this symbol has lost its parent.
func AttachTargetLost()
{
	return RemoveObject();
}
	
func SaveScenarioObject() { return false; }


/* --- Interface --- */

public func AddTo(object target, bool ignore_fadeout)
{
	AssertDefinitionContext();
	var circle = CreateObject(this, 0, 0, target->GetOwner());
	
	circle->Init(target);
	
	if (!ignore_fadeout)
	{
		circle->FadeOut(80, true);
	}
	return circle;
}

public func Get(object to)
{
	AssertDefinitionContext();	
	return FindObject(Find_ID(this), Find_ActionTarget(to));
}


/* --- Internals --- */

func Init(object to)
{
	SetAction("Be", to);
	
	// Position at center of the target
	SetVertex(0, VTX_X, to->GetVertex(0, VTX_X));
	SetVertex(0, VTX_Y, to->GetVertex(0, VTX_Y));
	
	// Set plane to be higher than the object attached to.
	this.Plane = Max(to.Plane + 1, 1000);

	// Update colors and ownership continuously
	UpdateOwner();
	AddTimer(this.UpdateOwner, 1);	
}


func UpdateOwner()
{
	if (GetActionTarget())
	{
		SetOwner(GetActionTarget()->GetController());
	}
}
