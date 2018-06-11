
static const GUI_CMC_InventoryButton = new GUI_Element
{
	GUI_Compactness = 0,      // int - "compactness" of the GUI element, 0 - 100, 100 means expanded completely
	GUI_Width_Compact = nil,  // int - em unit width of the item when it is compact
	GUI_Width_Expanded = nil, // int - em unit width of the item when it is expanded
	GUI_Item_Name = nil,      // Name of the item

	Assemble = func (int slot_number)
	{
		this.GUI_Width_Compact  = GUI_CMC_Element_Inventory_Width + GUI_CMC_Element_Icon_Size;
		this.GUI_Width_Expanded = GUI_CMC_Element_Info_Width + GUI_CMC_Element_Icon_Size;

		SetWidth(GuiDimensionCmc(nil, GUI_Width_Expanded));
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		
		// Expanding and collapsing bar
		var bar = new GUI_Element
		{
			BackgroundColor =
			{
				Std = GUI_CMC_Background_Color_Default,
				Selected = GUI_CMC_Background_Color_Highlight,
				Empty = GUI_CMC_Background_Color_Inactive,
			},
		};
		bar->SetWidth(GuiDimensionCmc(nil, GUI_Width_Compact))
		   ->AlignRight()
		   ->AddTo(this, this.ID, "bar");
		   
		// Symbol, is attached to the bar
		var item_symbol = new GUI_Element {};
		item_symbol->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Inventory_Width))
		           ->AddTo(bar, nil, "item_symbol");
		
		// Text, is displayed in the same position always
		var item_name = new GUI_Element { Style = GUI_TextHCenter | GUI_TextVCenter | GUI_NoCrop};
		item_name->SetLeft(GuiDimensionCmc(nil, GUI_CMC_Element_Info_Width))       // Left border: Item symbol
		         ->SetRight(GuiDimensionCmc(nil, GUI_CMC_Element_Inventory_Width)) // Right border: Button hint
		         ->AddTo(this, nil, "item_name");

		// Button hint:
		var button_hint = new GUI_Element
		{
			actual_button =
			{
				Margin = GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)->ToString(),
				Symbol = CMC_Icon_Button,
				GraphicsName = Format("%d", slot_number),
			}
		};
		button_hint->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size))
		           ->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size))
		           ->AlignRight()
		           ->AddTo(this);
		return this;
	},
	
	SetInfo = func (object item, bool selected)
	{
		// Update the inventory symbol
		this.bar.item_symbol.Symbol = item;
		this.bar.item_symbol->Update();
		
		// Update the bar
		if (selected)
		{
			this.bar->SetTag("Selected");
		}
		else if (item == nil)
		{
			this.bar->SetTag("Empty");
		}
		else
		{
			this.bar->SetTag(nil);
		}
		this.bar->Update();
		
		// Update the item name
		if (item == nil)
		{
			GUI_Item_Name = nil;
		}
		else
		{
			GUI_Item_Name = item->GetName();
		}
		return this;
	},
};
