
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
		// The function name could be a string, but it seems that checking it this way is more safe for detecting incompatible changes
		// TODO: Concerning the radius, see below.
		var targets = FindObjects(Find_Distance(smoke_size / 2), Find_Func(GetFunctionName(CMC_Library_AffectedBySmokeGrenade.IsAffectedBySmokeGrenade))); 
		for (var target in targets)
		{
			target->BlindedBySmokeGrenade(this);
		}
	}
}

/* --- Internals --- */

func DestroyTracers()
{
	for (var target in FindObjects(Find_Distance(40),
					Find_Category(C4D_Living | C4D_Structure | C4D_Vehicle),
					Find_Allied(GetOwner())))
		if (GetEffect("TracerDart", target)) // TODO: There are no tracer darts yet.
			RemoveEffect("TracerDart", target);
}


func CanAffect(object target)
{
	return ObjectDistance(target) <= smoke_size / 2 // TODO: After testing I would actually increase this to full smoke_size
	    && target->~IsAffectedBySmokeGrenade(this);
}


func Damp(int strength)
{
	var precision = 1000;
	var damped = precision - strength;
	SetXDir(damped * GetXDir(precision) / precision, precision);
	SetXDir(damped * GetYDir(precision) / precision, precision);
}

