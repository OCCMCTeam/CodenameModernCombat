/* --- Casing --- */

func Initialize()
{
	// Set a custom object layer, so that the object does not hit other objects and is excluded from searches
	SetObjectLayer(this);
	AddTimer(this.Progress, 1);
	this.Phase = Trans_Rotate(RandomX(-10, 10) * 10, 0, 1, 0);
	this.HitSound = "Projectiles::Casing::Hit?";
	this.SpinPos = 0;
	this.SpinDir = 0;
	SetSize(20);
}

func Hit(int dx, int dy)
{
	StartFade();

	if (dy > 1)
	{
		SetXDir(RandomX(-5,5));
		SetYDir(dy / -2, 100);
		SetRDir(-GetRDir());
	}
	
	Sound(this.HitSound, {multiple = true});
}

/* --- Settings --- */

func SetAngle(int angle)
{
	SetR(angle);
	SetRDir(Sign(angle) * RandomX(-10, -20));
	return this;
}

func SetSize(int size) // Size, approx in mm
{
	var scale = size * 5;
	this.Scale = Trans_Scale(scale, scale, scale);
	Update();
	return this;
}

func DoSpin()
{
	this.Phase = Trans_Identity(); // Otherwise spin direction looks like normal rotation
	this.SpinDir = RandomX(-4, -8) * Sign(GetRDir());
	SetRDir(GetRDir() / 2);
}

func TypeShotgun()
{
	SetSize(70);
	this.HitSound = "Projectiles::Casing::HitSpecial?";
	SetGraphics("Shotgun");
	return this;
}

func TypeRifle()
{
	SetSize(45);
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
		this.SpinDir -= Sign(this.SpinDir); // Reduce by 1
		SetRDir(GetRDir() / 10);
		StartFade();
	}
	
	if (this.SpinDir)
	{
		this.SpinPos = (this.SpinPos + this.SpinDir) % 360;
		Update();
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

func Update()
{
	this.MeshTransformation = Trans_Mul(this.Phase, this.Scale, Trans_Rotate(this.SpinPos, 1, 0, 0));
}
