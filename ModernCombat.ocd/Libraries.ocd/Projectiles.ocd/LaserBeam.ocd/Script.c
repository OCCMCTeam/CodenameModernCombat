#include Library_Projectile

local laser_beam;

func Initialize()
{
	SetObjectBlitMode(GFX_BLIT_Additive);
	laser_beam = {};
}

func GetProjectileColor()
{
	return laser_beam.color ?? 0xffff0000;
}

func OnHitObject(object target)
{
}

func OnHitLandscape()
{
}

func OnHitScan(int x_start, int y_start, int x_end, int y_end)
{
	var laser = CreateObject(LaserEffect, x_start - GetX(), y_start - GetY(), NO_OWNER);

	laser->Line(x_start, y_start, x_end, y_end)
		 ->SetWidth(laser_beam.width ?? 5)
		 ->SetLifetime(laser_beam.lifetime ?? 20)
		 ->Color(this->GetProjectileColor())
		 ->Activate();
}

func LaserWidth(int width)
{
	laser_beam.width = width;
	return this;
}

func LaserLifetime(int lifetime)
{
	laser_beam.lifetime = lifetime;
	return this;
}

func LaserColor(int color)
{
	laser_beam.color = color;
	return this;
}
