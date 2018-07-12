/**
	List menu prototype
	
	This list offers a default selection, so that entries can be selected
	and deselected.
	
	@author Marky
 */

static const GUI_SelectionList = new GUI_List
{	
	SelectItem = func (identifier, int index)
	{
		if (identifier)
		{
			index = GetIndexOf(this.ListEntry_Ids, identifier);
		}
		index = index ?? 0;

		if (index == -1)
		{
			FatalError("Tab not found");
		}
		
		for (var i = 0; i < GetLength(this.ListEntry_Elements); ++i)
		{
			this.ListEntry_Elements[i]->SetSelected(i == index);
		}
	},
};

/* --- List menu entry --- */

static const GUI_SelectionListEntry = new GUI_Element
{
	// --- Properties

	ListEntry_Selected = nil,
	ListEntry_Hovered = nil,
	/*
	ListEntry_Callback = nil,
	ListEntry_Index = nil,

	// --- GUI Properties
	
	hover = nil, // Overlay for hover effect
	caption = nil, // Overlay for text, should be over the hover effect

	// --- Functions
	
	Assemble = func (desired_width)
	{
		this.OnClick = GuiAction_Call(this, GetFunctionName(this.OnClickCall));
		this.OnMouseIn = GuiAction_Call(this, GetFunctionName(this.OnMouseInCall));
		this.OnMouseOut = GuiAction_Call(this, GetFunctionName(this.OnMouseOutCall));
		
		this.hover = { Priority = 1};
		this.caption = { Priority = 2, Style = GUI_TextHCenter | GUI_TextVCenter};
		return this;
	},
	
	SetData = func (string caption, array callback, proplist style)
	{
		if (caption)
		{
			this.caption.Text = caption;
		}
		if (style)
		{
			AddProperties(this, style);
		}
		this.ListEntry_Callback = callback;
		return this;
	},
	
	SetIndex = func (int index)
	{
		this.ListEntry_Index = index;
		return this;
	},
	
	OnMouseInCall = func ()
	{
		this.ListEntry_Hovered = true;
		this->~UpdateEntry();
	},
	
	OnMouseOutCall = func ()
	{
		this.ListEntry_Hovered = false;
		this->~UpdateEntry();
	},
	
	OnClickCall = func ()
	{
		GetParent()->SelectTab(nil, this.ListEntry_Index);
	},*/
	
	SetSelected = func (bool selected, bool skip_callback)
	{
		// Update the display
		this.ListEntry_Selected = selected;
		this->~UpdateEntry();
		
		// Issue a callback?
		if (this.ListEntry_Callback && selected && !skip_callback)
		{
			DoCallback(this.ListEntry_Callback);
		}
		return this;
	},
	
	IsHovered = func ()
	{
		return this.ListEntry_Hovered;
	},
	
	IsSelected = func ()
	{
		return this.ListEntry_Selected;
	},
};
