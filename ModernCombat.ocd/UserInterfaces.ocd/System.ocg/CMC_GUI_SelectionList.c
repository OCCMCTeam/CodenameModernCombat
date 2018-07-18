/**
	Vertical list menu with containing just menu icons.

	@author Marky
 */

static const CMC_GUI_SelectionListMenu = new GUI_Element
{
	Symbol = CMC_Icon_ListSelection,
	Style = GUI_NoCrop,
	Components = nil,

	Assemble = func ()
	{
		this.Components = [];
		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Width         // Width for text
		                            + GUI_CMC_Element_SelectionList_Margin_H * 2  // Outside margin left/right
		                            + GUI_CMC_Element_ListIcon_Size               // Space for the icon, on the left
		                            + GUI_CMC_Element_Icon_Size));                // Space for the button hint, on the right
		
		var header_height = GuiDimensionCmc(nil, GUI_CMC_Element_Default_Height);
		var header = new GUI_Element { Style = GUI_TextHCenter | GUI_TextVCenter,};
		header->SetHeight(header_height);
		header->AddTo(this);

		var body = new GUI_Element { ID = 123, Style = GUI_VerticalLayout | GUI_NoCrop};
		body->SetTop(header->GetBottom())
		    ->SetBottom(header->GetBottom())
		    ->AddTo(this);
		
		var list = new CMC_GUI_SelectionList {ID = 456, Style = GUI_VerticalLayout | GUI_FitChildren };
		list->~Assemble();
		list->AddTo(body);

		this.Components[0] = header;
		this.Components[1] = list;
		
		return this;
	},
	
	
	GetHeader = func()
	{
		return this.Components[0];
	},
	
	GetList = func ()
	{
		return this.Components[1];
	},
	
	AdjustHeightToEntries = func ()
	{
		var entries = GetLength(GetList().ListEntry_Elements);
		var amount = Max(7, entries);
		var top = (amount - entries) / 2;
		
		var size = GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size);
		// TODO: This works well, but the background looks bad: 
		SetHeight(GetList()->GetHeight()->Add(this.Components[0]->GetHeight()));
		// These served the purpose of enlarging the background image and then
		// centering the entries.
		//SetHeight(size->Scale(amount));
		/*GetList()//->SetHeight(size->Scale(entries))
		         ->AlignTop(GetHeader()->GetBottom())
		         ->Update();*/

		return this;
	},
	
	SetHeaderCaption = func (string text)
	{
		GetHeader().Text = Format("<c %x>%s</c>", GUI_CMC_Text_Color_HeaderCaption, text);
		GetHeader()->Update();
		return this;
	},
};


static const CMC_GUI_SelectionList = new GUI_List2
{
	MakeEntryProplist = func(symbol, text)
	{
		var custom_entry = new CMC_GUI_SelectionListEntry{};
		return custom_entry->Assemble()->SetIcon(symbol)->SetCaption(text);
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
	
	AddItemSeparator = func()
	{
		// in case of a new entry, append to array
		this.ListEntry_Data = this.ListEntry_Data ?? [];
		var ID = GetLength(this.ListEntry_Data) + 1;
		
		var margin_h = GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Margin_H);
		var custom_entry = new GUI_Element
		{
				Left = margin_h->ToString(),
				Right = GuiDimensionCmc(1000)->Subtract(margin_h)->ToString(),
				Bottom = GuiDimensionCmc(nil, 3)->ToString(),
				BackgroundColor = 0x81ffffff,
		};
		
		// Always add some properties later. This is done so that real custom this.ListEntry_Data do not need to care about target etc.
		custom_entry.ID = ID; // A fixed ID is obligatory for now. Might be possible to omit that, but would need to check if updating etc works.
		custom_entry.Target = this; // Same as above.

		// These properties can in theory be set/customized by the user without breaking functionality. But they are (probably) required anway.
		custom_entry.Priority = ID;
		
		// Save entry to list and prepare call information.
		this.ListEntry_Data[ID - 1] = [];
		this[Format("_menuChild%d", ID)] = custom_entry;
		return custom_entry;
	},
};

static const CMC_GUI_SelectionListEntry = new GUI_SelectionListEntry
{
	Assemble = func ()
	{	
		var margin_h = GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Margin_H);
		var size = GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size);
		var button_hint_size = GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size);
		var caption_border_right = GuiDimensionCmc(1000)->Subtract(margin_h)->Subtract(button_hint_size)->ToString();
		this.icon =
		{
			Left = margin_h->ToString(),
			Right = margin_h->Add(size)->ToString(),
			Bottom = size->ToString(),
			
			Margin = [GuiDimensionCmc(nil, 2)->ToString()],
		};
		this.caption =
		{
			Left = margin_h->Add(size)->ToString(),
			Right = caption_border_right,
			
			Style = GUI_TextVCenter,
		};
		SetHeight(size);
		return this;
	},
	
	SetIcon = func (symbol)
	{
		this.icon.Symbol = symbol;
		return this;
	},
	
	SetCaption = func (string text)
	{
		this.caption.Text = text;
		return this;
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
