#include Library_Projectile

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local Tracer_StartX = 0;
local Tracer_StartY = 0;
local Tracer_Color = 0;

/* --- Launching --- */


func OnLaunch()
{
	SetAction("Travel");
	// FIXME: No idea if we actually want them to glow
	SetLightColor(GetPlayerColor(GetOwner()));
	SetLightRange(50, 30);
	
	Tracer_StartX = GetX();
	Tracer_StartY = GetY();
	
	var color = SplitRGBaValue(GetPlayerColor(GetController()));
	var max = Max([color.R, color.G, color.B]);
	Tracer_Color = RGB(color.R * 255 / max, color.G * 255 / max, color.B * 255 / max); // Max value
}


func OnTravelling()
{
	CreateFlare();

	if (InLiquid() || GetY() < 0)
	{
		Remove();
	}
}

func CreateFlare()
{
	var max = velocity / 10;
	var prec = 1000;
	
	var angle = Angle(0, 0, GetXDir(), GetYDir(), prec);

	var x = -Sin(angle, max, prec);
	var y = +Cos(angle, max, prec);
	var particles = {
		Prototype = Particles_Colored(Particles_Glimmer(), Tracer_Color),
		Size = PV_Linear(2, 1), 
		Stretch = nil, 
		ForceY = nil, 
		DampingX = nil, 
		DampingY = nil,
	};

	for (var dist = 0; dist < max; ++dist)
	{
		var x_off = x + Sin(angle, dist, prec); 
		var y_off = y - Cos(angle, dist, prec);
		var amount = dist / 3;
		CreateParticle("MagicFire", PV_Random(x_off - 1, x_off + 1), PV_Random(y_off - 1, y_off + 1), 0, 0,
		                        5, particles, amount);
	}
}

/* --- Effects --- */

/**
 Callback if the projectile hits another object.
 @par target This is the object that was hit.
 @par hitcheck_effect // FIXME: is currently not passed to this callback
 */
public func OnHitObject(object target, proplist hitcheck_effect)
{
	if (target)
	{
		if (target->~BlockTracer())
		{
			return Remove();
		}
	
	
		if ((Hostile(target->GetOwner(), GetController()) || target->~AttractTracer(this))
	    && !target->InLiquid()
	    && !target->~IgnoreTracer()
	    && !GetEffect("TracerDart", target))
		{
			PlaySoundAttach(target);
			
			target->CreateEffect(TracerDartTimer, 20, 1, GetController(), Tracer_Color);

			// Broadcast to allies
			for (var i = 0; i < GetPlayerCount(); ++i)
			{
				var player = GetPlayerByIndex(i);
				if (Hostile(GetController(), player))
				{
					continue;
				}
				var rocket_launcher = 0;
				var crew_index = GetCrewCount(player) - 1;
				for (var crew_index = GetCrewCount(player) - 1; crew_index >= 0; --crew_index)
				{
					var crew = GetCrew(player, crew_index);
					var rocket_launcher = crew->FindContents(CMC_Weapon_RocketLauncher);
					if (rocket_launcher/* || (crew->Contained()->GetID() == CMC_Vehicle_Blackhawk)*/)
					{
						// TODO: Event info message
						//EventInfo4K(iPlr+1, Format("$TargetMarked$", GetPlrColorDw(GetController()), GetPlayerName(GetController())), IC17, 0, 0, 0, "Info_Event.ogg");
						//Sound("RadioConfirm*.ogg", true, 0, 0, iPlr+1);
						PlaySoundRadio();
						break;
					}
				}
			}

			if (Hostile(target->GetOwner(), GetController()))
			{
				// TODO:
				// Punkte bei Belohnungssystem (Peilsender platziert)
				// DoPlayerPoints(BonusPoints("TracerSet"), RWDS_TeamPoints, GetController(), GetCursor(GetController()), IC17);
				// Achievement-Fortschritt (Radio Warfare)
				// DoAchievementProgress(1, AC19, GetController());
			}
		}
		
		DrawLaserBeam();
	}
}


/**
 Callback if the projectile collides with the landscape.
 */
public func OnHitLandscape()
{
	DrawLaserBeam();
}

/* --- Display --- */



func DrawLaserBeam()
{
	var laser = CreateObject(LaserEffect, 0, 0, user->GetController());
	laser.Visibility = VIS_Owner;
	laser->Line(GetX(), GetY(), Tracer_StartX, Tracer_StartY)
	     ->SetWidth(1)
		 ->Color(Tracer_Color)
		 ->SetLifetime(4)
		 ->Activate();
}

/* --- Sounds --- */

func PlaySoundRadio()
{
	// TODO
}

func PlaySoundAttach(object target)
{
	// TODO
}

/* --- Effect --- */

local TracerDartTimer = new Effect 
{
	Start = func (int temp, int player, int color)
	{
		if (!temp)
		{
			this.By_Player = player;
			this.LifeTime = 50 * 35;
			this.Team = GetPlayerTeam(player);
			color = SplitRGBaValue(GetPlayerColor(player));
			this.Color = RGBa(color.R, color.G, color.B, 250); //color;
			
			// Tag the target
			var tag = CMC_Icon_SensorBall_Tag->Get(this.Target, player, CMC_Projectile_TracerDart);
			if (tag)
			{
				tag->~RefreshRemoveTimer(this);
			}
			else
			{
				tag = CMC_Icon_SensorBall_Tag->AddTo(this.Target, player, CMC_Projectile_TracerDart, "Target");
				if (tag)
				{
					tag.RemoveTime = this.LifeTime;
				}
			}
		}
		return FX_OK;
	},
	
	Stop = func (int temp)
	{
		if (!temp)
		{
			var player = this.By_Player;
			if (!GetPlayerName(player)) return;
		    if (!Hostile(this.Target->GetKiller(), player) && this.Target->GetKiller() != player)
			if (!this.Target->GetAlive())
		    {
				//TODO: Punkte bei Belohnungssystem (Kill Assist durch Peilsender)
				//DoPlayerPoints(BonusPoints("TracerAssist"), RWDS_TeamPoints, iPlr, GetCursor(iPlr), IC22);
		    }
		}
	},
	
	Timer = func (int time)
	{
		if (!this.Target) return FX_Execute_Kill;
	
		// Countdown
		if (time > this.LifeTime)
		{
			return FX_Execute_Kill;
		}
	
		// Destroy on contact with water
		if (this.Target->InLiquid()) return FX_Execute_Kill;
	
		// Destroy if target is not hostile anymore
		var team = this.Target->~GetTeam() ?? GetPlayerTeam(this.Target->GetController());
		if (team && team == this.Team)
		{
			return FX_Execute_Kill;
		}
	
		// Destroy tracer if target requests it
		if (this.Target->~RemoveTracer(this))
		{
			return FX_Execute_Kill;
		}
		
		if (time % 10 == 0)
		{
			this.Target->CreateLEDEffect(this.Color, 0, 0, 20, 20, true);
		}
	},
};
