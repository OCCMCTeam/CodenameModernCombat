/**
	Should be backported to OC
	
	Overloadable animation sounds.
	
	Caution: Could cause problems with inherited and appendto at the moment because it overwrites the original functions!
	
	Note: Functions changed only where "// Original code => " is added as a comment.
	Change only these parts when backporting.
 */
 
#appendto Clonk_Animations

/* --- Overwritten functions --- */

func Footstep()
{
	if (GetMaterialVal("DigFree", "Material", GetMaterial(0,10)) == 0)
		PlaySoundMovementStepHard(); // Original code => Sound("Clonk::Movement::StepHard?");
	else
	{
		var dir = Sign(GetXDir());
		var clr = GetAverageTextureColor(GetTexture(0,10));
		var particles =
		{
			Prototype = Particles_Dust(),
			R = (clr >> 16) & 0xff,
			G = (clr >> 8) & 0xff,
			B = clr & 0xff,
		};
		CreateParticle("Dust", PV_Random(dir * -2, dir * -1), 8, PV_Random(dir * 2, dir * 1), PV_Random(-2, -3), PV_Random(36, 2 * 36), particles, 5);
		PlaySoundMovementStepSoft(); // Original code => Sound("Clonk::Movement::StepSoft?");
	}
}

func FxFallTimer(object target, effect, int timer)
{
	if(GetAction() != "Jump")
	return -1;
	//falling off ledges without jumping results in fall animation
	if(timer == 2 && GetYDir() > 1)
	{
		PlayAnimation("FallShort", CLONK_ANIM_SLOT_Movement, Anim_Linear(0, 0, GetAnimationLength("FallShort"), 8*3, ANIM_Hold), Anim_Linear(0, 0, 1000, 5, ANIM_Remove));
	}
	if(timer == 2 && GetYDir() < 1)
	{
		PlaySoundMovementRustle(); // Original code => Sound("Clonk::Movement::Rustle?");
	}

	if(GetYDir() > 55 && GetAction() == "Jump")
	{
		PlayAnimation("FallLong", CLONK_ANIM_SLOT_Movement, Anim_Linear(0, 0, GetAnimationLength("FallLong"), 8*3, ANIM_Hold), Anim_Linear(0, 0, 1000, 5, ANIM_Remove));
		return -1;
	}
}

func DoKneel(bool create_dust)
{
	var iKneelDownSpeed = 18;

	SetXDir(0);
	SetAction("Kneel");
	PlaySoundMovementRustleLand(); // Original code => Sound("Clonk::Movement::RustleLand");
	PlayAnimation("KneelDown", CLONK_ANIM_SLOT_Movement, Anim_Linear(0, 0, GetAnimationLength("KneelDown"), iKneelDownSpeed, ANIM_Remove), Anim_Linear(0, 0, 1000, 5, ANIM_Remove));

	ScheduleCall(this, "EndKneel", iKneelDownSpeed, 1);
	
	if (create_dust)
	{
		if (GetMaterialVal("DigFree", "Material", GetMaterial(0,10)))
		{
			var clr = GetAverageTextureColor(GetTexture(0,10));
			var particles =
			{
				Prototype = Particles_Dust(),
				R = (clr >> 16) & 0xff,
				G = (clr >> 8) & 0xff,
				B = clr & 0xff,
			};
			CreateParticle("Dust", PV_Random(-4, 4), 8, PV_Random(-3, 3), PV_Random(-2, -4), PV_Random(36, 2 * 36), particles, 12);
		}
	}
	
	return 1;
}

func OnStartRoll()
{	
	SetTurnForced(GetDir());
	PlaySoundMovementRoll(); // Original code => Sound("Clonk::Movement::Roll");
	if(GetDir() == 1) lAnim.rollDir = 1;
	else
		lAnim.rollDir = -1;

	lAnim.rollLength = 22;
	PlayAnimation("KneelRoll", CLONK_ANIM_SLOT_Movement, Anim_Linear(0, 0, 1500, lAnim.rollLength, ANIM_Remove), Anim_Linear(0, 0, 1000, 5, ANIM_Remove));
	AddEffect("Rolling", this, 1, 1, this);
}

public func FxIntDigStart(object target, effect fx, int temp)
{
	if (temp)
		return FX_OK;
	fx.animation = PlayAnimation("Dig", CLONK_ANIM_SLOT_Movement, Anim_Linear(0, 0, GetAnimationLength("Dig"), 36, ANIM_Loop), Anim_Linear(0, 0, 1000, 5, ANIM_Remove));

	// Update carried items
	UpdateAttach();

	// Sound
	PlaySoundActionDig(); // Original code => Sound("Clonk::Action::Dig::Dig?");

	// Set proper turn type
	SetTurnType(0);
	return FX_OK;
}



public func FxIntDigTimer(object target, effect fx, int time)
{
	if (time % 36 == 0)
	{
		PlaySoundActionDig(); // Original code => Sound("Clonk::Action::Dig::Dig?");
	}
	if (time == 18 || time >= 36)
	{
		var no_dig = true;
		for (var shovel in FindObjects(Find_ID(Shovel), Find_Container(this)))
			if (shovel->IsDigging()) 
				no_dig = false;
		if (no_dig)
		{
			SetAction("Walk");
			SetComDir(COMD_Stop);
			return FX_Execute_Kill;
		}
	}
	return FX_OK;
}

/* --- New functions --- */

func PlaySoundMovementStepHard()
{
	Sound("Clonk::Movement::StepHard?");
}

func PlaySoundMovementStepSoft()
{
	Sound("Clonk::Movement::StepSoft?");
}

func PlaySoundMovementRustle()
{
	Sound("Clonk::Movement::Rustle?");
}

func PlaySoundMovementRustleLand()
{
	Sound("Clonk::Movement::RustleLand");
}

func PlaySoundMovementRoll()
{
	Sound("Clonk::Movement::Roll");
}

func PlaySoundActionDig()
{
	Sound("Clonk::Action::Dig::Dig?");
}
