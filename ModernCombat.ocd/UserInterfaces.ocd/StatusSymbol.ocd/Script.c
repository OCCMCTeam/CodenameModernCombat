/**
	Status Symbol
	
	Shows a certain state of an object.
	
	@author Zapper, Maikel (orignal code), Marky (made it more modular)
*/

/* --- Properties --- */

local symbol_current; // Name of current symbol
local symbols;        // All symbols
local offset_y = 7;   // Offset from the target to the bottom border of the symbol
local offset_top;     // Offset from the target object to its top border

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


global func ShowStatusSymbol(id symbol_id, int priority)
{
	if (this) 
	{
		var helper = CMC_StatusSymbol->GetStatusSymbolHelper(this);
		if (helper)
		{
			helper->AddSymbol(symbol_id, priority);
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
	offset_top = to->GetTop();
	var x = to->GetVertex(0, VTX_X);
	SetVertex(0, VTX_X, x, 1);

	// Set plane to be higher than the object attached to.
	this.Plane = Max(to.Plane + 1, 1000);	
	return;
}

func AddSymbol(id symbol_id, int priority)
{
	var symbol_name = Format("%v", symbol_id);
	var symbol = symbols[symbol_name];
	
	if (symbol)
	{
		if (symbol.Priority != priority)
		{
			symbol.Priority = priority;
			UpdateSymbol();
		}
	}
	else
	{
		symbols[symbol_name] = 
		{
			Symbol = symbol_id,
			GraphicsName = nil,
			Priority = priority,
			Added = FrameCounter(),
			Blink = false,
			Visibility = GetActionTarget().Visibility,
			R = nil,
			G = nil,
			B = nil,
			Alpha = nil,
		};
		UpdateSymbol();
	}
}

func RemoveSymbol(id symbol_id, int priority)
{
	symbols[Format("%v", symbol_id)] = nil;
	UpdateSymbol();
}

func GetSymbol(id symbol_id)
{
	return symbols[Format("%v", symbol_id)];
}

func UpdateSymbol()
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
			symbol_current = current;
		}
	}

	if (!symbol)
	{
		SetGraphics(nil, nil, 1);
		this.Visibility = VIS_None;
		Blink(false);
		SetClrModulation();
	}
	else
	{
		var sym_x = symbol.Symbol->GetDefOffset(0);
		var sym_y = symbol.Symbol->GetDefOffset(1);
		var sym_width = symbol.Symbol->GetDefWidth();
		var sym_height = symbol.Symbol->GetDefHeight();
		var above = sym_y + sym_height + offset_y - offset_top;

		SetVertex(0, VTX_Y, above, 2);
		SetShape(sym_x, sym_y, sym_width, sym_height);
		this.Visibility = symbol.Visibility;
		Blink(symbol.Blink);
		UpdateData(symbol);
	}
}


func SetSymbolGraphics(id symbol_id, string graphics_name)
{
	var symbol = GetSymbol(symbol_id);
	if (symbol)
	{
		symbol.GraphicsName = graphics_name;
		UpdateData(symbol);
	}
	return this;
}


func SetSymbolColor(id symbol_id, int r, int g, int b, int alpha)
{
	var symbol = GetSymbol(symbol_id);
	if (symbol)
	{
		if (r) symbol.R = r;
		if (g) symbol.G = g;
		if (b) symbol.B = b;
		if (alpha) symbol.Alpha = alpha;
		
		UpdateData(symbol);
	}
	return this;
}


func UpdateData(proplist symbol)
{
	if (Format("%v", symbol.Symbol) == symbol_current)
	{
		SetGraphics(symbol.GraphicsName, symbol.Symbol, 1, GFXOV_MODE_Base);
		SetClrModulation(RGBa(symbol.R ?? 255, symbol.G ?? 255, symbol.B ?? 255, symbol.Alpha ?? 255), 1);
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
	return this;
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
