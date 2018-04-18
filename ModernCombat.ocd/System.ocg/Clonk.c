#appendto Clonk

// Override for Objects/Libraries/ClonkUseControl
// This will reenable the "Alt" callbacks for using_type C4D_Object
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