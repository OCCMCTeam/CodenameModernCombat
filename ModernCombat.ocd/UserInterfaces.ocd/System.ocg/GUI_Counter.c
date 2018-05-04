/**
	Simple proplist with functions for counters.

	@author Marky
 */
 

/*
	GUI prototype for a counter.
	
	Usage:
	- Create a counter by var counter = new GUI_Counter{};
	- Call layout->AddTo(...) to add the layout the bar to a menu
	
	You can call various functions on this layout after you have created it;
	As a general rule you can change these values around as much as you like
	and they are applied to the menu only when you call layout->Update(). 
 */
static const GUI_Counter = new GUI_Element
{	
	// --- Properties
	
	GUI_Counter_DigitAmount = 1,	      // The counter shows this many digits
	GUI_Counter_No_Trailing_Zeros = true, // Hide trailing zeros?
	
	// --- Functions / API
	
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
	
	// SetDigit* function: Sets the size of the GUI element.
	//
	// Manipulating a GUI element this way will always
	// change the right and/or bottom border, while
	// the left and/or top border stay fixed.
	//
	// Sets the dimensions as usually, but multiplies it by the supplied digit width
	
	SetDigitWidth = func (dimension, int em)
	{
		SetWidth(Dimension(dimension, em)->Scale(this.GUI_Counter_DigitAmount ?? 1));
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
		Defines whether you want to show trailing zeros.
		
		@par show If this is set to 'true' those digits that are not present
		          in the actual value will display '0' instead.
		          Set it to 'false' to hide traling zeros and display only
		          the number.
		@return proplist The counter proplist, for calling further functions.
	 */
	ShowTrailingZeros = func (bool show)
	{
		this.GUI_Counter_No_Trailing_Zeros = !show;
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
		if (!GetLength(digits))
		{
			digits[0] = 0;
		}
		return digits;
	},

	/*
		Internal function:
		Sets the counter digits
	 */
	SetDigits = func (array digits)
	{
		var graphics_names = [];
		for (var i = 0; i < this.GUI_Counter_DigitAmount; ++i)
		{
			var digit = digits[i];
			if (digit == nil && this.GUI_Counter_No_Trailing_Zeros)
			{
				graphics_names[i] = nil;
			}
			else
			{
				graphics_names[i] = Format("%d", digit);
			}
		}
		SetCounterElementProperties("GraphicsName", graphics_names, true);
	},
	
	SetDigitProperties = func (proplist properties)
	{
		for (var property_name in GetProperties(properties))
		{
			SetCounterElementProperties(property_name, properties[property_name]);
		}
		return this;
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
	SetCounterElementProperties = func (string property_name, value, bool in_order)
	{
		for (var i = 0; i < this.GUI_Counter_DigitAmount; ++i)
		{
			// Set the values in order, or copy the value to every counter?
			if (GetType(value) == C4V_Array && in_order)
			{
				SetCounterElementProperty(i, property_name, value[i]);
			}
			else
			{
				SetCounterElementProperty(i, property_name, value);
			}
		}
	},
	
	SetCounterElementProperty = func (int index, string property_name, value)
	{
		var counter_name = Format("digit_%d", GetOrder(index));
		
		if (!this[counter_name])
		{
			this[counter_name] = {};
		}
		this[counter_name][property_name] = value;
	},
	
	// Translates the integer position information to GUI layout properties
	ComposeLayout = func ()
	{
		// Copied from parent prototype, no idea how to inherit things here
		this.Left = GetLeft()->ToString();
		this.Right = GetRight()->ToString();
		this.Top = GetTop()->ToString();
		this.Bottom = GetBottom()->ToString();
		
		// Additional code below
		var remainder = 1000 % this.GUI_Counter_DigitAmount;
		var width = (1000 - remainder) / this.GUI_Counter_DigitAmount;
		
		var left = [], right = [];
		for (var i = 0; i < this.GUI_Counter_DigitAmount; ++i)
		{
			SetCounterElementProperty(i, "Left", ToPercentString(1000 - width * (i + 1)));
			SetCounterElementProperty(i, "Right", ToPercentString(1000 - width * i));
		}
	},
};
