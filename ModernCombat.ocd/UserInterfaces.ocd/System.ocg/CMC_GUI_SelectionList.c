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
		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Width         // Width for text
		                            + GUI_CMC_Element_SelectionList_Margin_H * 2  // Outside margin left/right
		                            + GUI_CMC_Element_ListIcon_Size               // Space for the icon, on the left
		                            + GUI_CMC_Element_Icon_Size));                // Space for the button hint, on the right
		
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
		this.header.Text = Format("<c %x>%s</c>", GUI_CMC_Text_Color_HeaderCaption, text);
		this.header->Update();
		return this;
	},
};


static const CMC_GUI_SelectionList = new GUI_List
{
	MakeEntryProplist = func(symbol, text)
	{
		var margin_h = GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Margin_H);
		var size = GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size);
		var button_hint_size = GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size);
		var caption_border_right = GuiDimensionCmc(1000)->Subtract(margin_h)->Subtract(button_hint_size)->ToString();

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
				Left = margin_h->ToString(),
				Right = margin_h->Add(size)->ToString(),
				Bottom = size->ToString(),
				
				Symbol = symbol,
				Margin = [GuiDimensionCmc(nil, 2)->ToString()],
			},
			caption =
			{
				Left = margin_h->Add(size)->ToString(),
				Right = caption_border_right,
				
				Text = text,
				Style = GUI_TextVCenter,
			},
		};
		return custom_entry;
	},
	
	AddButtonPrompt = func (proplist custom_entry)
	{
		if (1 <= custom_entry.ID && custom_entry.ID <= 10)
		{
			var margin_h = GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Margin_H);
			var button_hint_width = GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size);
			var caption_border_right = GuiDimensionCmc(1000)->Subtract(margin_h)->Subtract(button_hint_width);

			var button_hint = new CMC_GUI_ButtonHint{};
			button_hint->Assemble(nil, Format("%d", custom_entry.ID % 10));
			button_hint->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size));
			button_hint->AlignLeft(caption_border_right);
			button_hint->AddTo(custom_entry);
			button_hint->GetButtonIcon()->AlignCenterV();
		}
	},
};
