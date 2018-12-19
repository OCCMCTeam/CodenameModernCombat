/* --- Casing --- */

func Initialize()
{
	// Set a custom object layer, so that the object does not hit other objects and is excluded from searches
	SetObjectLayer(this);
	AddTimer(this.Progress, 1);
	this.HitSound = "Projectiles::Casing::Hit?";
}

func Hit(int dx, int dy)
{
	StartFade();

	if (dy > 1)
	{
		SetXDir(RandomX(-5,5));
		SetYDir(dy / -2, 100);
		SetRDir(GetRDir(100) * 3 / -4, 100);
	}
	
	Sound(this.HitSound, {multiple = true});
}

/* --- Settings --- */


func SetAngle(int angle)
{
	SetR(angle);
	SetRDir(Sign(angle) * RandomX(-20, -40));
	return this;
}

func SetSize(int size)
{
	var scale = size * 100;
	this.MeshTransformation = Trans_Scale(scale, scale, scale);
	return this;
}

func TypeShotgun()
{
	this.HitSound = "Projectiles::Casing::HitSpecial?";
	SetGraphics("Shotgun");
	return this;
}

func TypeRifle()
{
	this.HitSound = "Projectiles::Casing::HitBig?";
	SetGraphics("Rifle");
	return this;
}

/* --- Behaviour --- */

local is_fading = false;

func Progress()
{
	if (GetContact(-1) || InLiquid())
	{
		SetRDir(GetRDir() / 10);
		StartFade();
	}
}

func StartFade()
{
	if (!is_fading)
	{
		FadeOut(RandomX(350, 450), true);
		is_fading = true;
	}
}
