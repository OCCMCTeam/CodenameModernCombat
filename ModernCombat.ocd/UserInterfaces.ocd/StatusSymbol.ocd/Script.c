/**
	Status Symbol
	
	Shows a certain state of an object.
	
	@author Zapper, Maikel (orignal code), Marky (made it more modular)
*/

/* --- Properties --- */

local symbols;

local Name = "$Name$";
local Description = "$Description$";
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

public func GetStatusSymbolHelper(object to)
{
	AssertDefinitionContext();	
	var helper = FindObject(Find_ID(this), Find_ActionTarget(to));
	if (!helper)
	{
		helper = CreateObject(this, 0, 0, to->GetOwner());
		helper->Init(to);
	}
	return helper;
}


global func ShowStatusSymbol(id symbol_id, int priority, string graphics_name, int visibility, bool blink)
{
	if (this) 
	{
		var helper = CMC_StatusSymbol->GetStatusSymbolHelper(this);
		if (helper)
		{
			helper->AddSymbol(symbol_id, graphics_name, priority, visibility, blink);
			return helper;
		}
	}
	return helper;
}


global func RemoveStatusSymbol(id symbol)
{
	if (this)
	{
		var helper = CMC_StatusSymbol->GetStatusSymbolHelper(this);
		if (helper) 
		{
			helper->RemoveSymbol(symbol);
			return true;
		}
	}
	return false;
}

/* --- Internals --- */

func Init(object to)
{
	SetOwner(to->GetOwner());
	SetAction("Be", to);
	symbols = {};
	
	// Above the object.
	var height = to->GetDefCoreVal("Height", "DefCore") / 2;
	SetVertex(0, VTX_Y, height, 1);
	var x = to->GetVertex(0, VTX_X);
	SetVertex(0, VTX_X, x);

	// Set plane to be higher than the object attached to.
	this.Plane = Max(to.Plane + 1, 1000);	
	return;
}

func AddSymbol(id symbol_id, string graphics_name, int priority, int visibility, bool blink)
{
	symbols[Format("%v", symbol_id)] = 
	{
		Symbol = symbol_id,
		GraphicsName = graphics_name,
		Priority = priority,
		Added = FrameCounter(),
		Blink = blink,
		Visibility = visibility ?? GetActionTarget().Visibility,
	};
	Update();
}

func RemoveSymbol(id symbol_id, int priority)
{
	symbols[Format("%v", symbol_id)] = nil;
	Update();
}

func GetSymbol(id symbol_id)
{
	return symbols[Format("%v", symbol_id)];
}

func Update()
{
	var symbol;
	for (var current in GetProperties(symbols))
	{
		if (symbols[current] &&
		   (!symbol
		    || (symbols[current].Priority > symbol.Priority)
		    || (symbols[current].Priority == symbol.Priority && symbols[current].Added > symbol.Added)))
		{
			symbol = symbols[current];
			Log("New symbol is %v", symbol);
		}
	}

	if (!symbol)
	{
		SetGraphics(nil, nil, 1);
		this.Visibility = VIS_None;
		Blink(false);
	}
	else
	{
		SetShape(symbol.Symbol->GetDefOffset(0), symbol.Symbol->GetDefOffset(1), symbol.Symbol->GetDefWidth(), symbol.Symbol->GetDefHeight());
		SetGraphics(symbol.GraphicsName, symbol.Symbol, 1, GFXOV_MODE_Base);
		this.Visibility = symbol.Visibility;
		Blink(symbol.Blink);
	}
}

/* --- Optional: Blinking --- */

public func Blink(bool should_blink, int visibility)
{
	RemoveEffect("Blinking", this);
	if (should_blink)
	{
		CreateEffect(Blinking, 1, 16, visibility);
	}
}

local Blinking = new Effect 
{
	Start = func (int temp)
	{
		if (!temp)
		{
			this.Interval = 16;
			this.Visibility = this.Target.Visibility;
			this.Visible = true;
		}
		return FX_OK;
	},
	
	Timer = func ()
	{
		if (this.Visible)
		{
			this.Visible = false;
			this.Target.Visibility = VIS_None;
		}
		else
		{
			this.Visible = true;
			this.Target.Visibility = this.Visibility;
		}
		return FX_OK;
	}
};
