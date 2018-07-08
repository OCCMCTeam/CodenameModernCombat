/**
	Vertical list menu with containing just menu icons.

	@author Marky
 */

static const CMC_GUI_SelectionListMenu = new GUI_Element
{
	Symbol = CMC_Icon_ListSelection,
	Style = GUI_NoCrop,

	Assemble = func ()
	{
		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Width));
		
		var header_height = GuiDimensionCmc(nil, GUI_CMC_Element_Default_Height);
		var header = new GUI_Element {Style = GUI_TextHCenter | GUI_TextVCenter,};
		header->SetHeight(header_height);
		header->AddTo(this, nil, "header", true);

		var body = new GUI_Element { Style = GUI_VerticalLayout | GUI_FitChildren | GUI_TextHCenter | GUI_TextVCenter, };
		body->SetTop(header->GetBottom())
		    ->AddTo(this, nil, "body", true);
		
		var list = new CMC_GUI_SelectionList {};
		list->AddTo(body, nil, "list", true);
		return this;
	},
	
	GetList = func ()
	{
		return this.body.list;
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
	
	SetHeaderCaption = func (string text)
	{
		this.header.Text = text;
		this.header->Update();
		return this;
	},
};


static const CMC_GUI_SelectionList = new GUI_List
{
	MakeEntryProplist = func(symbol, text)
	{
		var size = GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size);
		var caption_border_right = GuiDimensionCmc(1000)->Subtract(size->Scale(3))->ToString();
		var icon_border_right = GuiDimensionCmc(1000)->Subtract(size->Scale(2))->ToString();
		var custom_entry = new GUI_Element
		{
			Bottom = size->ToString(),
			
			Symbol =
			{
				Std = nil,
				OnHover = CMC_Icon_ListSelectionHighlight,
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
				Right = caption_border_right,
				
				Text = text,
				Style = GUI_TextVCenter,
			},
			button_prompt = 
			{
				Left = caption_border_right,
				Right = icon_border_right,
				Bottom = size->ToString(),
			},
		};
		return custom_entry;
	},
	
	AddButtonPrompt = func (proplist custom_entry)
	{
		if (1 <= custom_entry.ID && custom_entry.ID <= 10)
		{
			custom_entry.button_prompt.Symbol = CMC_Icon_Button;
			custom_entry.button_prompt.GraphicsName = Format("%d", custom_entry.ID % 10);
		}
	},
};
