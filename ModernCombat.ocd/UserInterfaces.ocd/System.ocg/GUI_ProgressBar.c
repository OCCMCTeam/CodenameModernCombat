/**
	Simple proplist with functions for progress bars.

	@author Marky
 */
 

/*
	GUI prototype for a simple progress bar.
	
	Note:
	In the original objects there is a GUI_ProgressBar and derivates
	that define progress bar behavior. The code there is hard to understand
	and those seem suited for attaching progress bars to objects mostly,
	and assume too many defaults.
	
	Usage:
	- Create a layout by var layout = new CMC_GUI_ProgressBar{};
	- Configure the layout for the bar as you would with any GUI element
	- Configure the bar foreground as you would with any GUI element, access layout.GUI_Element_Progress
	- Call layout->AddTo(...) after you opened a menu to add the layout the bar to that menu
	
	You can call various functions on this layout after you have created it;
	As a general rule you can change these values around as much as you like
	and they are applied to the menu only when you call layout->Update(). 
 */
static const CMC_GUI_ProgressBar = new Global
{
	// --- Properties:
	GUI_ID = nil,       // int gui_id - for GuiUpdate()
	GUI_ID_Child = nil, // int child_id - for GuiUpdate()
	GUI_TargetNr = nil, // object target - for GuiUpdate(); Contains only the number to avoid infinite proplist (the object may contain the menu that contains this layout that saves this object as its target)
	GUI_Bar_Name = nil, // the name of the bar - the property name of the sub window in the menu
	GUI_Owner = nil,    // the owner for the bar - for visibility
	
	// --- GUI info
	
	/*
	 * The progress bar background, height, etc. can be defined via this proplist.
	 * The progress bar progress is defined via the GUI_Element_Progress.
	 */

	// The element for the progress bar overlay
	GUI_Element_Progress = nil,
	
	// --- Functions / API
	
	/*
		Adds the progress bar to a menu. Should be called after the menu definition.
		
		@par menu The GUI menu layout proplist.
		@par submenu_name The property will be added with this name
		@par gui_id For GuiUpdate()
		@par child_id For GuiUpdate()
		@par target For GuiUpdate(); Required if you want to set visibility of the bar
	 */
	AddTo = func (proplist menu, string submenu_name, int gui_id, int child_id, object target)
	{
		if (GetType(menu) != C4V_PropList)
		{
			FatalError("Progress bar can be added to a %v only, you passed %v", C4V_PropList, GetType(menu));
		}
		if (menu[submenu_name])
		{
			FatalError("Cannot add progress bar \"%s\", there is already such a sub menu", submenu_name);
		}
		
		menu[submenu_name] = this;
	
		this.GUI_Bar_Name = submenu_name;
		this.GUI_ID = gui_id;
		this.GUI_TargetNr = target->ObjectNumber();
		this.GUI_ID_Child = child_id;
		this.GUI_Owner = target->GetOwner();
	},
	
	/*
		Gets the name of the bar in the GUI layout proplist.
		
		@return string The name.
	 */
	GetName = func ()
	{
		return this.GUI_Bar_Name;
	},
	
	/*
		Sets the background color of the bar.
		
		@par color May be the color, or a proplist with tag information.
		@return proplist The bar layout proplist, for calling further functions.
	 */
	SetBackgroundColor = func (color)
	{
		if (GetType(color) == C4V_Int || GetType(color) == C4V_PropList)
		{
			this.BackgroundColor = color;
			return this;
		}
		else
		{
			FatalError("Color must be %v (for direct setting) or %v (for GUI tag support). Got %v", C4V_Int, C4V_PropList, GetType(color));
		}
	},
	
	/*
		Sets the foreground color of the bar.
		
		@par color May be the color, or a proplist with tag information.
		@return proplist The bar layout proplist, for calling further functions.
	 */
	SetBarColor = func (color)
	{
		if (GetType(color) == C4V_Int || GetType(color) == C4V_PropList)
		{
			this.GUI_Element_Progress = this.GUI_Element_Progress ?? {};
			this.GUI_Element_Progress.BackgroundColor = color;
			return this;
		}
		else
		{
			FatalError("Color must be %v (for direct setting) or %v (for GUI tag support). Got %v", C4V_Int, C4V_PropList, GetType(color));
		}
	},

	/*
		Set a progress value for the bar.
		By default the bar goes from left = empty to right = full.
		
		@par progress Value between 1 and 1000, where 1000 = 100%.
		@return proplist The bar layout proplist, for calling further functions.
	 */
	SetValue = func (int progress)
	{
		this.GUI_Element_Progress = this.GUI_Element_Progress ?? {};
		this.GUI_Element_Progress.Right = ToPercentString(BoundBy(progress, 0, 1000));
		
		return this;
	},
	
	/*
		Makes the bar visible to its owner.
		
		@return proplist The bar layout proplist, for calling further functions.
	 */
	Show = func ()
	{
		this.Player = this.GUI_Owner;
		return this;
	},
	
	/*
		Makes the bar invisible to its owner.
		
		@return proplist The bar layout proplist, for calling further functions.
	 */
	Hide = func ()
	{
		this.Player = NO_OWNER;
		return this;
	},
	
	/*
		Updates the GUI with all changes to the layout that were made previously.
		
		Works only if the bar was added to a GUI with AddTo(...).
		
		@return proplist The bar layout proplist, for calling further functions.
	 */
	Update = func ()
	{
		if (this.GUI_ID && this.GUI_Bar_Name)
		{
			var update = {};
			update[this.GUI_Bar_Name] = this;
			GuiUpdate(update, this.GUI_ID, this.GUI_ID_Child, Object(this.GUI_TargetNr));
		}
		return this;
	},
};
