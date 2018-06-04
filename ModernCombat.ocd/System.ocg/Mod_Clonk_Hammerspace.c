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
			if (this.AnimIndex_Arm_Lower)
			{
				this.Target->StopAnimation(this.AnimIndex_Arm_Lower);
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
			var dir = this.Target->GetCalcDir();
			this.AnimPos_Arm_Upper = [90 * dir, 50 * dir, -30];
			this.AnimIndex_Arm_Upper = this.Target->TransformBone(Format("skeleton_arm_upper.%s", this.Anim_Side), Trans_Identity(), CLONK_ANIM_SLOT_Arms + 1, Anim_Linear(0, 0, 1000, 3 * this.ReachTime, ANIM_Remove));

			this.AnimPos_Arm_Lower = [(-40) * dir, 60 * dir];
			this.AnimIndex_Arm_Lower = this.Target->TransformBone(Format("skeleton_arm_lower.%s", this.Anim_Side), Trans_Identity(), CLONK_ANIM_SLOT_Arms + 1, Anim_Linear(0, 0, 1000, 3 * this.ReachTime, ANIM_Remove));
		}

		if (this.AnimIndex_Arm_Upper != nil)
		{
			if (this.AnimProgress == 0 && this.AnimProgress_Change < 0)
			{
				this.Target->StopAnimation(this.AnimIndex_Arm_Upper);
				this.AnimIndex_Arm_Upper = nil;
				
				this.Target->StopAnimation(this.AnimIndex_Arm_Lower);
				this.AnimIndex_Arm_Lower = nil;
			}
			else
			{
				var u0 = Trans_Rotate(this.AnimProgress * this.AnimPos_Arm_Upper[0] / 1000, 1, 0, 0);
				var u1 = Trans_Rotate(this.AnimProgress * this.AnimPos_Arm_Upper[1] / 1000, 0, 1, 0);
				var u2 = Trans_Rotate(this.AnimProgress * this.AnimPos_Arm_Upper[2] / 1000, 0, 0, 1);
				
				//var upper = Trans_Mul(u0, u1, u2);
				var upper = Trans_Mul(u1, u2, u0);
				
				var l1 = Trans_Rotate(this.AnimProgress * this.AnimPos_Arm_Lower[0] / 1000, 0, 1, 0);
				var l2 = Trans_Rotate(this.AnimProgress * this.AnimPos_Arm_Lower[1] / 1000, 0, 0, 1);
				
				//var lower = Trans_Mul(l1, l2);
				var lower = Trans_Mul(l2, l1);

				this.Target->SetAnimationBoneTransform(this.AnimIndex_Arm_Upper, upper);
				this.Target->SetAnimationBoneTransform(this.AnimIndex_Arm_Lower, lower);
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
