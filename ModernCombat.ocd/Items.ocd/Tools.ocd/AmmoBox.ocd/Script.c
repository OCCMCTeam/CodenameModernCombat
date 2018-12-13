/**
	Ammo Box

	Should NOT have an extra slot, because we do not want the user to take ammo out of it
	by means other than our own user interface.
*/

#include Library_ObjectLimit


/* --- Engine callbacks --- */

func RejectCollect(id type, object item)
{
	// Must contain ammo exclusively
	return RejectStack(item) || _inherited(type, item, ...);
}

func Collection2(object item)
{
	UpdateAmmoBoxGraphics();
	return _inherited(item, ...);
}

func Ejection(object item)
{
	UpdateAmmoBoxGraphics();
	return _inherited(item, ...);
}

func Hit()
{
	Sound("Items::Tools::AmmoBox::Hit?");
}


/* --- Display --- */

// Update the graphics
func UpdateAmmoBoxGraphics()
{
	var graphics_name = nil;
	if (Contents())
	{
		var type = Contents()->GetID();
		if (CMC_Ammo_Bullets == type)
		{
			graphics_name = "Bullets";
		}
		else if (CMC_Ammo_Grenades == type)
		{
			graphics_name = "Grenades";
		}
		else if (CMC_Ammo_Missiles == type)
		{
			graphics_name = "Missiles";
		}
	}

	SetGraphics(graphics_name, GetID());
}

// Callback from the item status HUD element: What should be displayed?
public func GetGuiItemStatusProperties(object user)
{
	var status = new GUI_Item_Status_Properties {};

	var configuration = Format("<c %x>%s</c>", GUI_CMC_Text_Color_Highlight, this->GetName());
	if (Contents())
	{
		status->SetObjectCount(Contents()->~GetStackCount() ?? 0);
		configuration = Format("%s - %s", configuration, Contents()->GetName());
	}
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
		UnpackAmmo(user);
	}
	return true;
}

/* --- Functionality --- */


func RejectStack(object item)
{
	// Is allowed to collect ammo, but only if we do not have ammo
	if (item->~IsAmmo() && !Contents())
	{
		return false;
	}
	// Reject anything else
	return true;
}

// Callback for the spawn point: Create the desired ammo in this box.
func SpawnAs(id ammo_type)
{
	CreateContents(ammo_type);
}

// Unpack ammo and give it to user
func UnpackAmmo(object user)
{
	// Sanity checks
	AssertNotNil(user);
	if (!Contents())
	{
		RemoveObject();
		return false;
	}

	// Transfer the ammo
	var type = Contents()->GetID();
	var available = Contents()->GetStackCount();
	var transferred = user->DoAmmo(type, available);

	// Nothing transferred?
	if (transferred == 0)
	{
		return false;
	}

	// Effect
	user->Sound("Items::Tools::AmmoBox::ResupplyIn?", {player = user->GetOwner()});

	// Cleanup
	Contents()->DoStackCount(-transferred);
	if (!Contents())
	{
		RemoveObject();
	}
	return true;
}

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;

local ObjectLimitPlayer = 3;
