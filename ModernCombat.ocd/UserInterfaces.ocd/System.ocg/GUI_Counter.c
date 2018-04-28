/**
	Simple proplist with functions for counters.

	@author Marky
 */
 

/*
	GUI prototype for a counter.
	
	The counter is not a GUI element by itself. It references another GUI element
	that contains sub-elements of the name(s):
		<name>_digit_1,
		<name>_digit_10,
		<name>_digit_100, etc.
	where <name> is the name that you gave the counter. 
	
	Usage:
	- Create a layout that contains the <name>_digit_X elements as described above
	- Create a counter by var counter = new GUI_Counter{};
	- Call layout->AddTo(...) after you opened a menu to add the layout the bar to that menu
	- Call counter->SetReference(<parent_element>) to tell the counter which parent elements contains the <name>_digit_X elements
	
	You can call various functions on this layout after you have created it;
	As a general rule you can change these values around as much as you like
	and they are applied to the menu only when you call layout->Update(). 
 */
static const GUI_Counter = new GUI_Element
{	
	// --- Properties
	
	GUI_Counter_DigitAmount = 1,	  // The counter shows this many digits
	GUI_Counter_Submenu = nil,        // Reference to the menu that will be updated - this is needed only so that the correct digits will be updated
	GUI_Counter_Update = nil,         // Empty update info
	
	// --- Functions / API
	
	/*
		Sets the reference to a perent element that contains counter elements.
		This is mainly used for checking the display. You can display
		a digit only if the corresponding counter element exists in this parent element.
		
		@par submenu This menu contains counter elements that fulfill the following requirements:
		             - The counter elements follow the naming scheme "<name>_digit_x",
		               + where <name> is the name that you specified in the AddTo(..) call,
		               + where  x is a number from 10^(i - 1) with i = 1 ... N,
		                 and N is the amount of digits that you want to display.
		             - The counter elements have "Symbol" property that can display a number:
		               + the default graphics of that definition is empty
		               + the definition has graphics "0", ... , "9" that correspond to those numbers.
		
		@return proplist The counter proplist, for calling further functions.
	 */
	SetReference = func (proplist submenu)
	{
		this.GUI_Counter_Submenu = submenu;
		return this;
	},
	
	/*
		Defines the maximum amount of digits that the counter
		can display.
		
		If you want to display a number with more digits than
		this amount the counter will simply display "9" on
		every digit.
		
		@return proplist The counter proplist, for calling further functions.
	 */
	SetMaxDigits = func (int amount)
	{
		this.GUI_Counter_DigitAmount = amount;
		return this;
	},

	/*
		Sets the value that the counter displays.
		
		@par value The value.
		If you want to display a number with more digits than
		the counter can display it will simply display "9" on
		every digit.
		
		@return proplist The counter proplist, for calling further functions.
	 */
	SetValue = func (int value)
	{
		var digits = GetDigits(value);
		
		// Fill everything with 9s if the number is too long.s
		if (GetLength(digits) > this.GUI_Counter_DigitAmount)
		{
			digits = [];
			for (var i = 0; i < this.GUI_Counter_DigitAmount; ++i)
			{
				digits[i] = 9;
			}
		}
		
		SetDigits(digits);
		return this;
	},
	
	/*
		Makes the bar visible to its owner.
		
		@return proplist The bar layout proplist, for calling further functions.
	 */
	Show = func ()
	{
		SetCounterElementProperty("Player", this.GUI_Owner);
		return this;
	},
	
	/*
		Makes the bar invisible to its owner.
		
		@return proplist The bar layout proplist, for calling further functions.
	 */
	Hide = func ()
	{
		SetCounterElementProperty("Player", NO_OWNER);
		return this;
	},
	
	/*
		Updates the GUI with the counter number changes only.
		
		@return proplist The bar layout proplist, for calling further functions.
	 */
	Update = func ()
	{
		if (this.GUI_ID && this.GUI_Element_Name)
		{
			GuiUpdate(this.GUI_Counter_Update, this.GUI_ID, this.GUI_ID_Child /*, Object(this.GUI_TargetNr) - this seems to actually block the update if there is a child_id*/);
			this.GUI_Counter_Update = {};
		}
		return this;
	},
	
	// --- Internal Functions
	
	/*
		Internal function:
		Gets the digits of a value in an array (in reverse order).
	 */
	GetDigits = func (int value)
	{
		var digits = [];
		var exponent = 0;
		while (true)
		{
			var v = value / GetOrder(exponent);
			if (v == 0) // This makes use of the engine behavior that e.g. 9 / 10 = 0
			{
				break;
			}
			else
			{
				var digit = v % 10;
				digits[exponent] = digit;
			}
			++exponent;
		} 
		return digits;
	},

	/*
		Internal function:
		Sets the counter digits
	 */
	SetDigits = func (array digits)
	{
		this.GUI_Counter_Update = this.GUI_Counter_Update ?? {};
		var graphics_names = [];
		for (var i = 0; i < this.GUI_Counter_DigitAmount; ++i)
		{
			var digit = digits[i] ?? 0;
			graphics_names[i] = Format("%d", digit);
		}
		SetCounterElementProperty("GraphicsName", graphics_names, true);
	},
	
	/*
		Internal function:
		Gets the "order" of a digit: 10^exponent
	 */
	GetOrder = func (int exponent)
	{
		return 10 ** exponent;
	},
	
	/*
		Sets a property in the counter element.
		Update is displayed once the update function is called
	 */
	SetCounterElementProperty = func (string property_name, value, bool in_order)
	{
		for (var i = 0; i < this.GUI_Counter_DigitAmount; ++i)
		{
			var counter_name = Format("%s_digit_%d", this->GetName(), GetOrder(i));
			var property = this.GUI_Counter_Submenu[counter_name];
			if (property)
			{
				if (!this.GUI_Counter_Update[counter_name])
				{
					this.GUI_Counter_Update[counter_name] = {};
				}
				
				// Set the values in order, or copy the value to every counter?
				if (GetType(value) == C4V_Array && in_order)
				{
					this.GUI_Counter_Update[counter_name][property_name] = value[i];
				}
				else
				{
					this.GUI_Counter_Update[counter_name][property_name] = value;
				}
			}
		}
	}
};
