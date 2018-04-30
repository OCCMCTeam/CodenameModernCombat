/**
	Simple proplist with functions for GUI elements.
	
	Provides a (in my opinion) more comfortable interface than the basic
	GUI functions.

	@author Marky
 */


/*
	Measurement prototype for GUI elements.
	
	Contains percent and em unit components
	of a GUI position, accessible as integers.
	
	Background:
	The GUI interface saves positions as strings.
	This is sufficient for simple GUIs that you program
	manually.
	However, if you want to align something in a comfortable
	way, you have to be very careful as to how you specify your
	positions, because you can add positions as much as you
	like, but you cannot subtract properly:
	
	Shifting an existing "1.0em" to the right by "0.5em" results in "1.0em +0.5em".
	Shifting it back by "-0.1em" results in "1.0em +0.5em -0.1em";
	As you can see, the string gets longer and longer, and subtracting that position
	from some value is impossible (so you can never really calculate the width of an element,
	for example), because
	"-%s" will yield "-1.0em +0.5em -0.1em" for that position. 
 */
static const GUI_Dimension = new Global
{
	percent = nil,
	em = nil,
	
	// --- Functions
	
	/*
		Gets the percent component of the dimension.
	 */
	GetPercent = func ()
	{
		return this.percent;
	},

	/*
		Gets the em component of the dimension.
	 */	
	GetEm = func ()
	{
		return this.em;
	},
	
	/*
		Sets the percent commponent of the dimension.
		
		@par value The percent component.
		@return proplist The dimension itself for
		                 further function calls.
	 */
	SetPercent = func (int value)
	{
		this.percent = value;
		return this;
	},
	
	
	/*
		Sets the em component of the dimension.
		
		@par value The percent component.
		@return proplist The dimension itself for
		                 further function calls.
	 */
	SetEm = func (int value)
	{
		this.em = value;
		return this;
	},
	
	
	/*
		Adds to the percent commponent of the dimension.
		
		@par change The percent value to add.
		@return proplist The dimension itself for
		                 further function calls.
	 */
	AddPercent = func (int change)
	{
		SetPercent(GetPercent() + change);
		return this;
	},
	
	
	/*
		Adds to the em commponent of the dimension.
		
		@par change The em value to add.
		@return proplist The dimension itself for
		                 further function calls.
	 */
	AddEm = func (int change)
	{
		SetEm(GetEm() + change);
		return this;
	},
	
	CopyOf = func (proplist other)
	{
		if (IsValidPrototype(other))
		{
			return new GUI_Dimension{}
			       ->SetPercent(other->GetPercent())
			       ->SetEm(other->GetEm());
		}
		else
		{
			FatalError("Cannot copy proplist of prototype %s", GetPrototype(other));
		}
	},
	
	// --- Calculation functions 
	//
	// These will return a NEW dimension
	
	Add = func (proplist other)
	{
		if (IsValidPrototype(other))
		{
			return new GUI_Dimension{}->CopyOf(this)
			       ->AddPercent(+other->GetPercent())
			       ->AddEm(+other->GetEm());
		}
		else
		{
			FatalError("Cannot subtract proplist of prototype %s", GetPrototype(other));
		}
	},
	
	Subtract = func (proplist other)
	{
		if (IsValidPrototype(other))
		{
			return new GUI_Dimension{}->CopyOf(this)
			       ->AddPercent(-other->GetPercent())
			       ->AddEm(-other->GetEm());
		}
		else
		{
			FatalError("Cannot subtract proplist of prototype %s", GetPrototype(other));
		}
	},
	
	// --- Internal functions
	
	IsValidPrototype = func (proplist other)
	{
		return GetPrototype(other) == GetPrototype(this);
	},
	
	ToString = func ()
	{
		if (GetPercent() == nil && GetEm() == nil)
		{
			return nil; // Use default value!
		}
		else
		{
			// This looks a little complicated, but it is the safest way to get a correct string
			var p = "", e = "";
			if (GetPercent() != nil) p = ToPercentString(GetPercent());
			if (GetEm() != nil) e = ToPercentString(GetEm());
			return Format("%s%s", p, e);
		}
	},
};


/*
	GUI prototype for a generic element.
	
	You can call various functions on this layout after you have created it;
	As a general rule you can change these values around as much as you like
	and they are applied to the menu only when you call layout->Update(). 
	
	Note:
	Positions in the original GUI are 
 */
static const GUI_Element = new Global
{
	// --- Properties:
	GUI_ID = nil,           // int gui_id - for GuiUpdate()
	GUI_ID_Child = nil,     // int child_id - for GuiUpdate()
	GUI_TargetNr = nil,     // object target - for GuiUpdate(); Contains only the number to avoid infinite proplist
	                        // (the object may contain the menu that contains this layout that saves this object as its target)
	                        // also, if the object is saved directly it is proplist, too, and therefore the GUI engine would
	                        // try to display it as a subwindow
	                        
	GUI_Element_Name = nil, // the name of the element - the property name of the sub window in the menu
	GUI_Parent_Name = nil,  // the name of the parent element; if this name is nil then it is implied that it is the root menu
	GUI_Owner = nil,        // the owner for the bar - for visibility
	
	GUI_Element_Position =   // Contains the positions in percent and string; Is an array, because if it were a proplist it would count as a subwindow
	[
		new GUI_Dimension{}, // Left
		new GUI_Dimension{}, // Top
		new GUI_Dimension{}, // Right
		new GUI_Dimension{}  // Bottom
	],

	// --- Generic Functions
	
	Open = func ()
	{
		if (this.GUI_ID == nil)
		{
			ComposeLayout();
			this.GUI_ID = GuiOpen(this);
		}
		return this;
	},
	
	Close = func()
	{
		if (this.GUI_ID)
		{
			GuiClose(this.GUI_ID, this.GUI_Child_ID);
		}
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
		ComposeLayout();
		if (this.GUI_Element_Name && (this.GUI_ID || this.GUI_ID_Child))
		{
			var update = {};
			update[this.GUI_Element_Name] = this;
			var result = GuiUpdate(update, this.GUI_ID, this.GUI_ID_Child);
		}
		else if (this.GUI_ID)
		{
			var result = GuiUpdate(this, this.GUI_ID, this.GUI_ID_Child);
		}
		
		return this;
	},
	
	// --- Positioning functions
	
	// Set* functions: These leave the other values as they are,
	//                 so that the dimensions of the element can be changed
	
	SetLeft = func (proplist value)
	{
		this.GUI_Element_Position[0] = value;
		return this;
	},
	
	SetRight = func (proplist value)
	{
		this.GUI_Element_Position[2] = value;
		return this;
	},
	
	SetTop = func (proplist value)
	{
		this.GUI_Element_Position[1] = value;
		return this;
	},
	
	SetBottom = func (proplist value)
	{
		this.GUI_Element_Position[3] = value;
		return this;
	},
	
	// Align* functions: These leave the dimensions of the GUI element fixed,
	//                   but move its position so that it matches the target
	//                   position
	
	// Shift* functions: These leave the dimensions of the GUI element fixed,
	//                   but move its position by a specified amount.
	
	// Get* function: Get borders of the GUI, as a dimension.
	
	GetLeft = func ()
	{
		return this.GUI_Element_Position[0];
	},
	
	GetRight = func ()
	{
		return this.GUI_Element_Position[2];
	},
	
	GetTop = func ()
	{
		return this.GUI_Element_Position[1];
	},
	
	GetBottom = func ()
	{
		return this.GUI_Element_Position[3];
	},
	
	// --- Dimension related functions
	
	// Set* function: Sets the size of the GUI element.
	//
	//                Manipulating a GUI element this way will always
	//                change the right and/or bottom border, while
	//                the left and/or top border stay fixed.
	
	
	
	// Get* function: Get size of the GUI element, as a dimension.
	
	SetHeight = func (proplist dimension)
	{
		SetBottom(GetTop()->Add(dimension));
		return this;
	},
	
	SetWidth = func (proplist dimension)
	{
		SetRight(GetLeft()->Add(dimension));
		return this;
	},

	GetHeight = func ()
	{
		return GetBottom()->Subtract(GetTop());
	},
	
	GetWidth = func ()
	{
		return GetRight()->Subtract(GetLeft());
	},
	
	// --- Internals
	
	// Translates the integer position information to GUI layout properties
	ComposeLayout = func ()
	{
		this.Left = GetLeft()->ToString();
		this.Right = GetRight()->ToString();
		this.Top = GetTop()->ToString();
		this.Bottom = GetBottom()->ToString();
	},
};
