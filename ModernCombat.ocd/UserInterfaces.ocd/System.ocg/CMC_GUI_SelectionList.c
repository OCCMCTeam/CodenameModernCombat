/**
	Vertical list menu with containing just menu icons.

	@author Marky
 */

static const CMC_GUI_SelectionListMenu = new GUI_Element
{
	BackgroundColor = GUI_CMC_Background_Color_Default,
	Style = GUI_NoCrop,

	Assemble = func ()
	{
		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Width));
		var list = new CMC_GUI_SelectionList {};
		list->AddTo(this, nil, "list", true);
		return this;
	},
	
	GetList = func ()
	{
		return this.list;
	},
	
	AdjustHeightToEntries = func ()
	{
		var entries = GetLength(GetList().entries);
		var amount = Max(7, entries);
		var top = (amount - entries) / 2;
		
		var size = GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size);
		SetHeight(size->Scale(amount));
		GetList()->SetHeight(size->Scale(entries))
		         ->AlignTop(size->Scale(top))
		         ->Update(); 
		
		return this;
	},
};


static const CMC_GUI_SelectionList = new GUI_List
{
	MakeEntryProplist = func(symbol, text)
	{
		var size = GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size);
		var custom_entry = new GUI_Element
		{
			Bottom = size->ToString(),
			
			BackgroundColor =
			{
				Std = 0,
				OnHover = GUI_CMC_Text_Color_Highlight,
			},
			
			icon =
			{
				Left = size->Scale(2)->ToString(),
				Right = size->Scale(3)->ToString(),
				Bottom = size->ToString(),
				
				Symbol = symbol,
				Margin = [GuiDimensionCmc(nil, 2)->ToString()],
			},
			caption =
			{
				Left = size->Scale(3)->ToString(),
				
				Text = text,
				Style = GUI_TextVCenter,
			}
		};
		return custom_entry;
	},
};
