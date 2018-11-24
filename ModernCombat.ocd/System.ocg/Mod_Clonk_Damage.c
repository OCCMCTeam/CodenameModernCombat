/**
	Make clonks use the damage system.
*/

#appendto Clonk

// Add screen fading effect and sound system
func Construction(object creator)
{
	_inherited(creator, ...);
	
	CreateEffect(FxCmcDamageSystem, 1);
}


func CatchBlow(int health_change, object from)
{
	// Bleed if alive and outside, and damaged
	if (this->GetAlive() && this->Contained() == nil && health_change < 0)
	{
		var x = 0, y = 0;
		var bleed = false; // Do not bleed for every damage type, that would be weird

		// Blood burst from projectiles?
		if (from && from->~IsProjectile() && from->Contained() == nil)
		{
			x = from->GetX() - GetX();
			y = from->GetY() - GetY();
			bleed = true;
		}
		
		// Blood burst from explosions?
		// Note: Catch blow happens after the damage call
		var type = nil;
		var memory = GetEffect(FxCmcDamageSystem.Name, this);
		if (memory)
		{
			type = memory.last_cause;
			if (type == FX_Call_EngBlast)
			{
				bleed = true;
			}
		}
		
		if (bleed)
		{
			var damage = Min(-health_change, 200);	
			Splatter(damage, type, from);
			BloodBurst(damage, x, y);
			BloodSplatter(damage, 0, 0);
		}
	}

	// The usual
	_inherited(health_change, from, ...);
}



func BloodBurst(int size, int x, int y, int color)
{
	var values = SplitRGBaValue(color ?? RGB(RandomX(70, 195)));
	CreateParticle("BloodBurst", x, y, 0, 1, RandomX(30, 50), 
	{
	    Size = size,
		Phase = PV_Linear(0, 15),
		Rotation = 0,
		Alpha = 255,
		R = values.R, G = values.G, B = values.B,
		Attach = ATTACH_Front,
	});
}

func BloodSplatter(int size, int x, int y, int color)
{
	// Not in air
	if (BloodInAir()) return;

	var lifetime = RandomX(240, 360);
	var values = SplitRGBaValue(color ?? RGB(RandomX(70, 195)));
	
	var splat = CreateObject(Dummy, 0, 0, NO_OWNER);
	splat.Visibility = VIS_All;
	splat.Plane = 1;
	splat->
	CreateParticle("BloodSplatter", x, y, 0, 0, lifetime,
	{
		Size = size,
		Phase = PV_Random(0, 2),
		Rotation = PV_Random(-180, 180),
		Alpha = PV_KeyFrames(0, 0, 0, 100, 200, 800, 200, 1000, 0),
		R = values.R, G = values.G, B = values.B,
		Attach = ATTACH_Back,
	});
	ScheduleCall(splat, splat.RemoveObject, lifetime + 1);
}

func BloodSplatter2(int size, int x, int y, int angle, int color)
{
	//if(!GetEffectData(EFSM_Blood)) return;

	if (BloodInAir()) return;

	angle += 180;

	var values = SplitRGBaValue(color ?? RGB(RandomX(70, 195)));
	var radius = size/2 - 5;
	var lifetime = RandomX(180, 360);
	
	var splat = CreateObject(Dummy, 0, 0, NO_OWNER);
	splat.Visibility = VIS_All;
	splat.Plane = 2;
	splat->
	CreateParticle("BloodSplatter2", x + Sin(angle, radius), y - Cos(angle, radius), 0, 0, lifetime,
	{
		Size = size,
		Phase = PV_Random(0, 3),
		Rotation = PV_Random(angle - 10, angle + 10),
		Alpha = PV_KeyFrames(0, 0, 0, 100, 200, 800, 200, 1000, 0),
		R = values.R, G = values.G, B = values.B,
		Attach = ATTACH_Back,
	});
	ScheduleCall(splat, splat.RemoveObject, lifetime + 1);
}

func Splatter(int amount, int type, object from, int color)
{
	var x,y;
	var splatter_scale = 50;
	
	amount = splatter_scale * amount / 100;

	if (amount < 10)
	{
		if(!Random(5 - amount/2))
		{
			return;
		}
		var range_x = GetID()->GetDefWidth()/3; 
		var range_y = GetID()->GetDefHeight()/3; 
		x = RandomX(-range_x, +range_x);
		y = RandomX(-range_y, +range_y);
	}

	color = color ?? RGB(RandomX(70, 195));
	var size = BoundBy(amount, 8, 80);

	if (from)
	{
		//if((iType == DMG_Explosion)||
		//	 (pFrom->~IsBullet()))
		//{
			var angle = from->GetR() - 180;
			if (!angle) angle = Angle(GetX(),GetY(), from->GetX(), from->GetY());

			x += Sin(angle, GetDefWidth()/2);
			y -= Cos(angle, GetDefHeight()/2);

			BloodSplatter2(Min(size * 2, 100), x, y, angle, color);
		//}
	}
	var burst_size = Min(size * 3, 100);
	BloodSplatter(burst_size, x, y, color);
	BloodBurst(burst_size, x, y, color);
}

func BloodInAir(int x, int y)
{
	var material = GetMaterial(x, y);
	return (material == -1) || (GetMaterialVal("Density", "Material", material) != 0);
	
}
