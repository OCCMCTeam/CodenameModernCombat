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
	ListEntry_Index_Selected = nil,


	AddEntry = func (identifier, proplist new_entry)
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
			entry = new_entry ?? this->~MakeEntryProplist();
			entry.Priority = entry_count;
			entry->~SetIndex(entry_count);
			entry->AddTo(this);

			PushBack(this.ListEntry_Ids, identifier);
			PushBack(this.ListEntry_Elements, entry);
		}
		entry->Update();
		
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
	
	GetSelectedEntry = func ()
	{
		if (this.ListEntry_Elements)
		{
			return this.ListEntry_Elements[this.ListEntry_Selected];
		}
	},
	
	SelectEntry = func (identifier, int index, bool skip_callback)
	{
		if (identifier)
		{
			index = GetIndexOf(this.ListEntry_Ids, identifier);
		}
		index = index ?? 0;

		if (index == -1)
		{
			FatalError("Entry not found");
		}
		
		this.ListEntry_Selected = index;
		
		var can_select = false;
		for (var i = 0; i < GetLength(this.ListEntry_Elements); ++i)
		{
			var should_select = i == index;
			var did_select = this.ListEntry_Elements[i]->SetSelected(should_select, skip_callback); // Return value of 'nil' means that the entry cannot be selected/deselected
			if (should_select && !!did_select)
			{
				can_select = true;
			}
		}
		return can_select;
	},
	
	SelectNextEntry = func(bool backward, bool skip_callback)
	{
		var index = this.ListEntry_Selected ?? 0;
		var max = GetLength(this.ListEntry_Elements);
		if (backward)
		{
			index -= 1;
		}
		else
		{
			index += 1;
		}
		while (index < 0)
		{
			index += max;
		}
		index = index % max;
		if (!SelectEntry(nil, index, skip_callback))
		{
			SelectNextEntry(backward, skip_callback);
		}
	},
};

/* --- Tab button --- */

static const GUI_List2_Entry = new GUI_Element
{
	// --- Properties

	ListEntry_Hovered = nil,
	ListEntry_Callback_OnClick = nil,
	ListEntry_Callback_OnMouseIn = nil,
	ListEntry_Callback_OnMouseOut = nil,
	ListEntry_Callback_OnMenuClosed = nil,
	ListEntry_Index = nil,
	
	// --- Functions

	SetCallbackOnClick = func (array callback)
	{
		this.ListEntry_Callback_OnClick = callback;
		this.OnClick = GuiAction_Call(this, GetFunctionName(this.OnClickCall));
		return this;
	},
	
	SetCallbackOnMouseIn = func (array callback)
	{
		this.ListEntry_Callback_OnMouseIn = callback;
		this.OnMouseIn = GuiAction_Call(this, GetFunctionName(this.OnMouseInCall));
		return this;
	},
	
	SetCallbackOnMouseOut = func (array callback)
	{
		this.ListEntry_Callback_OnMouseOut = callback;
		this.OnMouseOut = GuiAction_Call(this, GetFunctionName(this.OnMouseOutCall));
		return this;
	},
	
	SetCallbackOnMenuClosed = func (array callback) // Custom callback from certain menus, does not correspond with any of the usual GUI callbacks
	{
		this.ListEntry_Callback_OnMenuClosed = callback;
		return this;
	},
	
	GetIndex = func ()
	{
		return this.ListEntry_Index;
	},
	
	SetIndex = func (int index)
	{
		this.ListEntry_Index = index;
		return this;
	},
	
	OnClickCall = func ()
	{
		DoCallback(this.ListEntry_Callback_OnClick);
	},
	
	OnMouseInCall = func ()
	{
		this.ListEntry_Hovered = true;
		this->~UpdateEntry();
		DoCallback(this.ListEntry_Callback_OnMouseIn);
	},
	
	OnMouseOutCall = func ()
	{
		this.ListEntry_Hovered = false;
		this->~UpdateEntry();
		DoCallback(this.ListEntry_Callback_OnMouseOut);
	},
	
	OnMenuClosedCall = func ()
	{
		if (this.ListEntry_Callback_OnMenuClosed)
		{
			DoCallback(this.ListEntry_Callback_OnMenuClosed);
		}
	}
};