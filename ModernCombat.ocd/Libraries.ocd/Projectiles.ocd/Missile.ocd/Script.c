#include CMC_Library_GrenadeProjectile

/* --- Properties --- */

local Grenade_MaxDamage = 5;
local Grenade_SecureDelay = 25;
local Grenade_SecureDistance = 0;
local Grenade_FuseTime = 200;

local Missile_Guided = false;
local Missile_Speed = 5;
local Missile_RDir = 3;

local Missile_HasFuel = true;
local Missile_IsDamaged = false;

/* --- Callbacks from projectile --- */

public func Launch(int angle, deviation)
{
	SetR(angle);
	inherited(angle, deviation, ...);
}

func OnLaunch()
{
	inherited(...);
	SetAction("Travel");
	SetLightColor(RGB(255, 200, 200));
	SetLightRange(70);
	SetColor(GetPlayerColor(GetController()));
}


/* --- Sounds --- */

func PlaySoundDetonation()
{
	Sound("Items::Grenades::Shared::Explosion?");
}

func PlaySoundMalfunction()
{
	// TODO
}

/* --- Events --- */


// What happens when it explodes
public func OnDetonation()
{
	if (this.Missile_IsDamaged)
	{
		Explode(15, true);
	}
	else
	{
		Explosion([25, 40], [50, 40], true);
	}
}

// If max damage is acquired
public func OnMaxDamage(int change, int cause, int cause_player)
{
	if (this.Missile_IsDamaged)
	{
		Detonate();
	}
	else
	{
		Malfunction();
	}
}


public func DoDamageObject(object target)
{
	if (IsSecure() || this.Missile_IsDamaged)
	{
		target->DoEnergy(this.Missile_Speed / 5);
	}
	else
	{
		Detonate();
	}
}

func OnHitLandscape()
{
	if (IsSecure())
	{
		CreateImpactEffect(3);
	}
	else
	{
		Detonate();
	}
}


func Fall()
{
	SetAction("TravelBallistic");
	this.Missile_HasFuel = false;
}


func Malfunction()
{
	if (!this.Missile_IsDamaged)
	{
		this.Missile_IsDamaged = true;
		
		//if(!IsSecure() && Hostile(iLastAttacker, GetController()))
		//{
		//	//Punkte bei Belohnungssystem (Projektil abgefangen)
		//	DoPlayerPoints(BonusPoints("Protection"), RWDS_TeamPoints, iLastAttacker, GetCursor(iLastAttacker), IC16);
		//}
		PlaySoundMalfunction();
		SetAction("TravelBallistic");
	}
}


/* --- Internals --- */

public func Acceleration()
{
	if (this.Missile_Guided)
	{
		return 3;
	}
	else
	{
		return 5;
	}
}

public func MaxSpeed()
{
	if (this.Missile_Guided)
	{
		return 100;
	}
	else
	{
		return 150;
	}
}

func ControlSpeed()
{
	if (GetAction() == "Travel")
	{
		if (this.Missile_Speed < this->MaxSpeed())
		{
			this.Missile_Speed = BoundBy(this.Missile_Speed + this->Acceleration(), 0, this->MaxSpeed());
		}
	
		velocity_x = +Sin(GetR(), this.Missile_Speed);
		velocity_y = -Cos(GetR(), this.Missile_Speed);

		SetXDir(velocity_x);
		SetYDir(velocity_y);
	}
	
	var target_r = Angle(0, 0, GetXDir(), GetYDir());
	var diff_r = BoundBy(target_r - GetR(), -this.Missile_RDir, +this.Missile_RDir);
	SetR(GetR() + diff_r);
}


func OnTravelling()
{
	if (GBackLiquid())
	{
		Fall();
	}
	
	if (GetY() < -500)
	{
		Detonate();
	}
	
	HandleSmokeTrail();
}


func OnFuseTimeout()
{
	Fall();
}


func HandleSmokeTrail()
{
	var distance = Distance(0, 0, GetXDir(), GetYDir());
	var max_x = +Sin(GetR(), distance / 10);
	var max_y = -Cos(GetR(), distance / 10);
	var off_x = -max_x;
	var off_y = -max_y;
	var particle_distance = 25;

	for(var i = 0; i < distance; i += particle_distance)
	{
		var x = -max_x * i / distance;
		var y = -max_y * i / distance;

		var rand = RandomX(-30, 30);
		var xdir = +Sin(GetR() + rand, 20);
		var ydir = -Cos(GetR() + rand, 20);
		
		var size_thrust = RandomX(8, 10);
		var size_smoke = RandomX(10, 12);
		
		if (this.Missile_IsDamaged)
		{
			off_x *= -1;
			off_y *= -1;
			size_thrust /= 2;
		}

		// Damaged missile or missile with fule creates fire
		if (this.Missile_IsDamaged || this.Missile_HasFuel)
		{
			CreateParticle("Thrust", x + off_x, y + off_y, GetXDir()/2, GetYDir()/2, PV_Random(5, 10),
			{
				Prototype = Particles_ThrustColored(255, 200, 200),
				Size = size_thrust,
			});
		}
		
		// Missile with fuel creates smoke
		if (this.Missile_HasFuel)
		{
			CreateParticle("Smoke2", 2 * (x + off_x), 2 * (y + off_y) + 1, xdir, ydir, PV_Random(30, 40),
			{
				Prototype = Particles_ThrustColored(220, 200, 180),
				Size = size_smoke,
			});
		}
	}
}
