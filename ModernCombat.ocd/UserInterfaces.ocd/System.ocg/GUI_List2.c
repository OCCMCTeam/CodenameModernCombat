/**
	List menu prototype
	
	@author Marky
 */

static const GUI_List2 = new GUI_Element
{
	Style = GUI_VerticalLayout,

	// Elements for pseudo-proplist
	// Adding a real proplist would add the elements as a submenu ()
	ListEntry_Ids = nil,
	ListEntry_Elements = nil,
	ListEntry_Width = nil,


	AddEntry = func (identifier, string caption, array callback, proplist style)
	{
		// Establish defaults
		this.ListEntry_Ids = this.ListEntry_Ids ?? [];
		this.ListEntry_Elements = this.ListEntry_Elements ?? [];
		this.ListEntry_Width = this.ListEntry_Width ?? GuiDimensionCmc();
		
		var entry;
		var index = GetIndexOf(this.ListEntry_Ids, identifier);
		if (index >= 0)
		{
			entry = this.ListEntry_Elements[index];
		}
		else
		{
			var entry_count = GetLength(this.ListEntry_Elements);
			
			// Add additional entry
			entry = this->~MakeEntryProplist(identifier, caption) ?? new GUI_List2_Entry {}; // TODO <= passing identifier is too restrictive
			entry.Priority = entry_count + 1;
			entry.ListEntry_Index = entry_count;
			entry->~Assemble(); // TODO <= this is not really cool because it overwrites stuff
			entry->~SetIndex(entry_count);
			entry->AddTo(this);

			PushBack(this.ListEntry_Ids, identifier);
			PushBack(this.ListEntry_Elements, entry);
		}
		entry->SetData(caption, callback, style)->Update();
		
		// Update the list height
		var height = nil;
		for (var element in this.ListEntry_Elements)
		{
			if (height == nil)
			{
				height = element->GetHeight();
			}
			else
			{
				height = height->Add(element->GetHeight());
			}
		}
		SetHeight(height);
		Update(ComposeLayout());
		
		// Done
		return entry;
	},
	
	SelectEntry = func (identifier, int index)
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

/* --- Tab button --- */

static const GUI_List2_Entry = new GUI_Element
{
	Style = GUI_Multiple,

	// --- Properties

	ListEntry_Selected = nil,
	ListEntry_Hovered = nil,
	ListEntry_Callback = nil,
	ListEntry_Index = nil,

	// --- GUI Properties

	BackgroundColor = GUI_CMC_Background_Color_Default,
	
	// --- Functions
	
	Assemble = func (desired_width)
	{
		this.OnClick = GuiAction_Call(this, GetFunctionName(this.OnClickCall));
		this.OnMouseIn = GuiAction_Call(this, GetFunctionName(this.OnMouseInCall));
		this.OnMouseOut = GuiAction_Call(this, GetFunctionName(this.OnMouseOutCall));
		return this;
	},
	
	SetData = func (string caption, array callback, proplist style)
	{
		return this;
	},
	
	SetIndex = func (int index)
	{
		this.ListEntry_Index = index;
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
		GetParent()->SelectEntry(nil, this.ListEntry_Index);
	},
	
	SetSelected = func (bool selected, bool skip_callback)
	{
		// Update the display
		this.ListEntry_Selected = selected;
		UpdateBackground();
		
		// Issue a callback?
		if (this.ListEntry_Callback && selected && !skip_callback)
		{
			DoCallback(this.ListEntry_Callback);
		}
		return this;
	},
	
	IsSelected = func ()
	{
		return this.ListEntry_Selected;
	},
	
	UpdateBackground = func (int color)
	{
		if (color == nil)
		{
			if (this.ListEntry_Selected)
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