/**
	Simple proplist with functions for GUI elements.
	
	Provides a (in my opinion) more comfortable interface than the basic
	GUI functions.

	@author Marky
 */
 

/*
	GUI prototype for a generic element.
	
	You can call various functions on this layout after you have created it;
	As a general rule you can change these values around as much as you like
	and they are applied to the menu only when you call layout->Update(). 
 */
static const GUI_Element = new Global
{
	// --- Properties:
	GUI_ID = nil,       // int gui_id - for GuiUpdate()
	GUI_ID_Child = nil, // int child_id - for GuiUpdate()
	GUI_TargetNr = nil, // object target - for GuiUpdate(); Contains only the number to avoid infinite proplist (the object may contain the menu that contains this layout that saves this object as its target)
	GUI_Element_Name = nil, // the name of the bar - the property name of the sub window in the menu
	GUI_Owner = nil,    // the owner for the bar - for visibility
	
	// --- Functions / API
	
	/*
		Adds the GUI element to a menu. Should be called after the menu definition.
		
		@par menu The GUI menu layout proplist.
		@par submenu_name The property will be added with this name,
		                  or an existing property will be extended
		                  to work with these functions.
		@par gui_id For GuiUpdate()
		@par child_id For GuiUpdate()
		@par target For GuiUpdate(); Required if you want to set visibility of the bar
	 */
	AddTo = func (proplist menu, string submenu_name, int gui_id, int child_id, object target)
	{
		if (GetType(menu) != C4V_PropList)
		{
			FatalError("GUI element can be added to a %v only, you passed %v", C4V_PropList, GetType(menu));
		}
		
		// Copy existing properties to the element
		if (menu[submenu_name])
		{
			AddProperties(this, menu[submenu_name]);
		}
		
		menu[submenu_name] = this;
	
		this.GUI_Element_Name = submenu_name;
		this.GUI_ID = gui_id;
		this.GUI_TargetNr = target->ObjectNumber();
		this.GUI_ID_Child = child_id;
		this.GUI_Owner = target->GetOwner();
		return this;
	},
	
	/*
		Gets the name of the bar in the GUI layout proplist.
		
		@return string The name.
	 */
	GetName = func ()
	{
		return this.GUI_Element_Name;
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
		if (this.GUI_ID && this.GUI_Element_Name)
		{
			var update = {};
			update[this.GUI_Element_Name] = this;
			GuiUpdate(update, this.GUI_ID, this.GUI_ID_Child, Object(this.GUI_TargetNr));
		}
		return this;
	},
};
