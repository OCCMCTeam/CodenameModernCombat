#appendto Clonk

/* --- Use controls --- */

// Override for Objects/Libraries/ClonkUseControl
// This will reenable the "Alt" callbacks for using_type C4D_Object
// Also, handle CON_CMC_AimingCursor
func GetUseCallString(string action)
{
	// Control... or Contained...
	var control_string = "Control";
	if (this.control.using_type == C4D_Structure)
		control_string = "Contained";
	// ..Use.. or ..UseAlt...
	var estr = "";
	//if (this.control.alt && this.control.using_type != C4D_Object) // This is the original
	if (this.control.alt) // This is enough for the purpose of CMC
		estr = "Alt";
	// Action
	if (!action)
		action = "";
	return Format("~%sUse%s%s", control_string, estr, action);
}

// Handle controls to the clonk
public func ObjectControl(int plr, int ctrl, int x, int y, int strength, bool repeat, int status)
{
	if (!this)
		return false;

	// Handle reloading?
	if (ctrl == CON_CMC_Reload)
	{
		var contents = this->GetHandItem(0);
		if (contents && contents->~StartReload(this, x, y, true))
		{
			return true;
		}
	}
	
	// Handle item configuration
	// This sort of blends in with the use calls,
	// so that it becomes clear that it cannot be
	// done while RejectUse() returns true.
	if (ctrl == CON_CMC_ItemConfig && status == CONS_Up)
	{
		var contents = this->GetHandItem(0);
		if (contents
		&& !contents->~RejectUse(this)
		&&  contents->~ControlUseItemConfig(this, x, y))
		{
			return true;
		}
	}
	
	// Handle aiming?
	if (ctrl == CON_CMC_AimingCursor)
	{
		// save last mouse position:
		// if the using has to be canceled, no information about the current x,y
		// is available. Thus, the last x,y position needs to be saved
		this.control.mlastx = x;
		this.control.mlasty = y;

		var contents = this->GetHandItem(0);
		if (contents && ControlUse2Script(ctrl, x, y, strength, repeat, status, contents))
		{
			return true;
		}
	}

	return _inherited(plr, ctrl, x, y, strength, repeat, status);
}

// Handle CON_CMC_AimingCursor
func ControlUse2Script(int ctrl, int x, int y, int strength, bool repeat, int status, object obj)
{
	if (ctrl == CON_CMC_AimingCursor)
		return AimingUseControl(x, y, obj);

	return _inherited(ctrl, x, y, strength, repeat, status, obj);
}

// Handle CON_CMC_AimingCursor
func AimingUseControl(int x, int y, object obj)
{
	// automatic adjustment of the direction

	// doing something were aiming is possible
	var proc = GetProcedure();
	if (proc != "ATTACH" && !Contained() && proc != "SCALE" && proc != "PUSH")
	{
		if ((GetComDir() == COMD_Stop && GetXDir() == 0) || GetProcedure() == "FLIGHT")
		{
			if (GetDir() == DIR_Left)
			{
				if (x > 0)
					SetDir(DIR_Right);
			}
			else
			{
				if (x < 0)
					SetDir(DIR_Left);
			}
		}
	}

	var handled = obj->Call(GetUseCallString("Aiming"), this, x, y);
	
	// Adjust cursor
	this->~UpdateCmcVirtualCursor(x, y);

	return handled;
}

/* --- Menu controls --- */

func Control2Menu(int control, int x, int y, int strength, bool repeat, int status)
{
	if (this->GetMenu())
	{
		var control_object = this->GetMenu().ControlObject;
		if (control_object)
		{
			control_object->~ControlMenu(this, control, x, y, strength, repeat, status);
		}
	}
	return _inherited(control, x, y, strength, repeat, status);
}

// Additional parameter: control_object
func SetMenu(new_menu, bool unclosable, object control_object)
{
	var menu = _inherited(new_menu, unclosable);
	if (GetType(menu) == C4V_PropList && control_object)
	{
		menu.ControlObject = control_object;
	}
	return menu;
}
