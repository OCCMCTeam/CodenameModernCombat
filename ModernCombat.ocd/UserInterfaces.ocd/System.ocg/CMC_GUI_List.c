
static const CMC_GUI_List = new GUI_List
{
	MakeEntryProplist = func(symbol, text)
	{
		var size = new GUI_Dimension {};
		size->SetEm(20);
		var custom_entry =
		{
			Style = GUI_FitChildren,
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
				Margin = [GuiDimensionCmc(10)->ToString()],
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
