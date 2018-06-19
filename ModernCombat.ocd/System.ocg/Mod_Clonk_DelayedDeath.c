#appendto Clonk

/* --- Engine callbacks --- */

func Recruitment(int player)
{
	// If the rule object does not exist (rule object disables the delayed death)
	CMC_Rule_MortalWounds->SetDelayedDeath(!CMC_Rule_MortalWounds->GetInstance(), this);
	return _inherited(player, ...);
}

/* --- Better death animation --- */

func StartDead()
{
	// Blend death animation with other animations, except for the death slot
	var merged_animations = false;	
	for (var slot = 0; slot < CLONK_ANIM_SLOT_Death; ++slot)
	{
		if (GetRootAnimation(slot) == nil) continue;
		OverlayDeathAnimation(slot);
		merged_animations = true;
	}

	// Force the death animation if there were no other animations active
	if (!merged_animations)
	{
		OverlayDeathAnimation(CLONK_ANIM_SLOT_Death);
	}

	// Update carried items
	UpdateAttach();
	// Set proper turn type
	SetTurnType(1);
}

// Merges the animation in an animation slot with the death animation
// More variation is possible if we considered adding a random value for the length,
// or if we set the parameters according to current speed, etc.
func OverlayDeathAnimation(int slot)
{
	var animation = "Dead";
	PlayAnimation(animation, slot, Anim_Linear(0, 0, GetAnimationLength(animation), 20, ANIM_Hold), Anim_Linear(0, 0, 1000, 10, ANIM_Remove));
}
