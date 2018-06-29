
/* --- Properties --- */

local Visibility = VIS_Allies | VIS_Owner | VIS_Editor;
local Plane = 1000;

local Host;
local EnergyBar;
local RemoveTime = 26; // Was the same value in all calls in the old implementation, makes sense to have it as a property immediately.

local lifetime;


local ActMap =
{
	Be = 
	{
		Prototype = Action,
		Name = "Be",
		Procedure = DFA_ATTACH,
		NextAction = "Hold",
		Length = 5,
		Delay = 2,
		//FacetBase = 1,
		X = 0, Y = 0, Wdt = 25, Hgt = 25,
		AbortCall = "AttachTargetLost",
	}
};


/* --- Callbacks --- */


// Callback from the engine: this symbol has lost its parent.
func AttachTargetLost()
{
	return RemoveObject();
}
	
func SaveScenarioObject() { return false; }

func Destruction()
{
	if (EnergyBar)
	{
		EnergyBar->RemoveObject();
	}
}


/* --- Interface --- */

public func AddTo(object target, object host, string graphics_name)
{
	AssertDefinitionContext();
	var tag = CreateObject(this, 0, 0, target->GetOwner());
	tag->Init(target, host, graphics_name, );
	return tag;
}

public func Get(object to, int for_player)
{
	AssertDefinitionContext();	
	
	var allied = nil;
	if (for_player != nil)
	{
		allied = Find_Allied(for_player);
	}
	
	return FindObject(Find_ID(this), Find_ActionTarget(to), allied);
}


/* --- Internals --- */

func Init(object to, object host, string graphics_name)
{
	// Initialize
	SetAction("Be", to);
	Host = host;
	SetOwner(host->GetOwner());
	var alive = to->GetOCF() & OCF_Alive;
	
	// Graphics, if supplied
	if (graphics_name)
	{
		SetGraphics(graphics_name);
	}
	else if (!alive)
	{
		SetGraphics("Object");
	}
	
	// Position at center of the target
	SetVertex(0, VTX_X, to->GetVertex(0, VTX_X));
	SetVertex(0, VTX_Y, to->GetVertex(0, VTX_Y));
	
	// Set plane to be higher than the object attached to.
	this.Plane = Max(to.Plane + 1, 1000);

	// Update colors and ownership continuously
	UpdateOwnerColor();
	AddTimer(this.UpdateOwnerColor, 1);
	
	// Remove eventually
	AddTimer(this.RemoveTimer, 1);
	
	// Energy bar for livings
	if (alive)
	{
		EnergyBar = to->AddEnergyBar();
		
		if (EnergyBar)
		{
			EnergyBar->SetOwner(GetOwner());
			EnergyBar.Visibility = this.Visibility;
		}
	}
}


func UpdateOwnerColor()
{
	if (GetActionTarget())
	{
		SetColor(GetActionTarget()->GetColor());
// TODO		SetOwner(GetActionTarget()->GetController());
	}
}

func RefreshRemoveTimer()
{
	lifetime = 0;
}

func RemoveTimer()
{
	if (lifetime >= RemoveTime)
	{
		RemoveObject();
	}
	else
	{
		++lifetime;
	}
}
