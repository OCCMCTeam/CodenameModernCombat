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
	- Configure the bar foreground as you would with any GUI element, access layout.GUI_Element_Controller_Progress
	- Call layout->AddTo(...) after you opened a menu to add the layout the bar to that menu
	
	You can call various functions on this layout after you have created it;
	As a general rule you can change these values around as much as you like
	and they are applied to the menu only when you call layout->Update(). 
 */
static const CMC_GUI_ProgressBar = new GUI_Element
{	
	// --- GUI info
	
	/*
	 * The progress bar background, height, etc. can be defined via this proplist.
	 * The progress bar progress is defined via the GUI_Element_Controller_Progress.
	 */

	// The element for the progress bar overlay
	GUI_Element_Controller_Progress = nil,
	
	// --- Functions / API
	
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
			this.GUI_Element_Controller_Progress = this.GUI_Element_Controller_Progress ?? {};
			this.GUI_Element_Controller_Progress.BackgroundColor = color;
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
		this.GUI_Element_Controller_Progress = this.GUI_Element_Controller_Progress ?? {};
		this.GUI_Element_Controller_Progress.Right = ToPercentString(BoundBy(progress, 0, 1000));
		
		return this;
	},
};
