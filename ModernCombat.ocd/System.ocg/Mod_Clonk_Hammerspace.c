/**
	Animations for clonk
	
	Reaching into hammer space upon item selection.
	
 */
 
#appendto Clonk

/* --- Callbacks from ClonkControl --- */


func OnSlotFull(int slot)
{
	StartHammerspaceAnimation(slot);
	return _inherited(slot, ...);
}

func OnSlotEmpty(int slot)
{
	StartHammerspaceAnimation(slot);
	return _inherited(slot, ...);
}

/* --- Animation --- */

func StartHammerspaceAnimation(int slot)
{
	var animation = GetEffect("IntHammerspaceAnimation", this) ?? CreateEffect(IntHammerspaceAnimation, 1, 1);
	var inventory_slot = GetItemPos(GetHandItem(slot));
	animation->SetInventorySlot(inventory_slot);
}

local IntHammerspaceAnimation = new Effect
{
	ReachTime = 5,
	SelectionTime = 35,
	SelectionTime_Default = 35,

	Start = func (int temp, int slot)
	{
		if (!temp)
		{
			SetInventorySlot(slot);
			this.AnimProgress = 0; // of 1000
			ReachBack();
		}
	},

	Timer = func ()
	{
		this.AnimProgress = BoundBy(this.AnimProgress + this.AnimProgress_Change, 0, 1000);
		Animation();
		var now = FrameCounter();
		if (this.SlotChange != nil)
		{
			var change = now - this.SlotChange;
			if (change >= this.ReachTime && this.AnimProgress_Change >= 0)
			{
				ReachForward();
			}
			if (change >= this.SelectionTime)
			{
				return FX_Execute_Kill;
			}
		}
		return FX_OK;
	},

	Stop = func (int temp)
	{
		if (!temp && this.Target)
		{
			if (this.AnimIndex_Arm_Upper)
			{
				this.Target->StopAnimation(this.AnimIndex_Arm_Upper);
			}
			this.Target->UpdateAttach();
		}
	},

	ReachBack = func ()
	{
		this.AnimProgress_Change = +1000 / this.ReachTime;
	},

	ReachForward = func ()
	{
		var time = this.SelectionTime - this.ReachTime;
		if (time <= 0)
		{
			this.SelectionTime = 2 * this.ReachTime;
			time = this.ReachTime;
		}
		this.AnimProgress_Change = (-1000) / time;
		this.Target->UpdateAttach();
	},

	Animation = func ()
	{
		if (this.AnimIndex_Arm_Upper == nil)
		{
			if (this.Target->GetDir() == DIR_Left)
			{
				this.Anim_Side = "L";
			}
			else
			{
				this.Anim_Side = "R";
			}
			var dir = this.Target->GetCalcDir();
			this.AnimIndex_Arm_Upper = this.Target->PlayAnimation("Hammerspace", CLONK_ANIM_SLOT_Arms, Anim_Linear(0, 0, this.Target->GetAnimationLength("Hammerspace"), this.ReachTime, ANIM_Hold), Anim_Const(1000));

		}

		if (this.AnimIndex_Arm_Upper != nil)
		{
			if (this.AnimProgress == 0 && this.AnimProgress_Change < 0)
			{
				this.Target->StopAnimation(this.AnimIndex_Arm_Upper);
				this.AnimIndex_Arm_Upper = nil;
			}
			else
			{
				this.Target->SetAnimationPosition(this.AnimIndex_Arm_Upper, Anim_Const(this.AnimProgress / 2));
				this.Target->SetAnimationWeight(this.AnimIndex_Arm_Upper, Anim_Const(BoundBy(this.AnimProgress * 2, 0, 1000)));
			}
		}
	},

	SetInventorySlot = func (int slot)
	{
		if (slot != this.Slot)
		{
			var now = FrameCounter();
			var change = now - this.SlotChange;

			if (change >= this.ReachTime)
			{
				ReachBack();
			}

			var item = this.Target->GetHandItem(slot);
			this.SlotChange = now;

			if (item)
			{			
				this.SelectionTime = item->~SelectionTime() ?? this.SelectionTime_Default;
			}
			else
			{
				this.SelectionTime = this.SelectionTime_Default;
			}
		}
		this.Slot = slot;
	},
};

/* --- Prevent using items --- */

func IsSwitchingItems(bool is_stashing)
{

	var hammerspace = GetEffect("IntHammerspaceAnimation", this);
	if (is_stashing)
	{
		return hammerspace && hammerspace.AnimProgress_Change >= 0;
	}
	else
	{
		return !!hammerspace;
	}
}

// Handle controls to the clonk
public func ObjectControl(int plr, int ctrl, int x, int y, int strength, bool repeat, int status)
{
	if (!this)
		return false;

	// Handle reloading?
	if (ctrl == CON_CMC_Reload
	 || ctrl == CON_CMC_DrawGrenade
	 || ctrl == CON_CMC_UseAlt
	 || ctrl == CON_Use
	 || ctrl == CON_UseAlt
	 || ctrl == CON_Throw)
	{
		if (IsSwitchingItems()) // Block all actions until the selection time is over
		{
			return true;
		}
	}
	
	return _inherited(plr, ctrl, x, y, strength, repeat, status);
}

func HasHandAction(slot, just_wear, bool needs_both_hands)
{
	if ((!slot || needs_both_hands) // First hand, or needs both hands
	 && IsSwitchingItems(true))     // Only while putting the item back, so that it is displayed in the hand when reaching forward
	{
		return false;
	}

	return inherited(slot, just_wear, needs_both_hands);
}

