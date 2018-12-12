/**
	Default button for tabs and/or generic menu controls.
	
	@author Marky
 */


static const CMC_GUI_Button = new GUI_Element
{
	// --- Properties

	Tab_Selected = nil,
	Tab_Hovered = nil,
	Tab_Callback = nil,
	Tab_Index = nil,
	Tab_Enabled = true,

	// --- GUI Properties

	BackgroundColor = GUI_CMC_Background_Color_Default,
	
	hover = nil, // Overlay for hover effect
	label = nil, // Overlay for text, should be over the hover effect
	
	// --- Functions
	
	Assemble = func ()
	{
		this.OnClick = GuiAction_Call(this, GetFunctionName(this.OnClickCall));
		this.OnMouseIn = GuiAction_Call(this, GetFunctionName(this.OnMouseInCall));
		this.OnMouseOut = GuiAction_Call(this, GetFunctionName(this.OnMouseOutCall));
		
		this.hover = { Priority = 1};
		this.label = { Priority = 2, Style = GUI_TextHCenter | GUI_TextVCenter};
		
		return this;
	},
	
	SetData = func (string caption, array callback, proplist style)
	{
		if (caption)
		{
			this.label.Text = caption;
		}
		if (style)
		{
			AddProperties(this, style);
		}
		this.Tab_Callback = callback;
		return this;
	},
	
	SetIndex = func (int index)
	{
		this.Tab_Index = index;
		return this;
	},
	
	OnMouseInCall = func ()
	{
		Update({ hover = {BackgroundColor = GUI_CMC_Background_Color_Hover}});
	},
	
	OnMouseOutCall = func ()
	{
		Update({ hover = {BackgroundColor = nil}});
	},
	
	OnClickCall = func ()
	{
		if (this.Tab_Enabled)
		{
			if (IsTabButton())
			{
				GetParent()->~SelectTab(nil, this.Tab_Index);
			}
			else
			{
				GuiPlaySoundConfirm(this.GUI_Owner);
				DoCallback(this.Tab_Callback);
			}
		}
	},
	
	SetEnabled = func (bool enabled)
	{
		this.Tab_Enabled = enabled;
		
		// Deselect disabled buttons
		if (!enabled && IsSelected())
		{
			SetSelected(false, true); // Also updates the background color
		}
		else
		{
			UpdateBackground();
		}
		return this;
	},
	
	SetSelected = func (bool selected, bool skip_callback)
	{
		if (this.Tab_Enabled)
		{
			// Update the display
			this.Tab_Selected = selected;
			UpdateBackground();
			
			// Issue a callback?
			if (IsTabButton() && this.Tab_Callback && selected && !skip_callback)
			{
				GuiPlaySoundSelect(this.GUI_Owner);
				DoCallback(this.Tab_Callback);
			}
		}
		return this;
	},
	
	IsSelected = func ()
	{
		return this.Tab_Selected;
	},
	
	IsEnabled = func ()
	{
		return this.Tab_Enabled;
	},
	
	IsTabButton = func ()
	{
		return nil != this.Tab_Index;
	},
	
	UpdateBackground = func (int color)
	{
		if (color == nil)
		{
			if (!IsEnabled())
			{
				UpdateBackground(GUI_CMC_Background_Color_Invalid);
			}
			else if (IsSelected())
			{
				UpdateBackground(GUI_CMC_Background_Color_Highlight);
			}
			else
			{
				UpdateBackground(GUI_CMC_Background_Color_Default);
			}
		}
		else
		{
			this.BackgroundColor = color;
			Update({BackgroundColor = color});
		}
	},
};
