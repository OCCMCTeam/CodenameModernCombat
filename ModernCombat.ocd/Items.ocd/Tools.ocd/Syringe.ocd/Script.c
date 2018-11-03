/**
	Syringe
*/

#include Library_ObjectLimit


/* --- Engine callbacks --- */

func RejectEntrance(object into)
{
	return into->ContentsCount(GetID()) >= 1;
}


/* --- Display --- */

// Callback from the item status HUD element: What should be displayed?
public func GetGuiItemStatusProperties(object user)
{
	var status = new GUI_Item_Status_Properties {};
	
	var configuration = Format("<c %x>%s</c>", GUI_CMC_Text_Color_Highlight, this->GetName());
	status->SetObjectConfiguration(configuration);

	return status;
}


/* --- User Interface --- */

func ControlUseItemConfig(object user, int x, int y, int status)
{
	if (status == CONS_Down)
	{
		return ControlUse(user, x, y);
	}
	else
	{
		return false;
	}
}

func ControlUse(object user, int x, int y)
{
	// User has to be ready to act
	if (user->~HasActionProcedure())
	{
		Sting(user);
	}
	return true;
}

/* --- Functionality --- */

func Sting(object user)
{
	// Not implemented yet
}

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;

local ObjectLimitPlayer = 3;
