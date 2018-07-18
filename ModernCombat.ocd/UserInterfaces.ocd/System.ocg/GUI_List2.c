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
	
	SelectEntry = func (identifier, int index)
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
		
		for (var i = 0; i < GetLength(this.ListEntry_Elements); ++i)
		{
			this.ListEntry_Elements[i]->SetSelected(i == index);
		}
	},
};

/* --- Tab button --- */

static const GUI_List2_Entry = new GUI_Element
{
	// --- Properties

	ListEntry_Hovered = nil,
	ListEntry_Callback = nil,
	ListEntry_Index = nil,
	
	// --- Functions
	
	Assemble = func ()
	{
		this.OnClick = GuiAction_Call(this, GetFunctionName(this.OnClickCall));
		this.OnMouseIn = GuiAction_Call(this, GetFunctionName(this.OnMouseInCall));
		this.OnMouseOut = GuiAction_Call(this, GetFunctionName(this.OnMouseOutCall));
		return this;
	},
	
	SetOnClickCall = func (array callback)
	{
		this.ListEntry_Callback = callback;
		return this;
	},
	
	SetIndex = func (int index)
	{
		this.ListEntry_Index = index;
		return this;
	},
	
	OnClickCall = func ()
	{
		DoCallback(this.ListEntry_Callback);
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
};