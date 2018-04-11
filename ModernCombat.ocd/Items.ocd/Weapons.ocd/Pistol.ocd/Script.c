#include Library_AmmoManager
#include Library_Firearm

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;

public func GetCarryTransform()
{
	return Trans_Mul(Trans_Rotate(90, 1, 0, 0), Trans_Translate(-2500, 800, 0), Trans_Scale(800, 800, 800));
}


public func Initialize()
{
	_inherited(...);
	this.MeshTransformation = Trans_Scale(800, 800, 800);

	// fire mode list
	ClearFiremodes();
	AddFiremode(FiremodeStandard());
}

func Definition(id def)
{
	def.PictureTransformation = Trans_Mul(Trans_Rotate(-20, 0, 1, 0), Trans_Rotate(-20, 0, 0, 1), Trans_Rotate(5, 1, 0, 0), Trans_Translate(-1800, 0, -3000));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Using

local carry_mode = CARRY_Hand;

public func GetCarryMode(object clonk, bool idle, bool nohand)
{
	if (idle || nohand)
	{
		return CARRY_None;
	}
	return carry_mode;
}
public func GetCarrySpecial(object user) { return "pos_hand2"; }
public func GetCarryBone() { return "Grip"; }

func IsUserReadyToUse(object user)
{
	return user->HasActionProcedure(false); // must not grab landscape
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Fire modes


func FiremodeStandard()
{
	return
	{
		Prototype = fire_mode_default, 
		name = 				"$Bullets$",
		mode = 				WEAPON_FM_Single,
		icon = 				nil,
		
		ammo_id = 			CMC_Ammo_Bullets,
		ammo_load =         15,
	
		delay_reload =		40,
		delay_recover = 	5,
	
		damage = 			11,
	
		// TODO
		projectile_id = 	NormalBullet,
		projectile_speed = 	250,
		projectile_range =  450,
		projectile_spread = Projectile_Deviation(1, 1),
	
		projectile_distance = 8,
		projectile_offset_y = -6,
	};
}

