
/* --- Properties --- */

local Visibility = VIS_Allies | VIS_Owner | VIS_Editor;
local Plane = 1000;

local EnergyBar;
local RemoveTime = 26; // Was the same value in all calls in the old implementation, makes sense to have it as a property immediately.
local Type = nil;

local lifetime;


local ActMap =
{
	Be = 
	{
		Prototype = Action,
		Name = "Be",
		Procedure = DFA_ATTACH,
		NextAction = "Hold",
		Length = 4,
		Delay = 2,
		X = 0, Y = 0, Wdt = 25, Hgt = 25,
		AbortCall = "AttachTargetLost",
	},
	
	Fade = 
	{
		Prototype = Action,
		Name = "Fade",
		Procedure = DFA_ATTACH,
		NextAction = "Hold",
		Length = 4,
		Delay = 2,
		X = 0, Y = 0, Wdt = 25, Hgt = 25,
		AbortCall = "AttachTargetLost",
		EndCall = "AttachTargetLost",
		Reverse = 1,
	},
};


/* --- Callbacks --- */


// Callback from the engine: this symbol has lost its parent.
func AttachTargetLost()
{
	if (GetAction() == "Fade" && GetActTime() == 0)
	{
		return; // This simply happens when changing the action
	}
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

public func AddTo(object target, int host_player, id type, string graphics_name)
{
	AssertDefinitionContext();
	var tag = CreateObject(this, 0, 0, target->GetOwner());
	tag->Init(target, host_player, type, graphics_name);
	return tag;
}

public func Get(object to, int for_player, id type)
{
	AssertDefinitionContext();	
	
	var allied = nil;
	if (for_player != nil)
	{
		allied = Find_Allied(for_player);
	}
	
	return FindObject(Find_ID(this), Find_ActionTarget(to), allied, Find_Func("IsTagType", type));
}


/* --- Internals --- */

func Init(object to, int host_player, id type, string graphics_name)
{
	// Initialize
	Type = type;
	SetAction("Be", to);
	SetOwner(host_player);
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
	if (alive && graphics_name != "Target")
	{
		EnergyBar = to->AddEnergyBar();
		
		if (EnergyBar)
		{
			EnergyBar->SetOwner(GetOwner());
			EnergyBar.Visibility = this.Visibility;
		}
	}
	
	// Reveal FoW
	SetLightColor(RGB(128, 128, 128));
	SetLightRange(10, 40);
}


func UpdateOwnerColor()
{
	if (GetActionTarget())
	{
		SetColor(GetActionTarget()->GetColor());
// TODO		SetOwner(GetActionTarget()->GetController());
	}
}

func IsTagType(id type)
{
	if (type)
	{
		return type == Type;
	}
	else // Generic search
	{
		return true;
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
		Remove();
	}
	else
	{
		++lifetime;
	}
}

func Remove()
{
	if (GetAction() != "Fade")
	{
		if (EnergyBar)
		{
			EnergyBar->RemoveObject();
		}
		SetAction("Fade", GetActionTarget());
	}
}
