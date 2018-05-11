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

// Handle CON_CMC_AimingCursor
public func ObjectControl(int plr, int ctrl, int x, int y, int strength, bool repeat, int status)
{
	if (!this)
		return false;

	if (ctrl == CON_CMC_AimingCursor)
	{
		// save last mouse position:
		// if the using has to be canceled, no information about the current x,y
		// is available. Thus, the last x,y position needs to be saved
		this.control.mlastx = x;
		this.control.mlasty = y;

		var contents = this->GetHandItem(0);

		if (contents)
			if (ControlUse2Script(ctrl, x, y, strength, repeat, status, contents))
				return true;
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

	return handled;
}

/* --- Respawn stuff --- */

// Find out whether the crew is currently respawning
public func IsRespawning()
{
	return Contained() && Contained()->GetID() == Arena_RelaunchContainer;
}

// Opens the respawn menu
public func OpenRespawnMenu()
{
	if (IsRespawning())
	{
		// Open menu
		var menu = new CMC_GUI_RespawnMenu {};
		menu->Assemble(this)
		    ->Show()
		    ->Open(GetOwner());
		SetMenu(menu->GetRootID());

		// Add class selection tabs
		for (var class in this->~GetAvailableClasses())
		{
			menu->GetTabs()->AddTab(class,                                     // identifier
			                        class->~GetName(),                         // label text
			                        DefineCallback(this.SetCrewClass, class)); // called on button click
		}
		menu->GetTabs()->SelectTab();
	}
}

