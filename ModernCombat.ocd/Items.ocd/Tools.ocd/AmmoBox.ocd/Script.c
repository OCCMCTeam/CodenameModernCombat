/**
	Ammo Box
	
	Should NOT have an extra slot, because we do not want the user to take ammo out of it
	by means other than our own user interface.
*/


/* --- Engine callbacks --- */

func RejectCollect(id type, object item)
{
	// The barrel can only contain liquids.
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
	Sound("Tool::AmmoBox::Hit?");
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

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
