
/* --- Properties --- */

local Visibility = VIS_Allies | VIS_Owner | VIS_Editor;
local Plane = 1000;

local Host;
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
		// TODO: Energy bar effect
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

//local pTarget, pHost, iRemoveTime, iEffectRemoveTime;


/* Einstellung */
/*
public func Set(object target, object host, bool fClonk, int remove_time, string szGraphics, int color)
{

  //Effekt zur Lebensanzeige
  if(GetOCF(pTarget) & OCF_Alive)
    AddEffect("ShowEnergyBar", this, 1, 1, this, 0, GetOwner(host), pTarget);


public func RefreshRemoveTimer(object pByHost)
{
  if(pHost && pHost != pByHost)
    return;

  iEffectRemoveTime = iRemoveTime;
  return true;
}
*/
/* Statusbalkeneffekt f�r feindliche Energie */
/*
public func FxShowEnergyBarStart(object target, int nr, int temp, int owner, object actTarget)
{
  if(temp) return;
  var bar = CreateObject(SBAR, 0, 0, owner);
  bar->Set(GetActionTarget(0,target), RGB(255,255,255), BAR_Energybar);
  SetVisibility(VIS_Owner|VIS_Allies, bar);
  EffectVar(0, target, nr) = bar;
  EffectVar(1, target, nr) = owner;
  EffectVar(2, target, nr) = actTarget;
  return true;
}

public func FxShowEnergyBarTimer(object target, int nr)
{
  var bar = EffectVar(0, target, nr);
  var actTarget = EffectVar(2, target, nr);
  var percent = BoundBy(100000 * GetEnergy(actTarget) / GetPhysical("Energy", PHYS_Current, actTarget), 0, 100);
  if(bar) bar->Update(percent, 0);
  SetVisibility(VIS_Owner|VIS_Allies, bar);
  return true;
}

public func FxShowEnergyBarStop(object target, int nr, int reason, bool temp)
{
  if(EffectVar(0, target, nr))
    RemoveObject(EffectVar(0, target, nr));
}
