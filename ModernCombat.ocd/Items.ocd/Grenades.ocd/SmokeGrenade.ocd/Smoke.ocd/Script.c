
/* --- Properties --- */

local ContactCalls = 1;

local lifetime; // Live this long
local fadetime; // Start fading after this amount
local is_smoking;
local smoke_size;

static const CMC_SmokeGrenade_FadeTime = 175;		

public func IsSmoking()	{return is_smoking;}


/* --- Engine callbacks --- */

func Initialize()
{
	is_smoking = true;
	lifetime = 35 * RandomX(20, 25);
	fadetime = Max(0, lifetime - CMC_SmokeGrenade_FadeTime);

	CreateEffect(SmokeGrenade, 25, 2);
}

/* --- Contact calls --- */

func ContactTop()
{
	SetYDir(GetYDir() + 3);
}


func ContactBottom()
{
	SetYDir(GetYDir() - 3);
}


func ContactLeft()
{
	SetXDir(GetXDir() + 3);
}


func ContactRight()
{
	SetXDir(GetXDir() - 3);
}


/* --- Smoke effect --- */

local SmokeGrenade = new Effect
{
	Timer = func (int time)
	{
		if (time % 4 == 0)
		{
			this.Target->DoSmoke(time);
		}
		if (time % 6 == 0)
		{
			this.Target->DoStuff(time);
		}
	},
};


func DoSmoke(int time)
{
	var some_value = (time - fadetime) * 255 / CMC_SmokeGrenade_FadeTime;
	var alpha = 255 - BoundBy(some_value, 0, 255);
	CreateParticle("Smoke", 0, -smoke_size / 5, PV_Random(-20, 20), PV_Random(-10, +10), 85, {Prototype = CMC_Grenade_Smoke->Particles_GrenadeSmoke(5, smoke_size * 2), Alpha = PV_Linear(alpha, 0), });
}


func DoStuff(int time)
{
	if (time > lifetime || InLiquid())
	{
		return RemoveObject();
	}

	if (Inside(time, 35, 70))
	{
		DestroyTracers();
	}

	is_smoking = (time < fadetime / 3);

	if (time <= fadetime)
	{
		// Increase in size
		smoke_size = BoundBy(smoke_size + 5, 5, 50);

		// Slow down
		Damp(smoke_size);
		SetYDir(GetYDir(100) - GetGravity() / 2, 100);

		// Blind livings
		var targets = FindObjects(Find_Distance(smoke_size / 2), Find_NoContainer(), Find_OCF(OCF_Alive | OCF_CrewMember));
		for (var target in targets)
		{
			if (!GetEffect("BlindedBySmoke", target))
			{
				target->CreateEffect(BlindedBySmoke, 1, 1, GetController());
			}
		}
	}
}

/* --- Destroys tracer darts --- */

func DestroyTracers()
{
	for (var target in FindObjects(Find_Distance(40),
					Find_Category(C4D_Living | C4D_Structure | C4D_Vehicle),
					Find_Allied(GetOwner())))
		if (GetEffect("TracerDart", target))
			RemoveEffect("TracerDart", target);
}


/* --- Blind crew members --- */

local BlindedBySmoke = new Effect 
{
	Start = func (int temp, int by_player)
	{
		if (!temp)
		{
			//this.by_player = by_player;
			//EffectVar(0, pTarget, iEffectNumber) = ScreenRGB(pTarget, RGBa(150, 150, 150, 254), 0, 0, false, SR4K_LayerSmoke);
		}
	},

	Timer = func (int time)
	{
	/*
		//Sichteffekt ermitteln
		var rgb = EffectVar(0, pTarget, iEffectNumber);
		if(!rgb) return 0;

		// Objekt noch im Rauch?
		var blinded = false;
		if (!Contained() || Contained()->GetID() == FKDT)
		{
			for(var smoke in FindObjects(pTarget->Find_AtPoint(), Find_ID(SM4K), Find_Func("IsSmoking")))
			{
				if (GetCon(smoke)/2 > Distance(GetX(smoke),GetY(smoke),GetX(pTarget),GetY(pTarget)))
				{
					blinded = true;
					break;
				}
			}
		}

		//Bildschirmeffekt verdunkeln oder aufl�sen
		if (blinded)
			rgb->DoAlpha(+10, 0, 254);
		else
		{
			rgb->DoAlpha(-10, 0, 254);
			if (rgb->GetAlpha() >= 254)
				return -1;
		}
	*/
	},

	Stop = func ()
	{
		// TODO: Remove screen fade effect
	},
};


/* --- Internals --- */

func Damp(int strength)
{
	var precision = 1000;
	var damped = precision - strength;
	SetXDir(damped * GetXDir(precision) / precision, precision);
	SetXDir(damped * GetYDir(precision) / precision, precision);
}

