#include CMC_Firearm_Basic

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;

/* --- Engine callbacks --- */

public func Initialize()
{
	_inherited(...);

	// Fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeGrenades_Explosive());
	AddFiremode(FiremodeGrenades_Cluster());
	AddFiremode(FiremodeGrenades_Smoke());
}

func Definition(id weapon)
{
	weapon.PictureTransformation = Trans_Mul(Trans_Rotate(230, 0, 1, 0), Trans_Rotate(-15, 0, 0, 1), Trans_Rotate(10, 1, 0, 0), Trans_Translate(0, 0, -2000));
}

/* --- Display --- */

public func GetCarryMode(object clonk, bool not_selected)
{
	if (not_selected || !IsUserReadyToUse(clonk))
	{
		return CARRY_Back;
	}
	else
	{
		return CARRY_Blunderbuss;
	}
}
public func GetCarryBone() { return "Grip"; }
public func GetCarryTransform(object clonk, bool not_selected, bool nohand, bool onback)
{
	if (not_selected || !IsUserReadyToUse(clonk)) // On belt?
	{
		return Trans_Mul(Trans_Translate(0, 200), Trans_Rotate(90, 1, 0, 0));
	}
	else
	{
		return Trans_Mul(Trans_Rotate(90, 1, 0, 0), Trans_Translate(-2200, 600));
	}
}
public func GetCarrySpecial(clonk)
{
	if(IsAiming()) return "pos_hand2";
}

/* --- Fire modes --- */

func FiremodeGrenades()
{
	var mode = new Library_Firearm_Firemode {};
	
	// Generic info
	mode->SetMode(WEAPON_FM_Single);

	// Reloading
	mode->SetAmmoID(CMC_Ammo_Grenades);
	mode.ammo_load = 8;
	mode->SetRecoveryDelay(60);
	mode->SetReloadDelay(210);
	mode->SetDamage(20);

	// Projectile
	mode->SetProjectileSpeed(120);
	mode->SetProjectileRange(10000); // FIXME, not necessary once there are grenades
	mode->SetProjectileDistance(12);
	mode->SetYOffset(-6);

	// Ironsight aiming
	mode->SetIronsightType(WEAPON_FM_IronsightBlend);
	mode->SetIronsightDelay(15);
	mode->SetIronsightAimingAnimation("MusketAimArms");
	mode->SetForwardWalkingSpeed(95);
	mode->SetBackwardWalkingSpeed(65);
	
	// Spread
	mode->SetSpreadPerShot(ProjectileDeviationCmc(270));
	mode->SetSpreadBySelection(ProjectileDeviationCmc(80));
	mode->SetSpreadLimit(ProjectileDeviationCmc(400));

	// Crosshair, CMC Custom
	mode->SetAimCursor([CMC_Cursor_Cone]);

	// Effects, CMC custom
	mode->SetFireSound("Items::Weapons::AssaultRifle::Fire", 3);
	return mode;
}

func FiremodeGrenades_Explosive()
{
	var mode = FiremodeGrenades();
	
	// Generic info
	mode->SetName("$Explosive$");
	
	// Grenade
	mode->SetProjectileID(CMC_Projectile_Bullet); // FIXME: obviously :D
	return mode;
}

func FiremodeGrenades_Cluster()
{
	var mode = FiremodeGrenades();
	
	// Generic info
	mode->SetName("$Cluster$");
	
	// Grenade
	mode->SetProjectileID(CMC_Projectile_Bullet); // FIXME: obviously :D
	return mode;
}

func FiremodeGrenades_Smoke()
{
	var mode = FiremodeGrenades();
	
	// Generic info
	mode->SetName("$Smoke$");
	
	// Grenade
	mode->SetProjectileID(CMC_Projectile_Bullet); // FIXME: obviously :D
	return mode;
}


/* --- Effects --- */

func FireSound(object user, proplist firemode)
{
	Sound(firemode->GetCurrentFireSound(), {multiple = true});
}

func OnFireProjectile(object user, object projectile, proplist firemode)
{
	// Should not need an effect
	// TODO: Add part of the user speed?
	
	var controller = user->GetController();
	if (GetType(controller) == C4PT_User)
	{
		SetPlrView(controller, projectile);
	}
	
	// TODO: Can be removed again later
	ScheduleCall(projectile, projectile.SetAction, 1, 1, "TravelBallistic");
	
}

func FireEffect(object user, int angle, proplist firemode)
{
	var x = +Sin(angle, firemode->GetProjectileDistance());
	var y = -Cos(angle, firemode->GetProjectileDistance() + firemode->GetYOffset());

	var user_xdir = user->GetXDir();
	var user_ydir = user->GetYDir();
	
	var xdir = +Sin(angle, firemode->GetProjectileSpeed()) / 4;
	var ydir = -Cos(angle, firemode->GetProjectileSpeed()) / 4;
	
	CreateParticle("Smoke2", PV_Random(x - 5, x + 5), PV_Random(y - 5, y + 5), PV_Random(user_xdir, user_xdir + xdir), PV_Random(user_ydir, user_ydir + ydir), PV_Random(15, 25),
	{
		Prototype = Particles_ThrustColored(200, 200, 200),
		Size = PV_Random(8, 14),
	}, 20);

	CreateParticle("Thrust", x, y, user_xdir, user_ydir, PV_Random(20, 30),
	{
		Prototype = Particles_ThrustColored(255, 200, 200),
		Size = 12,
	});
}
