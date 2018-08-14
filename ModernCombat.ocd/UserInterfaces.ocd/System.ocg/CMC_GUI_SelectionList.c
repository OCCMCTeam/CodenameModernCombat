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
		this.Settings = {
			ClickAfterHotkey = false,
			ClickAfterScroll = false,
		};
		this.Components = [];
		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Width         // Width for text
		                            + GUI_CMC_Element_SelectionList_Margin_H * 2  // Outside margin left/right
		                            + GUI_CMC_Element_ListIcon_Size               // Space for the icon, on the left
		                            + GUI_CMC_Element_Icon_Size));                // Space for the button hint, on the right
		
		var header_height = GuiDimensionCmc(nil, GUI_CMC_Element_Default_Height);
		var header = new GUI_Element { Style = GUI_TextHCenter | GUI_TextVCenter,};
		header->SetHeight(header_height);
		header->AddTo(this);

		var body = new GUI_Element {Style = GUI_NoCrop};
		body->SetTop(header->GetBottom())
		    ->AddTo(this);
		
		var list = new CMC_GUI_SelectionList {Style = GUI_VerticalLayout };
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
	
	MakeSeparatorProplist = func()
	{
		var separator = new CMC_GUI_SelectionListSeparator{};
		return separator->Assemble();
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
			Style = GUI_TextBottom | GUI_TextRight,
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
	
	SetCount = func (int amount)
	{
		if (nil == amount || 0 == amount)
		{
			this.icon.Text = nil;
		}
		else
		{
			this.icon.Text = Format("%dx", amount);
		}
		return this;
	},
	
	SetButtonHint = func (string button)
	{
		var margin_h = GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Margin_H);
		var button_hint_width = GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size);
		var caption_border_right = GuiDimensionCmc(1000)->Subtract(margin_h)->Subtract(button_hint_width);

		var button_hint = new CMC_GUI_ButtonHint{};
		button_hint->Assemble(nil, button);
		button_hint->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size));
		button_hint->AlignLeft(caption_border_right);
		button_hint->AddTo(this);
		button_hint->GetButtonIcon()->AlignCenterV();
		return this;
	},
	
	SetScrollHint = func (bool on)
	{
		var margin_h = GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Margin_H);
		var button_hint_width = GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size);
		var caption_border_right = GuiDimensionCmc(1000)->Subtract(margin_h)->Subtract(button_hint_width);

		var button_hint = new CMC_GUI_ButtonHint{};
		button_hint->Assemble(nil, "MouseMiddle");
		button_hint->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size));
		button_hint->AlignLeft(caption_border_right->Add(button_hint_width));
		button_hint->AddTo(this, nil, "scroll_hint", true);
		button_hint->GetButtonIcon()->AlignCenterV();
		return this;
	},
	
	UpdateEntry = func ()
	{
		// The actual menu
		if (IsSelected())
		{
			this.Symbol = CMC_Icon_ListSelectionHighlight;
		}
		else
		{
			this.Symbol = nil;
		}
		Update({Symbol = this.Symbol});
		// Scroll hint
		if (this.scroll_hint)
		{
			if (IsSelected())
			{
				this.scroll_hint.button_symbol.Symbol = CMC_Icon_Button;
			}
			else
			{
				this.scroll_hint.button_symbol.Symbol = nil;
			}
			Update({scroll_hint = {button_symbol = {Symbol = this.scroll_hint.button_symbol.Symbol}}});
		}
	},
};

static const CMC_GUI_SelectionListSeparator = new GUI_List2_Entry
{
	BackgroundColor = 0x81ffffff,

	Assemble = func ()
	{
		var margin_h = GuiDimensionCmc(nil, GUI_CMC_Element_SelectionList_Margin_H);
		SetLeft(margin_h);
		SetRight(GuiDimensionCmc(1000)->Subtract(margin_h));
		SetHeight(GuiDimensionCmc(nil, 3));
		return this;
	},
	
	SetSelected = func ()
	{
		// Does nothing, but is required by selection list entries. Could call via ->~, too, but this seems to be more prone to errors, such as forgetting the callback.
		// Also, the return value of 'nil' tells the list to select the next entry.
	},
};
