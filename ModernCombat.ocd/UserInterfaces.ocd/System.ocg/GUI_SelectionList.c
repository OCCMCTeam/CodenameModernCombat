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

static const GUI_SelectionListEntry = new GUI_List2_Entry
{
	// --- Properties

	ListEntry_Selected = nil,

	// --- Functions
	
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
	
	IsSelected = func ()
	{
		return this.ListEntry_Selected;
	},
	
	UpdateEntry = func ()
	{
		if (IsSelected())
		{
			this.Symbol = CMC_Icon_ListSelectionHighlight;
		}
		else
		{
			this.Symbol = nil;
		}
		Update({Symbol = this.Symbol});
	},
};