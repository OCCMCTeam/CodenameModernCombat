
/* --- Properties --- */

local lifetime;

local ActMap = 
{
	Attach = 
	{
		Prototype = Action,
		Name = "Attach",
		Procedure = DFA_ATTACH,
		Length = 1,
		Delay = 0,
		FacetBase = 1,
	},
	Stick = 
	{
		Prototype = Action,
		Name = "Stick",
		Procedure = DFA_FLOAT,
		Length = 1,
		Delay = 0,
		FacetBase = 1,
	},
};

/* --- Engine callbacks --- */

func Initialize()
{
	lifetime = RandomX(300, 400);

	SetLightRange(20, 40);
	// TODO: AddFireEffect(this, 0, RGB(0, 255, 255), 0, RandomX(-5, -20));

	AddTimer(this.Timer, 1);
	Timer();	
}

/* --- Activity --- */

func Timer()
{
	if (Contained())
	{
		Exit();
		FatalError("This should never happen, actually");
	}

	var speed = Distance(GetXDir(), GetYDir()) + Random(3);

	if (!(lifetime % 5))
	{
		var modulation = BoundBy(speed, 1, 40) * 255 / 40;
		var rgb = RGB(modulation, BoundBy(modulation, 140, 255), 255);
		SetLightColor(rgb);	
	}

	BurnObjects();

	if (!Random(20))
	{
		// TODO: Sparks - but I am sure this could be handled via the usual fire effect
	}

	if (GBackLiquid())
	{
		// TODO: Bubbles
		lifetime -= 10;
	}
	else
	{
		lifetime -= 1;
	}

	if (lifetime <= 0)
	{
		RemoveObject();
	}
}

/* --- Functionality --- */

func BurnObjects()
{
	var targets = FindObjects(Find_Distance(10),
	                          Find_Not(Find_ID(GetID())), 
	                          Find_NoContainer(), 
	                          Find_OCF(OCF_Inflammable), 
	                          Find_Or(Find_Func("IsBulletTarget", GetID(), this), Find_OCF(OCF_Alive)));
	for (var target in targets)
	{
		BurnObject(target);
	}
}

func BurnObject(object target)
{
	if (!target) return;

	if (target->GetAlive())
	{
		target->CreateEffect(PhosphorBurn, 50, 20, this);
	}
	
	target->DoEnergy(-3, false, FX_Call_DmgFire, GetController());
	
	//DoDmg(3, DMG_Fire, pObj, 1);
	//AddFireEffect(pObj,50,FIRE_Red,1);

	if (target) // May be removed after taking damage
	{
		// Incinerate non-living targets	
		if (target->GetOCF() & OCF_Inflammable && !(target->GetOCF() & OCF_Alive))
		{
			target->Incinerate(nil, GetController());
		}
	}
}



func Hit()
{
	// This code makes no sense:
	//var contact = GetContact(-1, 0);
	//if (contact & CNAT_Left || contact & CNAT_Right || contact & CNAT_Top || lifetime < 300)
	//{
		SetAction("Stick");
	//}
}


/* --- Sticking effect --- */

local PhosphorBurn = new Effect
{
	Start = func (int temp, object glob)
	{
		if (temp || ObjectCount(Find_ID(CMC_Grenade_PhosphorHelper), Find_ActionTarget(this.Target)) > 3)
		{
			return -1;
		}
		this.Glob = glob;
		this.Glob->SetAction("Attach", this.Target);
	},
	
	Timer = func (int time)
	{
		if (!this.Target->GetAlive() || this.Target->Contained() || time > 40)
		{
			this.Glob->AttachTargetLost();
		}
		
		if (time > 60)
		{
			return FX_Execute_Kill;
		}
		return FX_OK;
	},
};


func AttachTargetLost()
{
	SetAction("Idle");
	SetXDir(RandomX(-20, 20));
	SetYDir(RandomX(+10,-30));
}
