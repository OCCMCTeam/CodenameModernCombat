
static const CMC_GUI_List = new GUI_List
{
	MakeEntryProplist = func(symbol, text)
	{
		var size = GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size);
		var custom_entry =
		{
			Bottom = size->ToString(),
			
			BackgroundColor =
			{
				Std = 0,
				OnHover = GUI_CMC_Text_Color_Highlight,
			},
			
			icon =
			{
				Right = size->ToString(),
				Bottom = size->ToString(),
				
				Symbol = symbol,
				Margin = [ToPercentString(100)],
			},
			caption =
			{
				Left = size->ToString(),
				
				Text = text,
				Style = GUI_TextVCenter,
			}
		};
		return custom_entry;
	},
};
