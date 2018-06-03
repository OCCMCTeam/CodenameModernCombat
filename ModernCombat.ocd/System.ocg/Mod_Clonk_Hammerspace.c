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
		if (!temp)
		{
			if (this.AnimIndex_Arm_Upper)
			{
				this.Target->StopAnimation(this.AnimIndex_Arm_Upper);
			}
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
			
			this.AnimPos_Arm_Upper = this.Target->GetCalcDir() * 90;
			this.AnimIndex_Arm_Upper = this.Target->TransformBone(Format("skeleton_arm_upper.%s", this.Anim_Side), Trans_Identity(), CLONK_ANIM_SLOT_Arms + 1, Anim_Const(1000));
		}

		if (this.AnimIndex_Arm_Upper != nil)
		{
			if (this.AnimProgress == 0 && this.AnimProgress_Change < 0)
			{
				this.Target->StopAnimation(this.AnimIndex_Arm_Upper);
				this.AnimIndex_ArmUpper = nil;
			}
			else
			{
				var angle = this.AnimProgress * this.AnimPos_Arm_Upper / 1000;
				this.Target->SetAnimationBoneTransform(this.AnimIndex_Arm_Upper, Trans_Rotate(angle, 0, 0, 1));
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

// TODO
